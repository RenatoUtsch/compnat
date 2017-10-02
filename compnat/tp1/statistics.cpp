/*
 * Copyright 2017 Renato Utsch
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "statistics.hpp"

#include <cmath>
#include <fstream>
#include <unordered_set>

#include <glog/logging.h>

#include "compnat/tp1/results/results_generated.h"
#include "utils.hpp"

namespace stats {
namespace {

flatbuffers::Offset<results::Params>
buildParams_(flatbuffers::FlatBufferBuilder &builder,
             const repr::Params &params) {
  results::ParamsBuilder paramsBuilder(builder);
  paramsBuilder.add_seed(params.seed);
  paramsBuilder.add_numInstances(params.numInstances);
  paramsBuilder.add_numGenerations(params.numGenerations);
  paramsBuilder.add_populationSize(params.populationSize);
  paramsBuilder.add_tournamentSize(params.tournamentSize);
  paramsBuilder.add_maxHeight(params.maxHeight);
  paramsBuilder.add_crossoverProb(params.crossoverProb);
  paramsBuilder.add_elitism(params.elitism);
  paramsBuilder.add_alwaysTest(params.alwaysTest);
  return paramsBuilder.Finish();
}
std::pair<double, double>
aggregateParamPair_(const std::vector<std::vector<Statistics>> &allStats,
                    size_t generation,
                    std::function<double(const Statistics &)> &&accessor) {
  double mean = 0;
  for (size_t i = 0; i < allStats.size(); ++i) {
    mean += accessor(allStats[i][generation]);
  }
  mean /= allStats.size();

  double stddev = 0;
  for (size_t i = 0; i < allStats.size(); ++i) {
    stddev += std::pow(accessor(allStats[i][generation]) - mean, 2);
  }
  stddev = std::sqrt(stddev / allStats.size());

  return {mean, stddev};
}

results::meanStddev
aggregateParam_(const std::vector<std::vector<Statistics>> &allStats,
                size_t generation,
                std::function<double(const Statistics &)> &&accessor) {
  auto[mean, stddev] = aggregateParamPair_(
      allStats, generation, std::forward<decltype(accessor)>(accessor));
  return results::meanStddev(mean, stddev);
}

std::tuple<flatbuffers::Offset<flatbuffers::String>, double, size_t>
bestIndividual_(flatbuffers::FlatBufferBuilder &builder,
                const std::vector<std::vector<Statistics>> &allStats,
                size_t generation) {
  size_t best = 0;
  for (size_t i = 0; i < allStats.size(); ++i) {
    if (allStats[i][generation].bestFitness <
        allStats[best][generation].bestFitness) {
      best = i;
    }
  }

  return {builder.CreateString(allStats[best][generation].bestStr),
          allStats[best][generation].bestFitness,
          allStats[best][generation].bestSize};
}

flatbuffers::Offset<results::AggregatedStats>
buildAggregatedStats_(flatbuffers::FlatBufferBuilder &builder,
                      const std::vector<std::vector<Statistics>> &allStats,
                      size_t generation) {
  auto bestFitness = aggregateParam_(
      allStats, generation, [](const auto &s) { return s.bestFitness; });
  auto bestSize = aggregateParam_(allStats, generation,
                                  [](const auto &s) { return s.bestSize; });
  auto worstFitness = aggregateParam_(
      allStats, generation, [](const auto &s) { return s.worstFitness; });
  auto worstSize = aggregateParam_(allStats, generation,
                                   [](const auto &s) { return s.worstSize; });
  auto avgFitness = aggregateParam_(allStats, generation,
                                    [](const auto &s) { return s.avgFitness; });
  auto avgSize = aggregateParam_(allStats, generation,
                                 [](const auto &s) { return s.avgSize; });
  auto numRepeated = aggregateParam_(
      allStats, generation, [](const auto &s) { return s.numRepeated; });
  auto numCrossBetter = aggregateParam_(
      allStats, generation, [](const auto &s) { return s.numCrossBetter; });
  auto numCrossWorse = aggregateParam_(
      allStats, generation, [](const auto &s) { return s.numCrossWorse; });
  auto numMutBetter = aggregateParam_(
      allStats, generation, [](const auto &s) { return s.numMutBetter; });
  auto numMutWorse = aggregateParam_(
      allStats, generation, [](const auto &s) { return s.numMutWorse; });
  auto[bestIndividualStr, bestIndividualFitness, bestIndividualSize] =
      bestIndividual_(builder, allStats, generation);

  results::AggregatedStatsBuilder statsBuilder(builder);
  statsBuilder.add_bestFitness(&bestFitness);
  statsBuilder.add_bestSize(&bestSize);
  statsBuilder.add_worstFitness(&worstFitness);
  statsBuilder.add_worstSize(&worstSize);
  statsBuilder.add_avgFitness(&avgFitness);
  statsBuilder.add_avgSize(&avgSize);
  statsBuilder.add_numRepeated(&numRepeated);
  statsBuilder.add_numCrossBetter(&numCrossBetter);
  statsBuilder.add_numCrossWorse(&numCrossWorse);
  statsBuilder.add_numMutBetter(&numMutBetter);
  statsBuilder.add_numMutWorse(&numMutWorse);
  statsBuilder.add_bestIndividualStr(bestIndividualStr);
  statsBuilder.add_bestIndividualFitness(bestIndividualFitness);
  statsBuilder.add_bestIndividualSize(bestIndividualSize);
  return statsBuilder.Finish();
}

flatbuffers::Offset<
    flatbuffers::Vector<flatbuffers::Offset<results::AggregatedStats>>>
buildAllStats_(flatbuffers::FlatBufferBuilder &builder,
               const std::vector<std::vector<Statistics>> &allStats) {
  std::vector<flatbuffers::Offset<results::AggregatedStats>> aggregatedStats;
  for (size_t i = 0; i < allStats[0].size(); ++i) {
    aggregatedStats.push_back(buildAggregatedStats_(builder, allStats, i));
  }

  return builder.CreateVector(aggregatedStats);
}

void saveToFile_(const std::string &outputFile, const uint8_t *buf,
                 size_t size) {
  std::ofstream out(outputFile, std::ofstream::out | std::ofstream::trunc |
                                    std::ofstream::binary);
  CHECK(out.is_open());

  out.write((const char *)buf, size);
}

} // namespace

double fitness(const repr::Node &individual, const repr::Dataset &dataset) {
  double error = 0;
  for (const auto &sample : dataset) {
    // Unpack outside the loop because clang on MacOS can't compile it in the
    // loop.
    const auto & [ input, expected ] = sample;
    error += std::pow(individual.eval(input) - expected, 2);
  }

  return std::sqrt(error / dataset.size());
}

std::vector<double> fitness(const std::vector<repr::Node> &population,
                            const repr::Dataset &dataset) {
  std::vector<double> results(population.size());
#pragma omp parallel for
  for (size_t i = 0; i < population.size(); ++i) {
    results[i] = fitness(population[i], dataset);
  }

  return results;
}

std::vector<size_t> sizes(const std::vector<repr::Node> &population) {
  std::vector<size_t> sizes(population.size());
  for (size_t i = 0; i < population.size(); ++i) {
    sizes[i] = population[i].size();
  }

  return sizes;
}

Statistics::Statistics(const std::string &statsName,
                       const std::vector<repr::Node> &population,
                       const std::vector<double> &fitnesses,
                       const std::vector<size_t> &sizes,
                       const ImprovementMetadata &metadata)
    : best(0), bestFitness(0), bestSize(0), worst(0), worstFitness(0),
      worstSize(0), avgFitness(0), avgSize(0), numRepeated(0),
      numCrossBetter(-1), numCrossWorse(-1), numMutBetter(-1), numMutWorse(-1) {

  calcFitnessAndSizeStats_(population, fitnesses, sizes);
  calcRepeatedIndividuals_(fitnesses);
  calcImprovementStats_(fitnesses, metadata);

  printStats_(statsName);
}

void Statistics::calcFitnessAndSizeStats_(
    const std::vector<repr::Node> &population,
    const std::vector<double> &fitnesses, const std::vector<size_t> &sizes) {
  for (size_t i = 0; i < fitnesses.size(); ++i) {
    if (fitnesses[best] > fitnesses[i]) {
      best = i;
    }
    if (fitnesses[worst] < fitnesses[i]) {
      worst = i;
    }
    avgFitness += fitnesses[i];
    avgSize += sizes[i];
  }
  avgFitness /= fitnesses.size();
  avgSize /= sizes.size();

  bestFitness = fitnesses[best];
  bestSize = sizes[best];
  bestStr = population[best].str();
  worstFitness = fitnesses[worst];
  worstSize = sizes[worst];
}

void Statistics::calcRepeatedIndividuals_(
    const std::vector<double> &fitnesses) {
  std::unordered_set<double> set;
  for (const auto &fit : fitnesses) {
    if (set.count(fit)) {
      ++numRepeated;
    } else {
      set.insert(fit);
    }
  }
}

void Statistics::calcImprovementStats_(const std::vector<double> &fitnesses,
                                       const ImprovementMetadata &metadata) {
  calcFitnessImprovement_(metadata.crossoverAvgParentFitness, fitnesses,
                          numCrossBetter, numCrossWorse);
  calcFitnessImprovement_(metadata.mutationParentFitness, fitnesses,
                          numMutBetter, numMutWorse);
}

void Statistics::calcFitnessImprovement_(
    const std::vector<std::pair<size_t, double>> &parentFitnesses,
    const std::vector<double> &fitnesses, int &better, int &worse) {
  if (!parentFitnesses.empty()) {
    better = 0;
    worse = 0;
  }

  for (const auto &p : parentFitnesses) {
    const auto & [ childIndex, parentFitness ] = p;
    if (fitnesses[childIndex] < parentFitness) {
      ++better;
    } else if (fitnesses[childIndex] > parentFitness) {
      ++worse;
    }
  }
}

void Statistics::printStats_(const std::string &statsName) {
  using utils::paddedStrCat;
  using utils::strCat;
  const size_t w = 30; // Width of each padded string.

  LOG(INFO) << strCat("  ", statsName, ":");
  LOG(INFO) << paddedStrCat(w, "    best fitness: ", bestFitness)
            << paddedStrCat(w, "| best size: ", bestSize)
            << paddedStrCat(w, "| worst fitness: ", worstFitness)
            << paddedStrCat(w, "| worst size: ", worstSize);
  LOG(INFO) << paddedStrCat(w, "    avgFitness: ", avgFitness)
            << paddedStrCat(w, "| avgSize: ", avgSize)
            << paddedStrCat(w, "| numRepeated: ", numRepeated);

  if (numCrossBetter != -1) {
    LOG(INFO) << paddedStrCat(w, "    numCrossBetter: ", numCrossBetter)
              << paddedStrCat(w, "| numCrossWorse: ", numCrossWorse)
              << paddedStrCat(w, "| numMutBetter: ", numMutBetter)
              << paddedStrCat(w, "| numMutWorse: ", numMutWorse);
  }
}

void saveResults(const repr::Params &params,
                 const std::vector<std::vector<Statistics>> &allTrainStats,
                 const std::vector<std::vector<Statistics>> &allTestStats) {
  flatbuffers::FlatBufferBuilder builder;

  auto resultsParams = buildParams_(builder, params);
  auto resultsTrainStats = buildAllStats_(builder, allTrainStats);
  auto resultsTestStats = params.alwaysTest
                              ? buildAllStats_(builder, allTestStats)
                              : buildAllStats_(builder, {{}});
  auto resultsFinalStats =
      buildAggregatedStats_(builder, allTestStats, allTestStats[0].size() - 1);

  results::ResultsBuilder resultsBuilder(builder);
  resultsBuilder.add_params(resultsParams);
  resultsBuilder.add_trainStats(resultsTrainStats);
  resultsBuilder.add_testStats(resultsTestStats);
  resultsBuilder.add_finalStats(resultsFinalStats);
  builder.Finish(resultsBuilder.Finish());

  saveToFile_(params.outputFile, builder.GetBufferPointer(), builder.GetSize());

  auto[finalMeanFitness, finalFitnessError] =
      aggregateParamPair_(allTestStats, allTestStats[0].size() - 1,
                          [](const auto &s) { return s.bestFitness; });
  auto[finalMeanSize, finalSizeError] =
      aggregateParamPair_(allTestStats, allTestStats[0].size() - 1,
                          [](const auto &s) { return s.bestSize; });
  LOG(INFO) << "";
  LOG(INFO) << "Final results: ";
  LOG(INFO) << "  best fitness: " << finalMeanFitness << " +/- "
            << finalFitnessError;
  LOG(INFO) << "  best size: " << finalMeanSize << " +/- " << finalSizeError;
}

} // namespace stats

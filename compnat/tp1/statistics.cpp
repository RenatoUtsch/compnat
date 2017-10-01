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
#include <unordered_set>

#include <glog/logging.h>

#include "utils.hpp"

namespace stats {

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

std::vector<double> fitness(threading::ThreadPool &pool,
                            const std::vector<repr::Node> &population,
                            const repr::Dataset &dataset) {
  std::vector<double> results(population.size());
  pool.run(0, population.size(), [&results, &population, &dataset](size_t i) {
    results[i] = fitness(population[i], dataset);
  });

  return results;
}

std::vector<size_t> sizes(const std::vector<repr::Node> &population) {
  std::vector<size_t> sizes(population.size());
  for (size_t i = 0; i < population.size(); ++i) {
    sizes[i] = population[i].size();
  }

  return sizes;
}

#if 0
flatbuffers::Offset<stats::GenerationStats>
buildGenerationStats(flatbuffers::FlatBufferBuilder &builder,
                     const Statistics &stats) {
  stats::GenerationStatsBuilder statsBuilder(builder);
  statsBuilder.add_bestFitness(stats.bestFitness);
  statsBuilder.add_bestSize(stats.bestSize);
  statsBuilder.add_bestStr(builder.CreateString(stats.bestStr));
  statsBuilder.add_worstFitness(stats.worstFitness);
  statsBuilder.add_worstSize(stats.worstSize);
  statsBuilder.add_avgFitness(stats.avgFitness);
  statsBuilder.add_avgSize(stats.avgSize);
  statsBuilder.add_numRepeated(stats.numRepeated);
  statsBuilder.add_numCrossoverBetter(stats.numCrossBetter);
  statsBuilder.add_numCrossoverWorse(stats.numCrossWorse);
  statsBuilder.add_numMutationBetter(stats.numMutBetter);
  statsBuilder.add_numMutationWorse(stats.numMutWorse);
  return statsBuilder.Finish();
}
#endif

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

} // namespace stats

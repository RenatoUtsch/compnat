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

Statistics::Statistics(threading::ThreadPool &pool,
                       const std::vector<repr::Node> &population,
                       const repr::Dataset &dataset,
                       const ImprovementMetadata &metadata)
    : fitness(::stats::fitness(pool, population, dataset)),
      sizes(::stats::sizes(population)), best(0), worst(0), averageFitness(0),
      averageSize(0), numRepeatedIndividuals(0), numCrossoverBetter(0),
      numCrossoverWorse(0), numMutationBetter(0), numMutationWorse(0) {

  calcFitnessStats_();
  calcRepeatedIndividuals_();
  calcAverageSize_();
  calcImprovementStats_(metadata);

  bestStr = population[best].str();

  printStats_(metadata);
}

void Statistics::calcFitnessStats_() {
  for (size_t i = 0; i < fitness.size(); ++i) {
    if (fitness[best] > fitness[i]) {
      best = i;
    }
    if (fitness[worst] < fitness[i]) {
      worst = i;
    }
    averageFitness += fitness[i];
  }
  averageFitness /= fitness.size();
}

void Statistics::calcRepeatedIndividuals_() {
  std::unordered_set<double> set;
  for (const auto &fit : fitness) {
    if (set.count(fit)) {
      ++numRepeatedIndividuals;
    } else {
      set.insert(fit);
    }
  }
}

void Statistics::calcAverageSize_() {
  for (auto size : sizes) {
    averageSize += size;
  }
  averageSize /= sizes.size();
}

void Statistics::calcImprovementStats_(const ImprovementMetadata &metadata) {
  calcFitnessImprovement_(metadata.crossoverAvgParentFitness,
                          numCrossoverBetter, numCrossoverWorse);
  calcFitnessImprovement_(metadata.mutationParentFitness, numMutationBetter,
                          numMutationWorse);
}

void Statistics::calcFitnessImprovement_(
    const std::vector<std::pair<size_t, double>> &fitnesses, size_t &better,
    size_t &worse) {
  for (const auto &p : fitnesses) {
    const auto & [ childIndex, parentFitness ] = p;
    if (fitness[childIndex] < parentFitness) {
      ++better;
    } else if (fitness[childIndex] > parentFitness) {
      ++worse;
    }
  }
}

void Statistics::printStats_(const ImprovementMetadata &metadata) {
  using utils::paddedStrCat;
  const size_t w = 30; // Width of each padded string.

  LOG(INFO) << paddedStrCat(w, "  best fitness: ", fitness[best])
            << paddedStrCat(w, "| best size: ", sizes[best])
            << paddedStrCat(w, "| worst fitness: ", fitness[worst])
            << paddedStrCat(w, "| worst size: ", sizes[worst]);
  LOG(INFO) << paddedStrCat(w, "  avgFitness: ", averageFitness)
            << paddedStrCat(w, "| avgSize: ", averageSize)
            << paddedStrCat(w, "| numRepeated: ", numRepeatedIndividuals);

  if (!metadata.crossoverAvgParentFitness.empty() &&
      !metadata.mutationParentFitness.empty()) {
    LOG(INFO) << paddedStrCat(w, "  numCrossBetter: ", numCrossoverBetter)
              << paddedStrCat(w, "| numCrossWorse: ", numCrossoverWorse)
              << paddedStrCat(w, "| numMutBetter: ", numMutationBetter)
              << paddedStrCat(w, "| numMutWorse: ", numMutationWorse);
  }
}

} // namespace stats

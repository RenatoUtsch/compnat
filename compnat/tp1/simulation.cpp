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

#include "simulation.hpp"

#include <utility>
#include <vector>

#include "glog/logging.h"

#include "generators.hpp"
#include "operators.hpp"
#include "statistics.hpp"

namespace {
std::pair<std::vector<stats::Statistics>, std::vector<stats::Statistics>>
simulateGeneration_(repr::RNG &rng, const repr::Params &params,
                    const repr::Dataset &trainDataset,
                    const repr::Dataset &testDataset) {
  std::vector<stats::Statistics> trainStats;
  std::vector<stats::Statistics> testStats;

  LOG(INFO) << "Generation 0";
  auto population = generators::rampedHalfAndHalf(rng, params);

  auto fitnesses = stats::fitness(population, trainDataset);
  auto sizes = stats::sizes(population);

  trainStats.emplace_back("Train", population, fitnesses, sizes);
  if (params.alwaysTest) {
    testStats.emplace_back("Test", population,
                           stats::fitness(population, testDataset), sizes);
  }

  stats::ImprovementMetadata metadata;
  for (size_t i = 1; i <= params.numGenerations; ++i) {
    LOG(INFO) << "Generation " << i;
    std::tie(population, metadata) = operators::newGeneration(
        rng, params, population, fitnesses, sizes, trainStats[i - 1]);

    fitnesses = stats::fitness(population, trainDataset);
    sizes = stats::sizes(population);

    trainStats.emplace_back("Train", population, fitnesses, sizes, metadata);
    if (params.alwaysTest || i == params.numGenerations) {
      // Always save test stats for the last generation.
      testStats.emplace_back("Test", population,
                             stats::fitness(population, testDataset), sizes);
    }
  }

  return {trainStats, testStats};
}

} // namespace

namespace simulation {

std::pair<std::vector<std::vector<stats::Statistics>>,
          std::vector<std::vector<stats::Statistics>>>
simulate(const repr::Params &params, const repr::Dataset &trainDataset,
         const repr::Dataset &testDataset) {
  repr::RNG rng(params.seed);

  std::vector<std::vector<stats::Statistics>> allTrainStats;
  std::vector<std::vector<stats::Statistics>> allTestStats;
  for (size_t i = 1; i <= params.numInstances; ++i) {
    LOG(INFO) << "";
    LOG(INFO) << "";
    LOG(INFO) << "INSTANCE " << i;
    LOG(INFO) << "";
    LOG(INFO) << "";

    auto[trainStats, testStats] =
        simulateGeneration_(rng, params, trainDataset, testDataset);
    allTrainStats.push_back(std::move(trainStats));
    allTestStats.push_back(std::move(testStats));
  }

  return {allTrainStats, allTestStats};
}

} // namespace simulation

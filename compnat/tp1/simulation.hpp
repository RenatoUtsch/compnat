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

#ifndef COMPNAT_TP1_SIMULATION_HPP
#define COMPNAT_TP1_SIMULATION_HPP

#include <thread>

#include <glog/logging.h>

#include "generators.hpp"
#include "operators.hpp"
#include "representation.hpp"
#include "statistics.hpp"
#include "threading.hpp"

namespace simulation {

template <typename T, class RNG>
void simulate(const Params<T, RNG> &params, const Dataset<T> &trainDataset,
              const Dataset<T> &testDataset) {
  RNG rng(params.seed);
  threading::ThreadPool pool;

  LOG(INFO) << "";
  LOG(INFO) << "Generation 0";
  auto population = generators::rampedHalfAndHalf(rng, params);
  auto stats = stats::Statistics<T, RNG>(pool, population, trainDataset);
  stats::ImprovementMetadata metadata;
  for (size_t i = 1; i <= params.numGenerations; ++i) {
    LOG(INFO) << "Generation " << i;
    std::tie(population, metadata) =
        operators::newGeneration(rng, params, population, stats);
    stats = stats::Statistics<T, RNG>(pool, population, trainDataset, metadata);
  }

  LOG(INFO) << "Test statistics";
  auto testStats = stats::Statistics<T, RNG>(pool, population, testDataset);
}

} // namespace simulation

#endif // !COMPNAT_TP1_SIMULATION_HPP

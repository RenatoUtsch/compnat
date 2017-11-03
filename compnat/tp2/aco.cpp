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

#include "aco.hpp"

#include <functional>
#include <numeric>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

#include <glog/logging.h>

#include "gap.hpp"

namespace tp2 {
namespace {

size_t selectPoint_(RNG &rng, const std::set<size_t> &unselected,
                    const std::vector<float> &pheromones) {
  const float sum =
      std::accumulate(unselected.begin(), unselected.end(), 0.0f,
                      [&](float a, size_t b) { return a + pheromones[b]; });
  std::uniform_real_distribution<float> distr(0.0f, sum);

  const float p = distr(rng);
  float boundary = 0.0f;
  for (size_t i : unselected) {
    boundary += pheromones[i];
    if (p <= boundary) {
      return i;
    }
  }

  LOG(FATAL) << "Should never get here, one prob should be selected.";
}

/**
 * Selects the medians, returns pair of <clients, medians>.
 */
std::pair<std::vector<size_t>, std::vector<size_t>>
selectMedians_(RNG &rng, const std::vector<float> &pheromones,
               size_t numMedians, size_t numPoints) {
  std::set<size_t> unselected;
  for (size_t i = 0; i < numPoints; ++i) {
    unselected.insert(i);
  }

  std::vector<size_t> medians;
  for (size_t i = 0; i < numMedians; ++i) {
    const size_t p = selectPoint_(rng, unselected, pheromones);
    unselected.erase(p);
    medians.push_back(p);
  }

  return {std::vector<size_t>(unselected.begin(), unselected.end()), medians};
}

} // namespace

void aco(RNG &rng, const Dataset &dataset, int numIterations, int numAnts) {
  CHECK(numIterations > 0);
  if (numAnts < 0) {
    numAnts = dataset.numPoints() - dataset.numMedians();
  }

  std::vector<float> pheromones(dataset.numPoints(), 0.5f);

  for (int i = 0; i < numIterations; ++i) {
    for (int j = 0; j < numAnts; ++j) {
      const auto[clients, medians] = selectMedians_(
          rng, pheromones, dataset.numMedians(), dataset.numPoints());
      const auto assignedMedians = gap(dataset, clients, medians);
    }
  }
}

} // namespace tp2

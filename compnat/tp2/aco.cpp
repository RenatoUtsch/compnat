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
#include <vector>

#include <glog/logging.h>

namespace tp2 {
namespace {

std::vector<float> calcProbabilities_(const std::set<size_t> &unselected,
                                      const std::vector<float> &pheromones) {
  const float sum =
      std::accumulate(unselected.begin(), unselected.end(), 0.0f,
                      [&](float a, size_t b) { return a + pheromones[b]; });

  std::vector<std::pair<size_t, float>> probabilities;
  for (size_t i : unselected) {
    probabilities.emplace_back(i, pheromones[i] / sum);
  }

  return probabilities;
}

size_t selectPoint_(RNG &rng, const std::set<size_t> &unselected,
                    const std::vector<float> &pheromones) {
  const auto probabilities = calcProbabilities_(unselected, pheromones);
  std::uniform_real_distribution<float> distr(0.0f, 1.0f);

  const float p = distr(rng);
  float boundary = 0.0f;
  for (const auto &probPair : probabilities) {
    const auto[i, prob] = probPair;
    boundary += prob;
    if (p <= boundary) {
      return p;
    }
  }

  LOG(ERROR) << "Should never get here, one prob should be selected.";
}

std::vector<std::reference_wrapper<Point>>
selectMedians_(const Dataset &dataset, const std::vector<float> &pheromones) {
  std::vector<std::reference_wrapper<Point>> medians;
  std::set<size_t> unselected(dataset.points().begin(), dataset.points().end());

  for (size_t k = 0; k < dataset.numMedians(); ++k) {
    const size_t p = selectPoint_(unselected, pheromones);
    unselected.erase(p);
    medians.emplace_back(dataset.point(p));
  }

  return medians;
}

} // namespace

void aco(const Dataset &dataset, int numAnts, int numIterations,
         unsigned seed) {
  CHECK(numIterations > 0);
  if (numAnts < 0) {
    numAnts = dataset.numPoints() - dataset.numMedians();
  }

  std::vector<float> pheromones(dataset.numPoints(), 0.5f);

  for (int i = 0; i < numIterations; ++i) {
    for (int j = 0; j < numAnts; ++j) {
      const auto medians = selectMedians_(dataset, pheromones);
    }
  }
}

} // namespace tp2

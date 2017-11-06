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

#include <cmath>
#include <functional>
#include <limits>
#include <numeric>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

#include <glog/logging.h>

#include "gap.hpp"

namespace tp2 {
namespace {

const float TInitial = 0.5f;
const float TMin = 0.001f;
const float TMax = 0.999f;
const float StagnationThreshold = 0.5f;

/**
 * Returns vector from client to distances to that client.
 */
std::vector<std::vector<float>> calcDistances_(const Dataset &dataset) {
  std::vector<std::vector<float>> distances(dataset.numPoints());
  std::generate(distances.begin(), distances.end(), [&]() {
    return std::vector<float>(dataset.numPoints(), 0.0f);
  });

  for (size_t i = 0; i < dataset.numPoints(); ++i) {
    for (size_t j = i + 1; j < dataset.numPoints(); ++j) {
      distances[i][j] =
          glm::distance(dataset.point(i).position, dataset.point(j).position);
      distances[j][i] = distances[i][j];
    }
  }

  return distances;
}

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

/// Returns the local best and local worst solutions.
std::pair<Solution, Solution>
findBestAndWorstSolutions_(const std::vector<Solution> &solutions) {
  Solution localBest, localWorst(FLT_MIN);
  for (const auto &solution : solutions) {
    if (solution.distance < localBest.distance) {
      localBest = solution;
    } else if (solution.distance > localWorst.distance) {
      localWorst = solution;
    }
  }

  return {localBest, localWorst};
}

/// Updates the pheromones vector and global best if needed.
/// localBest should not be used afterwards, as it may be moved.
void updatePheromones_(std::vector<float> &pheromones, float decay,
                       Solution &globalBest, Solution &localBest,
                       const Solution &localWorst) {
  std::set<size_t> medians(globalBest.medians.begin(),
                           globalBest.medians.end());
  for (const size_t median : localBest.medians) {
    if (!medians.count(median)) {
      medians.insert(median);
    }
  }

  for (const size_t median : medians) {
    const float delta = 1.0f - ((localBest.distance - globalBest.distance) /
                                (localWorst.distance - localBest.distance));
    const float update = decay * (delta - pheromones[median]);
    pheromones[median] =
        std::min(TMax, std::max(TMin, pheromones[median] + update));
  }

  if (localBest.distance < globalBest.distance) {
    globalBest = std::move(localBest);
  }
}

/// Restarts the pheromones vector if the algorithm has stagnated.
/// This happens every time the medians are at TMax and the rest at TMin.
void stagnationControl_(std::vector<float> &pheromones, size_t numPoints,
                        size_t numMedians) {
  const float sum = std::accumulate(pheromones.begin(), pheromones.end(), 0.0f);
  const float stagnationSum =
      numMedians * TMax + (numPoints - numMedians) * TMin;

  if (std::abs(sum - stagnationSum) < StagnationThreshold) {
    // Reset pheromones
    LOG(INFO) << "Algorithm stagnated, resetting";
    std::generate(pheromones.begin(), pheromones.end(),
                  []() { return TInitial; });
  }
}

} // namespace

Result aco(RNG &rng, const Dataset &dataset, int numIterations, int numAnts,
           float decay) {
  CHECK(numIterations > 0);
  CHECK(numAnts > 0);

  const auto &distances = calcDistances_(dataset);
  std::vector<float> pheromones(dataset.numPoints(), TInitial);

  std::vector<float> globalBests(numIterations);
  std::vector<float> localBests(numIterations);
  std::vector<float> localWorsts(numIterations);
  Solution globalBest;
  for (int i = 0; i < numIterations; ++i) {
    std::vector<Solution> solutions;
    for (int j = 0; j < numAnts; ++j) {
      auto[clients, medians] = selectMedians_(
          rng, pheromones, dataset.numMedians(), dataset.numPoints());
      const float distance = gap(dataset, clients, medians, distances);
      solutions.emplace_back(distance, std::move(medians));
    }

    auto[localBest, localWorst] = findBestAndWorstSolutions_(solutions);
    localBests[i] = localBest.distance;
    localWorsts[i] = localWorst.distance;

    LOG(INFO) << "  Iteration " << i
              << "\t| globalBest: " << globalBest.distance
              << "\t| localBest: " << localBest.distance
              << "\t| localWorst: " << localWorst.distance;

    updatePheromones_(pheromones, decay, globalBest, localBest, localWorst);
    stagnationControl_(pheromones, dataset.numPoints(), dataset.numMedians());
    globalBests[i] = globalBest.distance;
  }

  return Result(std::move(globalBests), std::move(localBests),
                std::move(localWorsts));
}

} // namespace tp2

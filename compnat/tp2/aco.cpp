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

/**
 * Allocates clients to the given potential median, until it is full. Returns
 * the heuristic weight.
 */
float informationHeuristicAllocate_(
    size_t median, const std::vector<size_t> &points,
    const std::vector<std::vector<float>> &distances, const Dataset &dataset) {
  size_t numNodes = 0;
  float sumDistance = 0.0f;
  float capacity =
      dataset.point(median).capacity - dataset.point(median).demand;
  for (size_t point : points) {
    if (median == point) {
      continue;
    }
    if (capacity <= 1e-6f) {
      break;
    }

    const float demand = dataset.point(point).demand;
    if (capacity - demand >= 0.0f) {
      ++numNodes;
      sumDistance += distances[median][point];
      capacity -= demand;
    }
  }

  return (float)numNodes / sumDistance;
}

/**
 * Calculates the information heuristic weight for all points.
 */
std::vector<float>
informationHeuristic_(const Dataset &dataset,
                      const std::vector<std::vector<float>> &distances) {
  std::vector<size_t> points(dataset.numPoints());
  std::iota(points.begin(), points.end(), 0);

  std::vector<float> heuristicWeights(dataset.numPoints());
  for (size_t i = 0; i < dataset.numPoints(); ++i) {
    std::sort(points.begin(), points.end(),
              [&](size_t a, size_t b) { return distances[a] < distances[b]; });

    heuristicWeights[i] =
        informationHeuristicAllocate_(i, points, distances, dataset);
  }

  return heuristicWeights;
}

size_t selectPoint_(RNG &rng, const std::set<size_t> &unselected,
                    const std::vector<float> &unselectedWeights) {
  const float sum = std::accumulate(
      unselected.begin(), unselected.end(), 0.0f,
      [&](float a, size_t b) { return a + unselectedWeights[b]; });
  std::uniform_real_distribution<float> distr(0.0f, sum);

  const float p = distr(rng);
  float boundary = 0.0f;
  for (size_t i : unselected) {
    boundary += unselectedWeights[i];
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
               const std::vector<float> &heuristics, float alpha, float beta,
               size_t numMedians, size_t numPoints) {
  std::set<size_t> unselected;
  std::vector<float> unselectedWeights(numPoints);
  for (size_t i = 0; i < numPoints; ++i) {
    unselected.insert(i);
    unselectedWeights[i] =
        std::pow(pheromones[i], alpha) * std::pow(heuristics[i], beta);
  }

  std::vector<size_t> medians;
  for (size_t i = 0; i < numMedians; ++i) {
    const size_t p = selectPoint_(rng, unselected, unselectedWeights);
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
  for (const size_t median : localBest.medians) {
    const float delta = 1.0f - ((localBest.distance - globalBest.distance) /
                                (localWorst.distance - localBest.distance));
    const float update = decay * (delta - pheromones[median]);
    pheromones[median] += std::min(TMax, std::max(TMin, update));
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

void aco(RNG &rng, const Dataset &dataset, int numIterations, int numAnts,
         float decay, float alpha, float beta) {
  CHECK(numIterations > 0);
  if (numAnts < 0) {
    numAnts = dataset.numPoints() - dataset.numMedians();
  }

  const auto &distances = calcDistances_(dataset);
  const auto &heuristics = informationHeuristic_(dataset, distances);

  std::vector<float> pheromones(dataset.numPoints(), TInitial);

  Solution globalBest;
  for (int i = 0; i < numIterations; ++i) {
    std::vector<Solution> solutions;
    for (int j = 0; j < numAnts; ++j) {
      auto[clients, medians] =
          selectMedians_(rng, pheromones, heuristics, alpha, beta,
                         dataset.numMedians(), dataset.numPoints());
      const float distance = gap(dataset, clients, medians, distances);
      solutions.emplace_back(distance, std::move(medians));
    }

    auto[localBest, localWorst] = findBestAndWorstSolutions_(solutions);
    LOG(INFO) << "  Iteration " << i
              << "\t| globalBest: " << globalBest.distance
              << "\t| localBest: " << localBest.distance
              << "\t| localWorst: " << localWorst.distance;

    updatePheromones_(pheromones, decay, globalBest, localBest, localWorst);
    stagnationControl_(pheromones, dataset.numPoints(), dataset.numMedians());
  }
}

} // namespace tp2

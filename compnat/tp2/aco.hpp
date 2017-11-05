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

#ifndef COMPNAT_TP2_ACO_HPP
#define COMPNAT_TP2_ACO_HPP

#include "representation.hpp"

#include <limits>

namespace tp2 {

/// Represents a solution built with the algorithm.
struct Solution {
  /// Sum of distances of all clients to the medians.
  float distance;

  /// The medians selected in the solution.
  std::vector<size_t> medians;

  Solution() : distance(FLT_MAX) {}

  Solution(const Solution &other) = default;
  Solution &operator=(const Solution &other) = default;

  Solution(float distance, std::vector<size_t> &&medians = {})
      : distance(distance), medians(std::move(medians)) {}
};

/// Represents the result for all generations.
struct Result {
  std::vector<float> globalBests, localBests, localWorsts;

  Result() {}
  Result(Result &&other) = default;
  Result &operator=(Result &&other) = default;
  Result(std::vector<float> &&globalBests, std::vector<float> &&localBests,
         std::vector<float> &&localWorsts)
      : globalBests(globalBests), localBests(localBests),
        localWorsts(localWorsts) {}
};

Result aco(RNG &rng, const Dataset &dataset, int numIterations, int numAnts,
           float decay);

} // namespace tp2

#endif // !COMPNAT_TP2_ACO_HPP

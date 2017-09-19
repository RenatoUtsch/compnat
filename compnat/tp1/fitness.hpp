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

#ifndef COMPNAT_TP1_FITNESS_HPP
#define COMPNAT_TP1_FITNESS_HPP

#include <cmath>
#include <utility>
#include <vector>

#include "representation.hpp"

/**
 * Calculates the fitness of an individual given a set of input data.
 * This is implemented as the Root-mean-square deviation.
 */
template <typename T>
T fitness(const Node<T> &individual,
          const std::vector<std::pair<EvalInput<T>, T>> &data) {
  T error = 0;
  for (const auto & [ input, expected ] : data) {
    error += std::pow(individual.eval(input) - expected, 2);
  }

  return std::sqrt(error / data.size());
}

/**
 * Calculates the fitness for all individuals and returns it in a vector.
 * This vector is in the same order of the individuals vector.
 */
template <typename T>
std::vector<T> fitness(const std::vector<Node<T>> &individuals,
                       const std::vector<std::pair<EvalInput<T>, T>> &data) {
  // TODO(renatoutsch): implement this in parallel.
  std::vector<T> results(individuals.size());
  for (size_t i = 0; i < individuals.size(); ++i) {
    results[i] = fitness(individuals[i], data);
  }

  return results;
}

#endif // !COMPNAT_TP1_FITNESS_HPP

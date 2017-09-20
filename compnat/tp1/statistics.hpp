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

#ifndef COMPNAT_TP1_STATISTICS_HPP
#define COMPNAT_TP1_STATISTICS_HPP

#include <cmath>

#include "representation.hpp"

namespace stats {

/**
 * Calculates the fitness of an individual given a set of input data.
 * This is implemented as the Root-mean-square deviation.
 */
template <typename T, class RNG>
T fitness(const Node<T, RNG> &individual, const Dataset<T> &dataset) {
  T error = 0;
  for (const auto &sample : dataset) {
    // Unpack outside the loop because clang on MacOS can't compile it in the
    // loop.
    const auto & [ input, expected ] = sample;
    error += std::pow(individual.eval(input) - expected, 2);
  }

  return std::sqrt(error / dataset.size());
}

/**
 * Calculates the fitness for all population and returns it in a vector.
 * This vector is in the same order of the population vector.
 */
template <typename T, class RNG>
std::vector<T> fitness(const std::vector<Node<T, RNG>> &population,
                       const Dataset<T> &dataset) {
  // TODO(renatoutsch): implement this in parallel.
  std::vector<T> results(population.size());
  for (size_t i = 0; i < population.size(); ++i) {
    results[i] = fitness(population[i], dataset);
  }

  return results;
}

/**
 * Calculates the size for all the population.
 */
template <typename T, class RNG>
std::vector<size_t> sizes(const std::vector<Node<T, RNG>> &population) {
  // TODO(renatoutsch): implement this in parallel.
  std::vector<size_t> sizes(population.size());
  for (size_t i = 0; i < population.size(); ++i) {
    sizes[i] = population[i].size();
  }

  return sizes;
}

/**
 * Converts all the population to string version.
 */
template <typename T, class RNG>
std::vector<std::string> strs(const std::vector<Node<T, RNG>> &population) {
  // TODO(renatoutsch): implement this in parallel.
  std::vector<std::string> texts(population.size());
  for (size_t i = 0; i < population.size(); ++i) {
    texts[i] = population[i].str();
  }

  return texts;
}

} // namespace stats

#endif // !COMPNAT_TP1_STATISTICS_HPP

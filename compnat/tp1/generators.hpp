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

#ifndef COMPNAT_TP1_GENERATORS_HPP
#define COMPNAT_TP1_GENERATORS_HPP

#include <glog/logging.h>
#include <random>
#include <stack>
#include <utility>
#include <vector>

#include "representation.hpp"

/// Returns a random primitive.
template <typename T, class RNG>
const Primitive<T> &
randomPrimitive(RNG &rng, const std::vector<Primitive<T>> &primitives) {
  const std::uniform_int_distribution<size_t> distr(0, primitives.size() - 1);
  const auto primitive = distr(rng);

  return primitives[primitive];
}

/// Returns a random function or terminal.
template <typename T, class RNG>
const Primitive<T> &
randomPrimitive(RNG &rng, const std::vector<Primitive<T>> &functions,
                const std::vector<Primitive<T>> &terminals) {
  const auto numPrimitives = functions.size() + terminals.size();
  const std::uniform_int_distribution<size_t> distr(0, numPrimitives - 1);
  const auto primitive = distr(rng);

  if (primitive < functions.size()) {
    return functions[primitive];
  } else {
    return terminals[primitive - functions.size()];
  }
}

/**
 * Implements the grow method for creating trees.
 * @param rng Random number generator.
 * @param maxHeight The maximum height of the generated tree.
 * @param functions Function primitives.
 * @param terminals Terminal primitives.
 */
template <typename T, class RNG>
Node<T> grow(RNG &rng, int maxHeight,
             const std::vector<Primitive<T>> &functions,
             const std::vector<Primitive<T>> &terminals) {
  Node<T> root(randomPrimitive(rng, functions, terminals));

  std::stack<std::pair<Node<T> *, int>> nodes;
  nodes.emplace(&root, 1);
  while (!nodes.empty()) {
    auto[node, height] = nodes.pop();
    if (node.isTerminal()) {
      continue;
    } else if (height >= maxHeight - 1) {
      for (int i = 0; i < node.numChildren(); ++i) {
        node.setChild(i, Node<T>(randomPrimitive(rng, terminals)));
      }
      continue;
    }

    for (int i = 0; i < node.numChildren(); ++i) {
      node.setChild(i, Node<T>(randomPrimitive(rng, functions, terminals)));
      nodes.emplace(&node.child(i), height + 1);
    }
  }

  return root;
}

/**
 * Implements the full method for creating trees.
 * @param rng Random number generator.
 * @param maxHeight The maximum height of the generated tree.
 * @param functions Function primitives.
 * @param terminals Terminal primitives.
 */
template <typename T, class RNG>
Node<T> full(RNG &rng, int maxHeight,
             const std::vector<Primitive<T>> &functions,
             const std::vector<Primitive<T>> &terminals) {
  Node<T> root(randomPrimitive(rng, functions));

  std::stack<std::pair<Node<T> *, int>> nodes;
  nodes.emplace(&root, 1);
  while (!nodes.empty()) {
    auto[node, height] = nodes.pop();
    if (height >= maxHeight - 1) {
      for (int i = 0; i < node.numChildren(); ++i) {
        node.setChild(i, Node<T>(randomPrimitive(rng, terminals)));
      }
      continue;
    }

    for (int i = 0; i < node.numChildren(); ++i) {
      node.setChild(i, Node<T>(randomPrimitive(rng, functions)));
      nodes.emplace(&node.child(i), height + 1);
    }
  }

  return root;
}

/**
 * Generates trees using the ramped half and half method.
 * @param rng Random number generator.
 * @param maxHeight The maximum height of the generated tree.
 * @param population Number of individuals to generate. This number will be
 *   increased if it is not even or a multiple of (maxHeight - 1).
 * @param functions Function primitives.
 * @param terminals Terminal primitives.
 */
template <typename T, class RNG>
std::vector<Node<T>>
rampedHalfAndHalf(RNG &rng, int maxHeight, int population,
                  const std::vector<Primitive<T>> &functions,
                  const std::vector<Primitive<T>> &terminals) {
  const int increase = population % (maxHeight - 1);
  if (increase) { // Make the population a multiple of (maxHeight - 1).
    LOG(WARNING) << "rampedHalfAndHalf: population increase - " << increase;
    population += maxHeight - increase;
  }
  if ((population / (maxHeight - 1)) % 2) { // Needs to be even.
    LOG(WARNING) << "rampedHalfAndHalf: population increase to make buckets "
                 << "even - " << maxHeight - 1;
    population += maxHeight - 1;
  }
  LOG(INFO) << "rampedHalfAndHalf: final population size - " << population;

  const int halfPopulationPerHeight = population / (maxHeight - 1) / 2;
  std::vector<Node<T>> nodes;
  for (int i = 2; i <= maxHeight; ++i) {
    for (int j = 0; j < halfPopulationPerHeight; ++j) {
      nodes.emplace(grow(rng, i, functions, terminals));
      nodes.emplace(full(rng, i, functions, terminals));
    }
  }

  return nodes;
}

#endif // !COMPNAT_TP1_GENERATORS_HPP

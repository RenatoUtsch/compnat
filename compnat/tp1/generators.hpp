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
const Primitive<T, RNG> &
randomPrimitive(RNG &rng, const std::vector<Primitive<T, RNG>> &primitives) {
  const std::uniform_int_distribution<size_t> distr(0, primitives.size() - 1);
  const auto primitive = distr(rng);

  return primitives[primitive];
}

/// Returns a random function or terminal.
template <typename T, class RNG>
const Primitive<T, RNG> &
randomPrimitive(RNG &rng, const std::vector<Primitive<T, RNG>> &functions,
                const std::vector<Primitive<T, RNG>> &terminals) {
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
Node<T, RNG> grow(RNG &rng, int maxHeight,
                  const std::vector<Primitive<T, RNG>> &functions,
                  const std::vector<Primitive<T, RNG>> &terminals) {
  Node<T, RNG> root(randomPrimitive(rng, functions, terminals));

  std::stack<std::pair<Node<T, RNG> *, int>> nodes;
  nodes.emplace(&root, 1);
  while (!nodes.empty()) {
    auto[node, height] = nodes.pop();
    if (node.isTerminal()) {
      continue;
    } else if (height >= maxHeight - 1) {
      for (int i = 0; i < node.numChildren(); ++i) {
        node.setChild(i, Node<T, RNG>(randomPrimitive(rng, terminals)));
      }
      continue;
    }

    for (int i = 0; i < node.numChildren(); ++i) {
      node.setChild(i,
                    Node<T, RNG>(randomPrimitive(rng, functions, terminals)));
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
Node<T, RNG> full(RNG &rng, int maxHeight,
                  const std::vector<Primitive<T, RNG>> &functions,
                  const std::vector<Primitive<T, RNG>> &terminals) {
  Node<T, RNG> root(randomPrimitive(rng, functions));

  std::stack<std::pair<Node<T, RNG> *, int>> nodes;
  nodes.emplace(&root, 1);
  while (!nodes.empty()) {
    auto[node, height] = nodes.pop();
    if (height >= maxHeight - 1) {
      for (int i = 0; i < node.numChildren(); ++i) {
        node.setChild(i, Node<T, RNG>(randomPrimitive(rng, terminals)));
      }
      continue;
    }

    for (int i = 0; i < node.numChildren(); ++i) {
      node.setChild(i, Node<T, RNG>(randomPrimitive(rng, functions)));
      nodes.emplace(&node.child(i), height + 1);
    }
  }

  return root;
}

/**
 * Generates trees using the ramped half and half method.
 * @param params Genetic Programming parameters.
 * @param rng Random number generator.
 */
template <typename T, class RNG>
std::vector<Node<T, RNG>> rampedHalfAndHalf(Params<T, RNG> &params, RNG &rng) {
  const int increase = params.populationSize % (params.maxHeight - 1);
  if (increase) { // Make the population a multiple of (maxHeight - 1).
    LOG(WARNING) << "rampedHalfAndHalf: population increase - " << increase;
    params.populationSize += params.maxHeight - increase;
  }
  if ((params.populationSize / (params.maxHeight - 1)) % 2) {
    // Params needs to be even.
    LOG(WARNING) << "rampedHalfAndHalf: population increase to make buckets "
                 << "even - " << params.maxHeight - 1;
    params.populationSize += params.maxHeight - 1;
  }
  LOG(INFO) << "rampedHalfAndHalf: final population size - "
            << params.populationSize;

  const int halfPopulationPerHeight =
      params.populationSize / (params.maxHeight - 1) / 2;
  std::vector<Node<T, RNG>> nodes;
  for (int i = 2; i <= params.maxHeight; ++i) {
    for (int j = 0; j < halfPopulationPerHeight; ++j) {
      nodes.emplace(grow(rng, i, params.functions, params.terminals));
      nodes.emplace(full(rng, i, params.functions, params.terminals));
    }
  }

  return nodes;
}

#endif // !COMPNAT_TP1_GENERATORS_HPP

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

namespace generators {

/// Returns a random primitive.
template <typename T, class RNG>
Primitive<T, RNG>
randomPrimitive(RNG &rng,
                const std::vector<PrimitiveFn<T, RNG>> &primitiveFns) {
  std::uniform_int_distribution<size_t> distr(0, primitiveFns.size() - 1);
  const auto primitiveFn = distr(rng);

  return primitiveFns[primitiveFn](rng);
}

/// Returns a random function or terminal.
template <typename T, class RNG>
Primitive<T, RNG>
randomPrimitive(RNG &rng, const std::vector<PrimitiveFn<T, RNG>> &functions,
                const std::vector<PrimitiveFn<T, RNG>> &terminals) {
  const auto numPrimitiveFns = functions.size() + terminals.size();
  std::uniform_int_distribution<size_t> distr(0, numPrimitiveFns - 1);
  const auto primitiveFn = distr(rng);

  if (primitiveFn < functions.size()) {
    return functions[primitiveFn](rng);
  } else {
    return terminals[primitiveFn - functions.size()](rng);
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
Node<T, RNG> grow(RNG &rng, size_t maxHeight,
                  const std::vector<PrimitiveFn<T, RNG>> &functions,
                  const std::vector<PrimitiveFn<T, RNG>> &terminals) {
  Node<T, RNG> root(randomPrimitive(rng, functions, terminals));

  std::stack<std::pair<Node<T, RNG> *, size_t>> nodes;
  nodes.emplace(&root, 1);
  while (!nodes.empty()) {
    auto[node, height] = nodes.top();
    nodes.pop();

    if (node->isTerminal()) {
      continue;
    } else if (height >= maxHeight - 1) {
      for (size_t i = 0; i < node->numChildren(); ++i) {
        node->setChild(i, randomPrimitive(rng, terminals));
      }
      continue;
    }

    for (size_t i = 0; i < node->numChildren(); ++i) {
      node->setChild(i, randomPrimitive(rng, functions, terminals));
      nodes.emplace(&node->mutableChild(i), height + 1);
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
Node<T, RNG> full(RNG &rng, size_t maxHeight,
                  const std::vector<PrimitiveFn<T, RNG>> &functions,
                  const std::vector<PrimitiveFn<T, RNG>> &terminals) {
  Node<T, RNG> root(randomPrimitive(rng, functions));

  std::stack<std::pair<Node<T, RNG> *, size_t>> nodes;
  nodes.emplace(&root, 1);
  while (!nodes.empty()) {
    auto[node, height] = nodes.top();
    nodes.pop();

    if (height >= maxHeight - 1) {
      for (size_t i = 0; i < node->numChildren(); ++i) {
        node->setChild(i, randomPrimitive(rng, terminals));
      }
      continue;
    }

    for (size_t i = 0; i < node->numChildren(); ++i) {
      node->setChild(i, randomPrimitive(rng, functions));
      nodes.emplace(&node->mutableChild(i), height + 1);
    }
  }

  return root;
}

/**
 * Generates trees using the ramped half and half method.
 * @param params Genetic Programming parameters. PopulationSize must be even and
 *   a multiple of (maxHeight - 1).
 * @param rng Random number generator.
 */
template <typename T, class RNG>
std::vector<Node<T, RNG>> rampedHalfAndHalf(RNG &rng,
                                            const Params<T, RNG> &params) {
  CHECK(params.populationSize % 2 == 0);
  CHECK(params.populationSize % (params.maxHeight - 1) == 0);

  const size_t halfPopulationPerHeight =
      params.populationSize / (params.maxHeight - 1) / 2;
  std::vector<Node<T, RNG>> nodes;
  for (size_t i = 2; i <= params.maxHeight; ++i) {
    for (size_t j = 0; j < halfPopulationPerHeight; ++j) {
      nodes.push_back(grow(rng, i, params.functions, params.terminals));
      nodes.push_back(full(rng, i, params.functions, params.terminals));
    }
  }

  return nodes;
}

} // namespace generators

#endif // !COMPNAT_TP1_GENERATORS_HPP

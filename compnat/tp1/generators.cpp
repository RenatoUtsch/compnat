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

#include "generators.hpp"

#include <random>
#include <stack>
#include <tuple>

namespace generators {

repr::Primitive
randomPrimitive(repr::RNG &rng,
                const std::vector<repr::PrimitiveFn> &primitiveFns) {
  std::uniform_int_distribution<size_t> distr(0, primitiveFns.size() - 1);
  const auto primitiveFn = distr(rng);

  return primitiveFns[primitiveFn](rng);
}

repr::Primitive
randomPrimitive(repr::RNG &rng, const std::vector<repr::PrimitiveFn> &functions,
                const std::vector<repr::PrimitiveFn> &terminals) {
  const auto numPrimitiveFns = functions.size() + terminals.size();
  std::uniform_int_distribution<size_t> distr(0, numPrimitiveFns - 1);
  const auto primitiveFn = distr(rng);

  if (primitiveFn < functions.size()) {
    return functions[primitiveFn](rng);
  } else {
    return terminals[primitiveFn - functions.size()](rng);
  }
}

repr::Node grow(repr::RNG &rng, size_t maxHeight,
                const std::vector<repr::PrimitiveFn> &functions,
                const std::vector<repr::PrimitiveFn> &terminals) {
  CHECK(maxHeight > 0);
  if (maxHeight == 1) {
    return randomPrimitive(rng, terminals);
  }
  repr::Node root(randomPrimitive(rng, functions, terminals));

  std::stack<std::tuple<repr::Node &, size_t>> nodes;
  nodes.emplace(root, 1);
  while (!nodes.empty()) {
    auto[node, height] = nodes.top();
    nodes.pop();

    if (node.isTerminal()) {
      continue;
    } else if (height >= maxHeight - 1) {
      for (size_t i = 0; i < node.numChildren(); ++i) {
        node.setChild(i, randomPrimitive(rng, terminals));
      }
      continue;
    }

    for (size_t i = 0; i < node.numChildren(); ++i) {
      node.setChild(i, randomPrimitive(rng, functions, terminals));
      nodes.emplace(node.mutableChild(i), height + 1);
    }
  }

  return root;
}

repr::Node full(repr::RNG &rng, size_t maxHeight,
                const std::vector<repr::PrimitiveFn> &functions,
                const std::vector<repr::PrimitiveFn> &terminals) {
  CHECK(maxHeight > 0);
  if (maxHeight == 1) {
    return randomPrimitive(rng, terminals);
  }
  repr::Node root(randomPrimitive(rng, functions));

  std::stack<std::tuple<repr::Node &, size_t>> nodes;
  nodes.emplace(root, 1);
  while (!nodes.empty()) {
    auto[node, height] = nodes.top();
    nodes.pop();

    if (height >= maxHeight - 1) {
      for (size_t i = 0; i < node.numChildren(); ++i) {
        node.setChild(i, randomPrimitive(rng, terminals));
      }
      continue;
    }

    for (size_t i = 0; i < node.numChildren(); ++i) {
      node.setChild(i, randomPrimitive(rng, functions));
      nodes.emplace(node.mutableChild(i), height + 1);
    }
  }

  return root;
}

std::vector<repr::Node> rampedHalfAndHalf(repr::RNG &rng,
                                          const repr::Params &params) {
  CHECK(params.populationSize % 2 == 0);
  CHECK(params.populationSize % (params.maxHeight - 1) == 0);

  const size_t halfPopulationPerHeight =
      params.populationSize / (params.maxHeight - 1) / 2;
  std::vector<repr::Node> nodes;
  for (size_t i = 2; i <= params.maxHeight; ++i) {
    for (size_t j = 0; j < halfPopulationPerHeight; ++j) {
      nodes.push_back(grow(rng, i, params.functions, params.terminals));
      nodes.push_back(full(rng, i, params.functions, params.terminals));
    }
  }

  return nodes;
}

} // namespace generators

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
#include "primitives.hpp"
#include "utils.hpp"

#include <random>

#include <gtest/gtest.h>

namespace {
using generators::randomPrimitive;
using generators::grow;
using generators::full;
using generators::rampedHalfAndHalf;
using utils::strCat;
using T = double;
using RNG = std::mt19937;

std::vector<PrimitiveFn<T, RNG>> getFunctions() {
  return {
      primitives::sumFn<T, RNG>,  primitives::subFn<T, RNG>,
      primitives::multFn<T, RNG>, primitives::divFn<T, RNG>,
      primitives::logFn<T, RNG>,
  };
}
std::vector<PrimitiveFn<T, RNG>> getTerminals() {
  return {
      primitives::makeVarTerm<T, RNG>("x0"),
      primitives::makeVarTerm<T, RNG>("x1"),
  };
}

void fillChildrenWithVars(RNG &rng, Node<T, RNG> &node) {
  for (size_t i = 0; i < node.numChildren(); ++i) {
    node.setChild(i, primitives::makeVarTerm<T, RNG>(strCat("x", i))(rng));
  }
}

TEST(RandomPrimitiveTest, SinglePrimitivesWorksCorrectly) {
  RNG rng(0);
  const auto &functions = getFunctions();

  auto node1 = Node<T, RNG>(randomPrimitive(rng, functions));
  fillChildrenWithVars(rng, node1);

  auto node2 = Node<T, RNG>(randomPrimitive(rng, functions));
  fillChildrenWithVars(rng, node2);

  ASSERT_NE(node1.str(), node2.str());
}

TEST(RandomPrimitiveTest, MultiplePrimitivesWorksCorrectly) {
  RNG rng(1); // Seed to generate the test we want.
  const auto &functions = getFunctions();
  const auto &terminals = getTerminals();

  auto node1 = Node<T, RNG>(randomPrimitive(rng, functions, terminals));
  ASSERT_TRUE(node1.isTerminal());

  auto node2 = Node<T, RNG>(randomPrimitive(rng, functions, terminals));
  ASSERT_FALSE(node2.isTerminal());
  node2.setChild(0, primitives::makeVarTerm<T, RNG>("x0")(rng));
  node2.setChild(1, primitives::makeVarTerm<T, RNG>("x1")(rng));

  ASSERT_NE(node1.str(), node2.str());
}

TEST(GrowTest, WorksCorrectly) {
  RNG rng(1); // Seed to generate the test we want.
  const size_t maxHeight = 7;
  const auto &functions = getFunctions();
  const auto &terminals = getTerminals();

  auto node1 = grow<T, RNG>(rng, maxHeight, functions, terminals);
  auto node2 = grow<T, RNG>(rng, maxHeight, functions, terminals);

  ASSERT_NE(node1.str(), node2.str());
}

TEST(FullTest, WorksCorrectly) {
  RNG rng(1); // Seed to generate the test we want.
  const size_t maxHeight = 7;
  const auto &functions = getFunctions();
  const auto &terminals = getTerminals();

  auto node1 = full<T, RNG>(rng, maxHeight, functions, terminals);
  auto node2 = full<T, RNG>(rng, maxHeight, functions, terminals);

  ASSERT_NE(node1.str(), node2.str());
}

TEST(RampedHalfAndHalfTest, WorksCorrectly) {
  const unsigned seed = 0;
  const size_t populationSize = 48;
  const size_t maxHeight = 7;

  RNG rng(seed);
  Params<T, RNG> params(seed, 100, populationSize, 7, maxHeight, 0.8, false,
                        getFunctions(), getTerminals());
  auto nodes = rampedHalfAndHalf<T, RNG>(params, rng);
  ASSERT_EQ(populationSize, nodes.size());
}

} // namespace

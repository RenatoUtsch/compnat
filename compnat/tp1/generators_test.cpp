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

#include <gtest/gtest.h>

#include "primitives.hpp"
#include "utils.hpp"

namespace {
using generators::full;
using generators::grow;
using generators::rampedHalfAndHalf;
using generators::randomPrimitive;
using utils::strCat;

std::vector<repr::PrimitiveFn> getFunctions() {
  return {
      primitives::sumFn, primitives::subFn, primitives::multFn,
      primitives::divFn, primitives::logFn,
  };
}
std::vector<repr::PrimitiveFn> getTerminals() {
  return {
      primitives::makeVarTerm(0),
      primitives::makeVarTerm(1),
  };
}

void fillChildrenWithVars(repr::RNG &rng, repr::Node &node) {
  for (size_t i = 0; i < node.numChildren(); ++i) {
    node.setChild(i, primitives::makeVarTerm(i)(rng));
  }
}

TEST(RandomPrimitiveTest, SinglePrimitivesWorksCorrectly) {
  repr::RNG rng;
  const auto &functions = getFunctions();

  auto node1 = repr::Node(randomPrimitive(rng, functions));
  fillChildrenWithVars(rng, node1);

  auto node2 = repr::Node(randomPrimitive(rng, functions));
  fillChildrenWithVars(rng, node2);

  EXPECT_NE(node1.str(), node2.str());
}

TEST(RandomPrimitiveTest, MultiplePrimitivesWorksCorrectly) {
  repr::RNG rng;
  const auto &functions = getFunctions();
  const auto &terminals = getTerminals();

  auto node1 = repr::Node(randomPrimitive(rng, functions, terminals));
  for (size_t i = 0; i < node1.numChildren(); ++i) {
    node1.setChild(i, randomPrimitive(rng, terminals));
  }

  if (node1.numChildren()) {
    EXPECT_FALSE(node1.isTerminal());
  } else {
    EXPECT_TRUE(node1.isTerminal());
  }

  auto node2 = repr::Node(randomPrimitive(rng, functions, terminals));
  for (size_t i = 0; i < node2.numChildren(); ++i) {
    node2.setChild(i, randomPrimitive(rng, terminals));
  }

  if (node2.numChildren()) {
    EXPECT_FALSE(node2.isTerminal());
  } else {
    EXPECT_TRUE(node2.isTerminal());
  }

  EXPECT_NE(node1.str(), node2.str());
}

TEST(GrowTest, WorksCorrectly) {
  repr::RNG rng;
  const size_t maxHeight = 7;
  const auto &functions = getFunctions();
  const auto &terminals = getTerminals();

  auto node1 = grow(rng, maxHeight, functions, terminals);
  auto node2 = grow(rng, maxHeight, functions, terminals);

  EXPECT_NE(node1.str(), node2.str());
}

TEST(FullTest, WorksCorrectly) {
  repr::RNG rng;
  const size_t maxHeight = 7;
  const auto &functions = getFunctions();
  const auto &terminals = getTerminals();

  auto node1 = full(rng, maxHeight, functions, terminals);
  auto node2 = full(rng, maxHeight, functions, terminals);

  EXPECT_NE(node1.str(), node2.str());
}

TEST(RampedHalfAndHalfTest, WorksCorrectly) {
  const size_t populationSize = 48;
  const size_t maxHeight = 7;

  repr::RNG rng;
  repr::Params params(0, 100, populationSize, 7, maxHeight, 0.8, false,
                      getFunctions(), getTerminals());
  auto nodes = rampedHalfAndHalf(rng, params);
  EXPECT_EQ(populationSize, nodes.size());
}

} // namespace

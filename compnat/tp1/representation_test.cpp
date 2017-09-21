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

#include "representation.hpp"

#include <random>

#include <gtest/gtest.h>

#include "primitives.hpp"

namespace {
using T = double;
using RNG = std::mt19937;

TEST(ParamsTest, WorksCorrectly) {
  Params<T, RNG> params(42, 50, 8, 6, 5, 0.5, false,
                        {primitives::sumFn<T, RNG>},
                        {primitives::makeVarTerm<T, RNG>("x0"),
                         primitives::makeVarTerm<T, RNG>("x1")});
  EXPECT_EQ((unsigned)42, params.seed);
  EXPECT_EQ((size_t)50, params.numGenerations);
  EXPECT_EQ((size_t)8, params.populationSize);
  EXPECT_EQ((size_t)6, params.tournamentSize);
  EXPECT_EQ((size_t)5, params.maxHeight);
  EXPECT_FLOAT_EQ(0.5, params.crossoverProb);
  EXPECT_EQ(false, params.elitism);
  EXPECT_EQ((size_t)1, params.functions.size());
  EXPECT_EQ((size_t)2, params.terminals.size());
}

TEST(ParamsTest, UpdatesPopulationSizeCorrectly) {
  Params<T, RNG> params1(0, 0, 0, 0, 5, 0, false, {}, {});
  EXPECT_EQ((size_t)4, params1.populationSize);

  Params<T, RNG> params2(0, 0, 15, 0, 8, 0, false, {}, {});
  EXPECT_EQ((size_t)28, params2.populationSize);
}

TEST(NodeTest, AcceptsValidPrimitiveAndGivesCorrectResults) {
  RNG rng(0);
  Node<T, RNG> node(primitives::sumFn<T, RNG>(rng));
  EXPECT_EQ((size_t)2, node.numChildren());

  node.setChild(0, primitives::makeVarTerm<T, RNG>("x0")(rng));
  node.setChild(1, primitives::constTerm<T, RNG>(rng));
  EXPECT_EQ("(x0 + 0.185689)", node.str());
  EXPECT_FLOAT_EQ(42.185689, node.eval({{"x0", 42}}));
  EXPECT_FALSE(node.isTerminal());
  EXPECT_TRUE(node.child(0).isTerminal());
  EXPECT_TRUE(node.child(1).isTerminal());
}

} // namespace

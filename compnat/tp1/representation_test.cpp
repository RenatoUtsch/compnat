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
using repr::Node;
using repr::Params;
using repr::Primitive;
using repr::RNG;

TEST(ParamsTest, WorksCorrectly) {
  Params params("he", 42, 3, 50, 8, 6, 5, 0.5, false, true, {primitives::sumFn},
                {primitives::makeVarTerm(0), primitives::makeVarTerm(1)});
  EXPECT_EQ("he", params.outputFile);
  EXPECT_EQ((unsigned)42, params.seed);
  EXPECT_EQ((unsigned)3, params.numInstances);
  EXPECT_EQ((size_t)50, params.numGenerations);
  EXPECT_EQ((size_t)8, params.populationSize);
  EXPECT_EQ((size_t)6, params.tournamentSize);
  EXPECT_EQ((size_t)5, params.maxHeight);
  EXPECT_FLOAT_EQ(0.5, params.crossoverProb);
  EXPECT_FALSE(params.elitism);
  EXPECT_TRUE(params.alwaysTest);
  EXPECT_EQ((size_t)1, params.functions.size());
  EXPECT_EQ((size_t)2, params.terminals.size());
}

TEST(ParamsTest, UpdatesPopulationSizeCorrectly) {
  Params params1("", 0, 0, 0, 0, 0, 5, 0.8, false, false, {}, {});
  EXPECT_EQ((size_t)4, params1.populationSize);

  Params params2("", 0, 0, 0, 15, 0, 8, 0.8, false, false, {}, {});
  EXPECT_EQ((size_t)28, params2.populationSize);
}

TEST(NodeTest, AcceptsValidPrimitiveAndGivesCorrectResults) {
  RNG rng(0);
  Node node(primitives::sumFn(rng));
  EXPECT_EQ((size_t)2, node.numChildren());

  node.setChild(0, primitives::makeVarTerm(0)(rng));
  node.setChild(1, primitives::constTerm(rng));
  EXPECT_EQ("(x0 + 0.185689)", node.str());
  EXPECT_FLOAT_EQ(42.185689, node.eval({{42, 0}}));
  EXPECT_FALSE(node.isTerminal());
  EXPECT_TRUE(node.child(0).isTerminal());
  EXPECT_TRUE(node.child(1).isTerminal());
}

} // namespace

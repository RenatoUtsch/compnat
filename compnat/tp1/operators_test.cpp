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
#include "operators.hpp"
#include "primitives.hpp"
#include "statistics.hpp"

#include <random>

#include <gtest/gtest.h>

namespace {
using T = double;
using RNG = std::mt19937;
using operators::tournamentSelection;
using operators::crossover;
using operators::mutation;
using operators::generateNewPopulation;

TEST(TournamentSelectionTest, WorksCorrectly) {
  RNG rng(0);
  const size_t selected = tournamentSelection<T>(rng, 9, {1, 5, 4});
  EXPECT_EQ((size_t)1, selected);
}

TEST(CrossoverTest, WorksCorrectly) {
  RNG rng(3); // Seed for test we want

  Node<T, RNG> node0(primitives::sumFn<T>(rng));
  node0.setChild(0, primitives::makeVarTerm<T, RNG>("x0")(rng));
  node0.setChild(1, primitives::makeVarTerm<T, RNG>("x1")(rng));

  Node<T, RNG> node1(primitives::logFn<T>(rng));
  node1.setChild(0, primitives::logFn<T, RNG>(rng));
  node1.mutableChild(0).setChild(0, primitives::makeVarTerm<T, RNG>("x0")(rng));

  const auto &nodeSizes = stats::sizes<T, RNG>({node0, node1});
  EXPECT_EQ((size_t)3, nodeSizes[0]);
  EXPECT_EQ("(x0 + x1)", node0.str());
  EXPECT_EQ((size_t)3, nodeSizes[1]);
  EXPECT_EQ("log2(log2(x0))", node1.str());

  auto[child0, child1] =
      crossover<T, RNG>(rng, node0, nodeSizes[0], node1, nodeSizes[1]);

  const auto &childSizes = stats::sizes<T, RNG>({child0, child1});
  EXPECT_EQ((size_t)4, childSizes[0]);
  EXPECT_EQ("(x0 + log2(x0))", child0.str());
  EXPECT_EQ((size_t)2, childSizes[1]);
  EXPECT_EQ("log2(x1)", child1.str());
}

TEST(MutationTest, WorksCorrectly) {
  RNG rng(3);

  // For params.maxHeight, functions and terminals.
  Params<T, RNG> params(0, 0, 4, 0, 3, 0, false,
                        {primitives::sumFn<T, RNG>, primitives::subFn<T, RNG>,
                         primitives::multFn<T, RNG>, primitives::divFn<T, RNG>,
                         primitives::logFn<T, RNG>},
                        {primitives::makeVarTerm<T, RNG>("x0"),
                         primitives::makeVarTerm<T, RNG>("x1")});

  Node<T, RNG> node(primitives::sumFn<T>(rng));
  node.setChild(0, primitives::makeVarTerm<T, RNG>("x0")(rng));
  node.setChild(1, primitives::makeVarTerm<T, RNG>("x1")(rng));

  const auto &nodeSizes = stats::sizes<T, RNG>({node});
  EXPECT_EQ((size_t)3, nodeSizes[0]);
  EXPECT_EQ("(x0 + x1)", node.str());

  auto child = mutation(rng, params, node, nodeSizes[0]);
  const auto &childSizes = stats::sizes<T, RNG>({child});
  EXPECT_EQ((size_t)9, childSizes[0]);
  EXPECT_EQ("(x0 + ((x0 - x1) + (x0 / x0)))", child.str());
}

TEST(GenerateNewPopulationTest, WorksCorrectly) { EXPECT_EQ(false, true); }

} // namespace

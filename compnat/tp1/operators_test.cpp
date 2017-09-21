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

#include "operators.hpp"

#include <random>

#include <gtest/gtest.h>

#include "generators.hpp"
#include "parser.hpp"
#include "primitives.hpp"
#include "statistics.hpp"

namespace {
using T = double;
using RNG = std::mt19937;
using operators::crossover;
using operators::mutation;
using operators::newGeneration;
using operators::tournamentSelection;

TEST(TournamentSelectionTest, WorksCorrectly) {
  RNG rng;
  const size_t selected = tournamentSelection<T>(rng, 9, {1, 5, 4});
  EXPECT_EQ((size_t)0, selected);
}

TEST(CrossoverTest, WorksCorrectly) {
  RNG rng;

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
  EXPECT_NE(nodeSizes[0], childSizes[0]);
  EXPECT_NE(node0.str(), child0.str());
  EXPECT_NE(nodeSizes[1], childSizes[1]);
  EXPECT_NE(node1.str(), child1.str());
}

TEST(MutationTest, WorksCorrectly) {
  RNG rng;

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
  EXPECT_NE(nodeSizes[0], childSizes[0]);
  EXPECT_NE(node.str(), child.str());
}

TEST(MutationTest, OneElementTree) {
  RNG rng;

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
  EXPECT_NE(nodeSizes[0], childSizes[0]);
  EXPECT_NE(node.str(), child.str());
}

TEST(NewGenerationTest, WorksCorrectly) {
  RNG rng;

  // For params.maxHeight, functions and terminals.
  Params<T, RNG> params(
      0, 10, 60, 5, 7, 0.9, true,
      {
          primitives::sumFn<T, RNG>, primitives::subFn<T, RNG>,
          primitives::multFn<T, RNG>, primitives::divFn<T, RNG>,
      },
      {
          primitives::constTerm<T, RNG>, primitives::makeVarTerm<T, RNG>("x0"),
      });

  const auto &dataset =
      parser::loadDataset<T>("compnat/tp1/datasets/keijzer-7-train.csv");

  const auto population = generators::rampedHalfAndHalf(rng, params);
  const auto stats = stats::Statistics<T, RNG>(population, dataset);
  const auto[newPopulation, crossoverIndices] =
      newGeneration(rng, params, population, stats);
  const auto newStats = stats::Statistics<T, RNG>(
      newPopulation, dataset, stats.averageFitness, crossoverIndices);

  EXPECT_EQ(population.size(), newPopulation.size());

  // Test elitism.
  EXPECT_EQ(population[stats.best].str(), newPopulation[0].str());
}

} // namespace

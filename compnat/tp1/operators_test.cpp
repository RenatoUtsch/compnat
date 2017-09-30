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
#include "threading.hpp"

namespace {
using operators::crossover;
using operators::mutation;
using operators::newGeneration;
using operators::tournamentSelection;

TEST(TournamentSelectionTest, WorksCorrectly) {
  repr::RNG rng;
  const size_t selected = tournamentSelection(rng, 9, {1, 5, 4});
  EXPECT_EQ((size_t)0, selected);
}

TEST(CrossoverTest, WorksCorrectly) {
  repr::RNG rng;

  // For params.maxHeight.
  repr::Params params(0, 0, 5, 0, 3, 0.8, false, {}, {});

  repr::Node node0(primitives::sumFn(rng));
  node0.setChild(0, primitives::makeVarTerm(0)(rng));
  node0.setChild(1, primitives::makeVarTerm(1)(rng));

  repr::Node node1(primitives::logFn(rng));
  node1.setChild(0, primitives::logFn(rng));
  node1.mutableChild(0).setChild(0, primitives::makeVarTerm(0)(rng));

  const auto &nodeSizes = stats::sizes({node0, node1});
  EXPECT_EQ((size_t)3, nodeSizes[0]);
  EXPECT_EQ("(x0 + x1)", node0.str());
  EXPECT_EQ((size_t)3, nodeSizes[1]);
  EXPECT_EQ("log2(log2(x0))", node1.str());

  auto[child0, child1] =
      crossover(rng, params, node0, nodeSizes[0], node1, nodeSizes[1]);

  [[maybe_unused]] const auto &childSizes = stats::sizes({child0, child1});
}

TEST(MutationTest, WorksCorrectly) {
  repr::RNG rng;

  // For params.maxHeight, functions and terminals.
  repr::Params params( // Keep formatting
      0, 0, 4, 0, 3, 0.8, false,
      {primitives::sumFn, primitives::subFn, primitives::multFn,
       primitives::divFn, primitives::logFn},
      {primitives::makeVarTerm(0), primitives::makeVarTerm(1)});

  repr::Node node(primitives::sumFn(rng));
  node.setChild(0, primitives::makeVarTerm(0)(rng));
  node.setChild(1, primitives::makeVarTerm(1)(rng));

  const auto &nodeSizes = stats::sizes({node});
  EXPECT_EQ((size_t)3, nodeSizes[0]);
  EXPECT_EQ("(x0 + x1)", node.str());

  auto child = mutation(rng, params, node, nodeSizes[0]);
  const auto &childSizes = stats::sizes({child});
  EXPECT_NE(nodeSizes[0], childSizes[0]);
  EXPECT_NE(node.str(), child.str());
}

TEST(MutationTest, OneElementTree) {
  repr::RNG rng;

  // For params.maxHeight, functions and terminals.
  repr::Params params( // Keep formatting
      0, 0, 4, 0, 3, 0.8, false,
      {primitives::sumFn, primitives::subFn, primitives::multFn,
       primitives::divFn, primitives::logFn},
      {primitives::makeVarTerm(0), primitives::makeVarTerm(1)});

  repr::Node node(primitives::sumFn(rng));
  node.setChild(0, primitives::makeVarTerm(0)(rng));
  node.setChild(1, primitives::makeVarTerm(1)(rng));

  const auto &nodeSizes = stats::sizes({node});
  EXPECT_EQ((size_t)3, nodeSizes[0]);
  EXPECT_EQ("(x0 + x1)", node.str());

  auto child = mutation(rng, params, node, nodeSizes[0]);
  const auto &childSizes = stats::sizes({child});
  EXPECT_NE(nodeSizes[0], childSizes[0]);
  EXPECT_NE(node.str(), child.str());
}

TEST(NewGenerationTest, WorksCorrectly) {
  repr::RNG rng;
  threading::ThreadPool pool;

  // For params.maxHeight, functions and terminals.
  repr::Params params( // Keep formatting
      0, 10, 60, 5, 7, 0.9, true,
      {
          primitives::sumFn,
          primitives::subFn,
          primitives::multFn,
          primitives::divFn,
      },
      {
          primitives::constTerm,
          primitives::makeVarTerm(0),
      });

  const auto &dataset =
      parser::loadDataset("compnat/tp1/datasets/keijzer-7-train.csv");

  const auto population = generators::rampedHalfAndHalf(rng, params);
  const auto stats = stats::Statistics(pool, population, dataset);
  const auto[newPopulation, improvementMetadata] =
      newGeneration(rng, params, population, stats);
  const auto newStats =
      stats::Statistics(pool, newPopulation, dataset, improvementMetadata);

  EXPECT_EQ(population.size(), newPopulation.size());

  // Test elitism.
  EXPECT_EQ(population[stats.best].str(), newPopulation[0].str());
}

} // namespace

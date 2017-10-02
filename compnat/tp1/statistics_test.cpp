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

#include "statistics.hpp"

#include <random>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "generators.hpp"
#include "parser.hpp"
#include "primitives.hpp"
#include "representation.hpp"

namespace {
using stats::Statistics;
using testing::ElementsAre;

std::vector<repr::Node> generatePopulation() {
  repr::RNG rng(0);
  repr::Node node1(primitives::sumFn(rng));
  node1.setChild(0, repr::Node(primitives::makeVarTerm(0)(rng)));
  node1.setChild(1, repr::Node(primitives::makeVarTerm(1)(rng)));

  repr::Node node2(primitives::logFn(rng));
  node2.setChild(0, repr::Node(primitives::makeVarTerm(0)(rng)));

  repr::Node node3(repr::Node(primitives::makeVarTerm(0)(rng)));

  return {std::move(node1), std::move(node2), std::move(node3)};
}

TEST(FitnessTest, SingleWorksCorrectly) {
  const auto &population = generatePopulation();
  const repr::Dataset dataset = {
      {{12, 2}, 15},
      {{15, 4}, 21},
  };

  const auto fitness = stats::fitness(population[0], dataset);
  ASSERT_FLOAT_EQ(1.5811388, fitness);
}

TEST(FitnessTest, WorksCorrectly) {
  const auto &population = generatePopulation();
  const repr::Dataset dataset = {
      {{12, 2}, 15},
      {{15, 4}, 21},
  };

  const auto fitness = stats::fitness(population, dataset);
  ASSERT_EQ((size_t)3, fitness.size());
  EXPECT_FLOAT_EQ(1.5811388, fitness[0]);
  EXPECT_FLOAT_EQ(14.534055, fitness[1]);
  EXPECT_FLOAT_EQ(4.7434163, fitness[2]);
}

TEST(FitnessTest, GeneratesExpectedValue) {
  repr::RNG rng;

  const auto &dataset =
      parser::loadDataset("compnat/tp1/datasets/keijzer-10-test.csv");

  const auto individual = repr::Node(primitives::literalTerm(0.791453)(rng));
  const auto fitness = stats::fitness(individual, dataset);
  EXPECT_FLOAT_EQ(0.089933448, fitness);
}

TEST(SizesTest, WorksCorrectly) {
  const auto &population = generatePopulation();

  const auto &sizes = stats::sizes(population);
  ASSERT_THAT(sizes, ElementsAre(3, 2, 1));
}

TEST(StatisticsTest, SingleGenerationPerformanceBenchmark) {
  const auto &dataset =
      parser::loadDataset("compnat/tp1/datasets/unit_test.csv");

  // Params for a big test.
  repr::Params params( // Improve formatting
      "", 1, 1, 1, 600, 7, 7, 0.9, false, false,
      {
          primitives::sumFn,
          primitives::subFn,
          primitives::multFn,
          primitives::divFn,
      },
      {
          primitives::constTerm,
          primitives::makeVarTerm(0),
          primitives::makeVarTerm(1),
          primitives::makeVarTerm(2),
          primitives::makeVarTerm(3),
      });

  repr::RNG rng;
  const auto &population = generators::rampedHalfAndHalf(rng, params);
  EXPECT_EQ((size_t)600, population.size());

  const auto fitnesses = stats::fitness(population, dataset);
  const auto sizes = stats::sizes(population);
  [[maybe_unused]] const auto stats =
      Statistics("train", population, fitnesses, sizes);
  EXPECT_EQ((size_t)600, fitnesses.size());
  EXPECT_EQ((size_t)600, sizes.size());
}

} // namespace

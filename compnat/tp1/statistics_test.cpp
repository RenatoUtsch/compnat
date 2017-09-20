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

#include "primitives.hpp"
#include "statistics.hpp"

#include <random>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {
using testing::ElementsAre;
using T = double;
using RNG = std::mt19937;

std::vector<Node<T, RNG>> generatePopulation() {
  RNG rng(0);
  Node<T, RNG> node1(primitives::sumFn<T>(rng));
  node1.setChild(0, Node<T, RNG>(primitives::makeVarTerm<T, RNG>("x0")(rng)));
  node1.setChild(1, Node<T, RNG>(primitives::makeVarTerm<T, RNG>("x1")(rng)));

  Node<T, RNG> node2(primitives::logFn<T>(rng));
  node2.setChild(0, Node<T, RNG>(primitives::makeVarTerm<T, RNG>("x0")(rng)));

  Node<T, RNG> node3(Node<T, RNG>(primitives::makeVarTerm<T, RNG>("x0")(rng)));

  return {std::move(node1), std::move(node2), std::move(node3)};
}

TEST(FitnessTest, SingleWorksCorrectly) {
  const auto &population = generatePopulation();
  const Dataset<T> dataset = {
      {{{"x0", 12}, {"x1", 2}}, 15}, {{{"x0", 15}, {"x1", 4}}, 21},
  };

  const T fitness = stats::fitness(population[0], dataset);
  ASSERT_FLOAT_EQ(1.5811388, fitness);
}

TEST(FitnessTest, WorksCorrectly) {
  const auto &population = generatePopulation();
  const Dataset<T> dataset = {
      {{{"x0", 12}, {"x1", 2}}, 15}, {{{"x0", 15}, {"x1", 4}}, 21},
  };

  const auto &fitness = stats::fitness(population, dataset);
  ASSERT_EQ((size_t)3, fitness.size());
  EXPECT_FLOAT_EQ(1.5811388, fitness[0]);
  EXPECT_FLOAT_EQ(14.534055, fitness[1]);
  EXPECT_FLOAT_EQ(4.7434163, fitness[2]);
}

TEST(SizesTest, WorksCorrectly) {
  const auto &population = generatePopulation();

  const auto &sizes = stats::sizes(population);
  ASSERT_THAT(sizes, ElementsAre(3, 2, 1));
}

TEST(StrsTest, WorksCorrectly) {
  const auto &population = generatePopulation();

  const auto &strs = stats::strs(population);
  ASSERT_THAT(strs, ElementsAre("(x0 + x1)", "log2(x0)", "x0"));
}

} // namespace

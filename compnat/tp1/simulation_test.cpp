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

#include "simulation.hpp"

#include <random>

#include <gtest/gtest.h>

#include "parser.hpp"
#include "primitives.hpp"
#include "representation.hpp"

namespace {
using T = double;
using RNG = std::mt19937;
using simulation::simulate;

TEST(NewGenerationTest, WorksCorrectly) {
  RNG rng;

  // For params.maxHeight, functions and terminals.
  Params<T, RNG> params( // Keep formatting
      0, 10, 60, 5, 7, 0.1, 0.9, false,
      {
          primitives::sumFn<T, RNG>,
          primitives::subFn<T, RNG>,
          primitives::multFn<T, RNG>,
          primitives::divFn<T, RNG>,
      },
      {
          primitives::constTerm<T, RNG>,
          primitives::makeVarTerm<T, RNG>(0),
      });

  const auto &trainDataset =
      parser::loadDataset<T>("compnat/tp1/datasets/keijzer-7-train.csv");
  const auto &testDataset =
      parser::loadDataset<T>("compnat/tp1/datasets/keijzer-7-test.csv");

  simulate(params, trainDataset, testDataset);
}

} // namespace

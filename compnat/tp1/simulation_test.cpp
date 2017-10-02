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
using simulation::simulate;

TEST(NewGenerationTest, WorksCorrectly) {
  repr::RNG rng;

  // For params.maxHeight, functions and terminals.
  repr::Params params( // Keep formatting
      "", 1, 1, 10, 60, 5, 7, 0.9, false, false,
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

  const auto &trainDataset =
      parser::loadDataset("compnat/tp1/datasets/keijzer-7-train.csv");
  const auto &testDataset =
      parser::loadDataset("compnat/tp1/datasets/keijzer-7-test.csv");

  simulate(params, trainDataset, testDataset);
}

} // namespace

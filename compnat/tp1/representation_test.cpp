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
#include "representation.hpp"

#include <random>

#include <gtest/gtest.h>

namespace {

TEST(NodeTest, AcceptsValidPrimitiveAndGivesCorrectResults) {
  std::mt19937 rng(0);
  Node<double, std::mt19937> node(primitives::sumFn<double>(rng));
  EXPECT_EQ((size_t)2, node.numChildren());

  node.setChild(0, primitives::makeVarTerm<double, decltype(rng)>("x0")(rng));
  node.setChild(1, primitives::constTerm<double>(rng));
  EXPECT_EQ("(x0 + 0.185689)", node.str());
  EXPECT_FLOAT_EQ(42.185689, node.eval({{"x0", 42}}));
  EXPECT_FALSE(node.isTerminal());
  EXPECT_TRUE(node.child(0).isTerminal());
  EXPECT_TRUE(node.child(1).isTerminal());
}

} // namespace

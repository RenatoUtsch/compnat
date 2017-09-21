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

#include <random>

#include <gtest/gtest.h>

#include "representation.hpp"

namespace {
using T = double;
using RNG = std::mt19937;

Node<T, RNG> generateNode2(const PrimitiveFn<T, RNG> &primitiveFn) {
  RNG rng(0);
  Node<T, RNG> node(primitiveFn(rng));
  node.setChild(0, Node<T, RNG>(primitives::makeVarTerm<T, RNG>("x0")(rng)));
  node.setChild(1, Node<T, RNG>(primitives::makeVarTerm<T, RNG>("x1")(rng)));

  return node;
}

Node<T, RNG> generateNode1(const PrimitiveFn<T, RNG> &primitiveFn) {
  RNG rng(0);
  Node<T, RNG> node(primitiveFn(rng));
  node.setChild(0, Node<T, RNG>(primitives::makeVarTerm<T, RNG>("x0")(rng)));

  return node;
}

Node<T, RNG> generateNode0(const PrimitiveFn<T, RNG> &primitiveFn) {
  RNG rng(0);
  Node<T, RNG> node(primitiveFn(rng));

  return node;
}

TEST(SumFnTest, WorksCorrectly) {
  const auto &node = generateNode2(primitives::sumFn<T, RNG>);
  EXPECT_FALSE(node.isTerminal());
  EXPECT_EQ("(x0 + x1)", node.str());
  EXPECT_EQ(5, node.eval({{"x0", 3}, {"x1", 2}}));
}

TEST(SubFnTest, WorksCorrectly) {
  const auto &node = generateNode2(primitives::subFn<T, RNG>);
  EXPECT_FALSE(node.isTerminal());
  EXPECT_EQ("(x0 - x1)", node.str());
  EXPECT_EQ(1, node.eval({{"x0", 3}, {"x1", 2}}));
}

TEST(MultFnTest, WorksCorrectly) {
  const auto &node = generateNode2(primitives::multFn<T, RNG>);
  EXPECT_FALSE(node.isTerminal());
  EXPECT_EQ("(x0 * x1)", node.str());
  EXPECT_EQ(6, node.eval({{"x0", 3}, {"x1", 2}}));
}

TEST(DivFnTest, WorksCorrectly) {
  const auto &node = generateNode2(primitives::divFn<T, RNG>);
  EXPECT_FALSE(node.isTerminal());
  EXPECT_EQ("(x0 / x1)", node.str());
  EXPECT_EQ(1.5, node.eval({{"x0", 3}, {"x1", 2}}));
  EXPECT_EQ(0, node.eval({{"x0", 3}, {"x1", 0}}));
}

TEST(LogFnTest, WorksCorrectly) {
  const auto &node = generateNode1(primitives::logFn<T, RNG>);
  EXPECT_FALSE(node.isTerminal());
  EXPECT_EQ("log2(x0)", node.str());
  EXPECT_FLOAT_EQ(1.5849625, node.eval({{"x0", 3}}));
}

TEST(ConstTermTest, WorksCorrectly) {
  const auto &node = generateNode0(primitives::constTerm<T, RNG>);
  EXPECT_TRUE(node.isTerminal());
  EXPECT_EQ("0.185689", node.str());
  EXPECT_FLOAT_EQ(0.18568923, node.eval({}));
}

TEST(VarTermTest, WorksCorrectly) {
  const auto &node = generateNode0(primitives::makeVarTerm<T, RNG>("x2"));
  EXPECT_TRUE(node.isTerminal());
  EXPECT_EQ("x2", node.str());
  EXPECT_FLOAT_EQ(3, node.eval({{"x2", 3}}));
}

} // namespace

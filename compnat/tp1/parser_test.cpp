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

#include "parser.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {
using parser::loadDataset;
using parser::splitLine;
using testing::ElementsAreArray;
using testing::Pair;
using testing::UnorderedElementsAreArray;

TEST(SplitLineTest, WorksCorrectly) {
  const auto &result = splitLine("0,12,3.44,aaa,bbb,-44", ',');

  EXPECT_THAT(result,
              ElementsAreArray({"0", "12", "3.44", "aaa", "bbb", "-44"}));
}

TEST(LoadDatasetTest, WorksCorrectly) {
  const auto &dataset =
      loadDataset<double>("compnat/tp1/datasets/unit_test.csv");
  ASSERT_EQ((size_t)2, dataset.size());

  auto & [ input0, expected0 ] = dataset[0];
  ASSERT_EQ((size_t)4, input0.size());
  ASSERT_FLOAT_EQ(4, input0[0]);
  ASSERT_FLOAT_EQ(5, input0[1]);
  ASSERT_FLOAT_EQ(3.6, input0[2]);
  ASSERT_FLOAT_EQ(7.8, input0[3]);
  ASSERT_FLOAT_EQ(900, expected0);

  auto & [ input1, expected1 ] = dataset[1];
  ASSERT_EQ((size_t)4, input1.size());
  ASSERT_FLOAT_EQ(6, input1[0]);
  ASSERT_FLOAT_EQ(3.3, input1[1]);
  ASSERT_FLOAT_EQ(4, input1[2]);
  ASSERT_FLOAT_EQ(5, input1[3]);
  ASSERT_FLOAT_EQ(-800.15, expected1);
}

} // namespace

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

#include "utils.hpp"

#include <gtest/gtest.h>

namespace {
using utils::paddedStrCat;
using utils::safeDiv;
using utils::strCat;
using utils::strSplit;

TEST(SafeDivTest, NormalDivision) {
  EXPECT_EQ(2, safeDiv(5, 2));
  EXPECT_FLOAT_EQ(2.5, safeDiv(5.0, 2.0));
}

TEST(SafeDivTest, ByZeroDivision) {
  EXPECT_EQ(0, safeDiv(-2, 0));
  EXPECT_FLOAT_EQ(0.0, safeDiv(7.0, 0.0));
}

TEST(StrCatTest, WorksCorrectly) {
  EXPECT_EQ("abc123D3.14", strCat("abc", 123, 'D', 3.14));
}

TEST(PaddedStrCatTest, WorksCorrectly) {
  EXPECT_EQ("abc123D3.14         ", paddedStrCat(20, "abc", 123, 'D', 3.14));
}

TEST(StrSplitTest, WorksCorrectly) {
  std::string s;
  double d = 0;
  char c = 0;
  int i = 0;
  strSplit("food 3.14 a 42", s, d, c, i);
  EXPECT_EQ("food", s);
  EXPECT_DOUBLE_EQ(3.14, d);
  EXPECT_EQ('a', c);
  EXPECT_EQ(42, i);
}

} // namespace

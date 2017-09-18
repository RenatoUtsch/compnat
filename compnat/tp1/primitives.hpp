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

#ifndef COMPNAT_TP1_OPERATORS_HPP
#define COMPNAT_TP1_OPERATORS_HPP

#include <cmath>
#include <limits>
#include <string>

#include "representation.hpp"

/// Safe division, returns default if b is 0.
template <typename T> T safeDiv(const T &a, const T &b, const T &def) {
  // Simplification to also support integers. Considers the epsilon as 0.
  if (std::abs(b) <= std::numeric_limits<T>::epsilon()) {
    return def;
  }
  return a / b;
}

/// Sum function.
template <typename T> inline Primitive<T> sumFn() {
  return Primitive<T>(2, [](const auto &input, const auto &children) {
    return children[0].eval(input) + children[1].eval(input);
  });
}

/// Subtraction function.
template <typename T> inline Primitive<T> subFn() {
  return Primitive<T>(2, [](const auto &input, const auto &children) {
    return children[0].eval(input) - children[1].eval(input);
  });
}

/// Multiplication function.
template <typename T> inline Primitive<T> multFn() {
  return Primitive<T>(2, [](const auto &input, const auto &children) {
    return children[0].eval(input) * children[1].eval(input);
  });
}

/// Division function. Returns 0 if division is by 0.
template <typename T> inline Primitive<T> divFn() {
  return Primitive<T>(2, [](const auto &input, const auto &children) {
    const T &secondResult = children[1].eval(input);
    return safeDiv(children[0].eval(input), children[1].eval(input), 0);
  });
}

/// Logarithm function.
template <typename T> inline Primitive<T> logFn() {
  return Primitive<T>(1, [](const auto &input, const auto &children) {
    return std::log(children[0].eval(input));
  });
}

/// Variable terminal. Returns the value of the given variable.
template <typename T> inline Primitive<T> varTerm(const std::string &var) {
  return Primitive<T>(
      0, [&var](const auto &input, [[maybe_unused]] const auto &children) {
        return input[var];
      });
}

/// Constant terminal. Returns value.
template <typename T> inline Primitive<T> constTerm(const T &value) {
  return Primitive<T>(
      0, [&value]([[maybe_unused]] const auto &input,
                  [[maybe_unused]] const auto &children) { return value; });
}

#endif // !COMPNAT_TP1_OPERATORS_HPP

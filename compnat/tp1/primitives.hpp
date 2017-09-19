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
#include <random>
#include <string>

#include "representation.hpp"

/// Safe division, returns default if b is 0.
template <typename T> T safeDiv(const T &a, const T &b, const T &def = 0) {
  // Simplification to also support integers. Considers the epsilon as 0.
  if (std::abs(b) <= std::numeric_limits<T>::epsilon()) {
    return def;
  }
  return a / b;
}

template <typename T, typename... Args>
void strCatJoiner_(std::stringstream &ss, const T &t, Args... args) {
  ss << t;
  strCatJoiner_(ss, args...);
}

/// Concatenates the given values into a single string.
template <typename... Args> std::string strCat(Args... args) {
  std::stringstream ss;
  strCatJoiner_(ss, args...);
  return ss.str();
}

/// Sum function.
template <typename T, class RNG>
inline Primitive<T> sumFn([[maybe_unused]] RNG &rng) {
  return Primitive<T>(
      2,
      [](const auto &input, const auto &children) {
        return children[0].eval(input) + children[1].eval(input);
      },
      [](const auto &children) {
        return strCat('(', children[0].str(), " + ", children[1].str(), ')');
      });
}

/// Subtraction function.
template <typename T, class RNG>
inline Primitive<T> subFn([[maybe_unused]] RNG &rng) {
  return Primitive<T>(
      2,
      [](const auto &input, const auto &children) {
        return children[0].eval(input) - children[1].eval(input);
      },
      [](const auto &children) {
        return strCat('(', children[0].str(), " - ", children[1].str(), ')');
      });
}

/// Multiplication function.
template <typename T, class RNG>
inline Primitive<T> multFn([[maybe_unused]] RNG &rng) {
  return Primitive<T>(
      2,
      [](const auto &input, const auto &children) {
        return children[0].eval(input) * children[1].eval(input);
      },
      [](const auto &children) {
        return strCat('(', children[0].str(), " * ", children[1].str(), ')');
      });
}

/// Division function. Returns 0 if division is by 0.
template <typename T, class RNG>
inline Primitive<T> divFn([[maybe_unused]] RNG &rng) {
  return Primitive<T>(
      2,
      [](const auto &input, const auto &children) {
        const T &secondResult = children[1].eval(input);
        return safeDiv(children[0].eval(input), children[1].eval(input), 0);
      },
      [](const auto &children) {
        return strCat('(', children[0].str(), " / ", children[1].str(), ')');
      });
}

/// Logarithm function.
template <typename T, class RNG>
inline Primitive<T> logFn([[maybe_unused]] RNG &rng) {
  return Primitive<T>(1,
                      [](const auto &input, const auto &children) {
                        return std::log(children[0].eval(input));
                      },
                      [](const auto &children) {
                        return strCat("log(", children[0].str(), ')');
                      });
}

/// Constant terminal. Returns a random value between 0 and 1.
template <typename T, class RNG> inline Primitive<T> constTerm(RNG &rng) {
  std::uniform_real_distribution<T> distr(-1, 1);
  const T value = distr(rng);

  return Primitive<T>(
      0,
      [value]([[maybe_unused]] const auto &input,
              [[maybe_unused]] const auto &children) { return value; },
      [value]([[maybe_unused]] const auto &children) {
        return std::string(value);
      });
}

/// Variable terminal. Returns the value of the given variable.
template <typename T> inline Primitive<T> makeVarTerm(const std::string &var) {
  return [&var]([[maybe_unused]] auto &rng) {
    return Primitive<T>(
        0,
        [&var](const auto &input, [[maybe_unused]] const auto &children) {
          return input[var];
        },
        [&var]([[maybe_unused]] const auto &children) { return var; });
  };
}

#endif // !COMPNAT_TP1_OPERATORS_HPP

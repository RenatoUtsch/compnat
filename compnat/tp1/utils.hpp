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

#ifndef COMPNAT_TP1_UTILS_HPP
#define COMPNAT_TP1_UTILS_HPP

#include <cmath>
#include <limits>
#include <sstream>
#include <string>

/// Safe division, returns default if b is 0.
template <typename T> T safeDiv(const T &a, const T &b, const T &def = 0) {
  // Simplification to also support integers. Considers the epsilon as 0.
  if (std::abs(b) <= std::numeric_limits<T>::epsilon()) {
    return def;
  }
  return a / b;
}

template <typename T> void strCatJoiner_(std::stringstream &ss, const T &t) {
  ss << t;
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

#endif // !COMPNAT_TP1_UTILS_HPP

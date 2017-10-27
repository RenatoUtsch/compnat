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

#ifndef COMPNAT_TP2_REPRESENTATION_HPP
#define COMPNAT_TP2_REPRESENTATION_HPP

#include <random>
#include <vector>

#include <glm/glm.hpp>

namespace tp2 {

// Random number generator used in the system.
using RNG = std::mt19937;

/**
 * Represents a point in the dataset.
 */
struct Point {
  glm::vec2 position;
  int capacity;
  int demand;
};

/**
 * Represents a dataset.
 */
class Dataset {

public:
  /// Constructs the dataset from the given file.
  Dataset(const char *filename);

  /// Returns the points of the dataset.
  const std::vector<Point> &points() const { return points_; }

  /// Returns the number of points on the dataset.
  size_t numPoints() const { return numPoints_; }

  /// Returns the number of medians on the dataset.
  size_t numMedians() const { return numMedians_; }

private:
  /**
   * Parses the input file and sets the internal fields.
   * @param filename The input filename.
   */
  void parseInputFile_(const char *filename);

  std::vector<Point> points_;
  size_t numPoints_;
  size_t numMedians_;
};

} // namespace tp2

#endif // !COMPNAT_TP2_REPRESENTATION_HPP

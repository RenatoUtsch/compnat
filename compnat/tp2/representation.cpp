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

#include "representation.hpp"

#include <fstream>
#include <tuple>

#include <glog/logging.h>

namespace tp2 {
namespace {} // namespace

Dataset::Dataset(const char *filename) { parseInputFile_(filename); }

void Dataset::parseInputFile_(const char *filename) {
  std::ifstream in(filename);
  CHECK(in.is_open()) << "Failed to open input dataset file.";

  in >> numPoints_ >> numMedians_;

  Point p;
  for (size_t i = 0; i < numPoints_; ++i) {
    in >> p.position.x >> p.position.y >> p.capacity >> p.demand;
    points_.push_back(p);
  }
}

} // namespace tp2

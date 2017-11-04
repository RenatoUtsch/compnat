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

#ifndef COMPNAT_TP2_GAP_HPP
#define COMPNAT_TP2_GAP_HPP

#include <vector>

#include "representation.hpp"

namespace tp2 {

/**
 * Solves the GAP problem. Returns the solution distance. If
 * empty, it's not possible to solve the problem.
 */
float gap(const Dataset &dataset, const std::vector<size_t> &clients,
          const std::vector<size_t> &medians);

} // namespace tp2

#endif // !COMPNAT_TP2_GAP_HPP

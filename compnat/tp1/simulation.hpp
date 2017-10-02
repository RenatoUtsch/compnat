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

#ifndef COMPNAT_TP1_SIMULATION_HPP
#define COMPNAT_TP1_SIMULATION_HPP

#include <utility>
#include <vector>

#include "representation.hpp"
#include "statistics.hpp"

namespace simulation {

/**
 * Runs the entire GA simulation for the given params and datasets.
 */
std::pair<std::vector<std::vector<stats::Statistics>>,
          std::vector<std::vector<stats::Statistics>>>
simulate(const repr::Params &params, const repr::Dataset &trainDataset,
         const repr::Dataset &testDataset);

} // namespace simulation

#endif // !COMPNAT_TP1_SIMULATION_HPP

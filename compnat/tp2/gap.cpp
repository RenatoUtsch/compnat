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

#include "gap.hpp"

#include <algorithm>
#include <utility>

#include <glm/glm.hpp>
#include <glog/logging.h>

#include "representation.hpp"

namespace tp2 {
namespace {

std::vector<std::pair<size_t, std::vector<size_t>>>
buildSortedClientMedians_(const Dataset &dataset,
                          const std::vector<size_t> &clients,
                          const std::vector<size_t> &medians) {
  std::vector<std::pair<size_t, std::vector<size_t>>> sortedClientMedians;
  for (size_t i : clients) {
    auto clientMedians = medians;
    std::sort(clientMedians.begin(), clientMedians.end(),
              [&](size_t a, size_t b) {
                return glm::distance(dataset.point(i).position,
                                     dataset.point(a).position) <
                       glm::distance(dataset.point(i).position,
                                     dataset.point(b).position);
              });

    sortedClientMedians.emplace_back(clients[i], std::move(clientMedians));
  }

  std::sort(
      sortedClientMedians.begin(), sortedClientMedians.end(),
      [](const auto &a, const auto &b) { return a.second[0] < b.second[0]; });

  return sortedClientMedians;
}

} // namespace

std::vector<size_t> gap(const Dataset &dataset,
                        const std::vector<size_t> &clients,
                        const std::vector<size_t> &medians) {
  const auto sortedClientMedians =
      buildSortedClientMedians_(dataset, clients, medians);

  std::vector<float> capacities(dataset.numPoints(), 0.0f);
  std::vector<size_t> assignedMedians(dataset.numPoints());
  for (size_t median : medians) {
    const auto &p = dataset.point(median);
    capacities[median] = p.capacity - p.demand;
    assignedMedians[median] = median;
  }

  for (const auto &p : sortedClientMedians) {
    const auto & [ client, clientMedians ] = p;
    bool foundMedian = false;
    for (size_t median : clientMedians) {
      const float demand = dataset.point(client).demand;
      if (demand <= capacities[median]) {
        capacities[median] -= demand;
        assignedMedians[client] = median;
        foundMedian = true;
        break;
      }
    }
    LOG(ERROR) << "Invalid median attribution found!";
    return {};
  }

  return assignedMedians;
}

} // namespace tp2

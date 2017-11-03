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
#include <glm/gtx/norm.hpp>
#include <glog/logging.h>

#include "representation.hpp"

namespace tp2 {
namespace {

std::vector<std::pair<size_t, std::vector<std::pair<size_t, float>>>>
buildSortedClientMedians_(const Dataset &dataset,
                          const std::vector<size_t> &clients,
                          const std::vector<size_t> &medians) {
  // God forgive me because I have sinned
  // Vector of (pair of (client index, vector of (pair of (median index,
  // distance from client to median))))
  std::vector<std::pair<size_t, std::vector<std::pair<size_t, float>>>>
      sortedClientMedians;
  for (size_t client : clients) {
    std::vector<std::pair<size_t, float>> clientMedians;
    for (size_t median : medians) {
      clientMedians.emplace_back(median,
                                 glm::distance(dataset.point(client).position,
                                               dataset.point(median).position));
    }
    std::sort(
        clientMedians.begin(), clientMedians.end(),
        [&](const auto &a, const auto &b) { return a.second < b.second; });

    sortedClientMedians.emplace_back(client, std::move(clientMedians));
  }

  std::sort(sortedClientMedians.begin(), sortedClientMedians.end(),
            [&](const auto &a, const auto &b) {
              // The article's GAP heuristic is broken. If you sort by the
              // distance to the nearest median, you risk not having enough
              // capacity to allocate a client that demands a lot. The solution
              // is to sort by the client's demand in descending order,
              // guaranteeing they will always be attributed first.
              return dataset.point(a.first).demand >
                     dataset.point(b.first).demand;
              // return a.second[0].second < b.second[0].second;
            });

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
    const float demand = dataset.point(client).demand;
    bool foundMedian = false;
    for (const auto &m : clientMedians) {
      const size_t median = m.first;
      if (demand <= capacities[median]) {
        capacities[median] -= demand;
        assignedMedians[client] = median;
        foundMedian = true;
        break;
      }
    }
    if (!foundMedian) {
      LOG(ERROR) << "Invalid median attribution found! client: " << client;
      return {};
    }
  }

  return assignedMedians;
}

} // namespace tp2

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

#include "operators.hpp"

#include <functional>
#include <random>
#include <stack>

#include "generators.hpp"

namespace operators {

size_t tournamentSelection(repr::RNG &rng, size_t tournamentSize,
                           const std::vector<repr::T> &fitnesses) {
  std::uniform_int_distribution<size_t> distr(0, fitnesses.size() - 1);
  size_t best = distr(rng);

  for (size_t i = 1; i < tournamentSize; ++i) {
    size_t candidate = distr(rng);
    if (fitnesses[candidate] < fitnesses[best]) {
      best = candidate;
    }
  }

  return best;
}

std::tuple<repr::Node &, size_t>
randomTreePoint(repr::RNG &rng, repr::Node &root, size_t size) {
  std::uniform_int_distribution<size_t> distr(0, size - 1);
  const size_t selectedPoint = distr(rng);

  std::stack<std::tuple<repr::Node &, size_t>> s;
  s.emplace(root, 1);
  for (size_t current = 0;; ++current) {
    auto[node, height] = s.top();
    if (current == selectedPoint) {
      return {node, height};
    }

    s.pop();
    for (size_t i = 0; i < node.numChildren(); ++i) {
      s.emplace(node.mutableChild(i), height + 1);
    }
  }
}

size_t maxNodeHeight(const repr::Node &root, size_t maxHeight) {
  std::stack<std::tuple<const repr::Node &, size_t>> s;
  s.emplace(root, 1);

  size_t finalHeight = 0;
  while (!s.empty()) {
    auto[node, height] = s.top();
    s.pop();

    if (finalHeight < height) {
      finalHeight = height;
    }
    if (height == maxHeight) {
      return maxHeight;
    }

    for (size_t i = 0; i < node.numChildren(); ++i) {
      s.emplace(node.child(i), height + 1);
    }
  }

  return finalHeight;
}

std::pair<repr::Node, repr::Node>
crossover(repr::RNG &rng, const repr::Params &params, const repr::Node &parentX,
          size_t sizeX, const repr::Node &parentY, size_t sizeY) {
  repr::Node childX = parentX;
  repr::Node childY = parentY;

  auto[crossPointX, heightPointX] = randomTreePoint(rng, childX, sizeX);
  auto[crossPointY, heightPointY] = randomTreePoint(rng, childY, sizeY);

  const auto heightCrossX =
      maxNodeHeight(crossPointX, params.maxHeight - heightPointX + 1);
  const auto heightCrossY =
      maxNodeHeight(crossPointY, params.maxHeight - heightPointY + 1);

  std::swap(crossPointX, crossPointY);

  return {
      heightPointX + heightCrossY - 1 > params.maxHeight ? parentX : childX,
      heightPointY + heightCrossX - 1 > params.maxHeight ? parentY : childY,
  };
}

repr::Node mutation(repr::RNG &rng, const repr::Params &params,
                    const repr::Node &parent, size_t size) {
  repr::Node child = parent;

  auto[mutationNode, height] = randomTreePoint(rng, child, size);
  mutationNode = generators::grow(rng, params.maxHeight - height + 1,
                                  params.functions, params.terminals);

  return child;
}

std::pair<std::vector<repr::Node>, stats::ImprovementMetadata>
newGeneration(repr::RNG &rng, const repr::Params &params,
              const std::vector<repr::Node> &parentPopulation,
              const std::vector<double> &parentFitnesses,
              const std::vector<size_t> &parentSizes,
              const stats::Statistics &parentStats) {
  CHECK(params.crossoverProb >= 0.0 && params.crossoverProb < 1.0);

  size_t i = 0;
  std::vector<repr::Node> newPopulation;
  if (params.elitism) {
    // Make a copy.
    newPopulation.push_back(parentPopulation[parentStats.best]);
    i++;
  }

  std::uniform_real_distribution<double> distr(0.0, 1.0);
  stats::ImprovementMetadata metadata;
  while (newPopulation.size() < parentPopulation.size()) {
    const size_t p1 =
        tournamentSelection(rng, params.tournamentSize, parentFitnesses);
    const auto p1Fitness = parentFitnesses[p1];
    const size_t p2 =
        tournamentSelection(rng, params.tournamentSize, parentFitnesses);
    const auto p2Fitness = parentFitnesses[p2];

    if (distr(rng) <= params.crossoverProb) { // Crossover
      auto[c1, c2] =
          crossover(rng, params, parentPopulation[p1], parentSizes[p1],
                    parentPopulation[p2], parentSizes[p2]);
      newPopulation.push_back(std::move(c1));
      newPopulation.push_back(std::move(c2));

      const auto avgParentFitness = (p1Fitness + p2Fitness) / 2.0;
      metadata.crossoverAvgParentFitness.emplace_back(i++, avgParentFitness);
      metadata.crossoverAvgParentFitness.emplace_back(i++, avgParentFitness);
    } else { // Mutation
      newPopulation.push_back(
          mutation(rng, params, parentPopulation[p1], parentSizes[p1]));
      newPopulation.push_back(
          mutation(rng, params, parentPopulation[p2], parentSizes[p2]));

      metadata.mutationParentFitness.emplace_back(i++, p1Fitness);
      metadata.mutationParentFitness.emplace_back(i++, p2Fitness);
    }
  }

  // We added always two new individuals, so we may have exceeded the population
  // size.
  if (newPopulation.size() > parentPopulation.size()) {
    CHECK(newPopulation.size() == parentPopulation.size() + 1);
    newPopulation.pop_back();
  }
  return {newPopulation, metadata};
}

} // namespace operators

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

#include <functional>
#include <random>
#include <stack>
#include <utility>
#include <vector>

#include "generators.hpp"
#include "representation.hpp"
#include "statistics.hpp"

namespace operators {

/**
 * Realizes tournament selection in the population.
 * Candidates may repeat when doing the tournament, but that's not a problem.
 * @param rng Random number generator.
 * @param tournamentSize Size of the tournament.
 * @param fitness Fitness of the individuals of the population
 * @return The index of the individual with the best fitness in the tournament.
 */
template <typename T, class RNG>
size_t tournamentSelection(RNG &rng, size_t tournamentSize,
                           const std::vector<T> &fitness) {
  std::uniform_int_distribution<size_t> distr(0, fitness.size() - 1);
  size_t best = distr(rng);

  for (size_t i = 1; i < tournamentSize; ++i) {
    size_t candidate = distr(rng);
    if (fitness[candidate] < fitness[best]) {
      best = candidate;
    }
  }

  return best;
}

/**
 * Uses a traversal to select a random tree point.
 * @param rng Random number generator.
 * @param root Tree to select a node.
 * @param size Size of the tree.
 * @return Reference to the selected node.
 */
template <typename T, class RNG>
Node<T, RNG> &randomTreePoint(RNG &rng, Node<T, RNG> &root, size_t size) {
  std::uniform_int_distribution<size_t> distr(0, size - 1);
  const size_t selectedPoint = distr(rng);

  std::stack<std::reference_wrapper<Node<T, RNG>>> s;
  s.push(root);
  for (size_t current = 0;; ++current) {
    Node<T, RNG> &node = s.top();
    if (current == selectedPoint) {
      return node;
    }

    s.pop();
    for (size_t i = 0; i < node.numChildren(); ++i) {
      s.push(node.mutableChild(i));
    }
  }
}

/**
 * Realizes crossover on the two trees.
 * @param rng Random number generator.
 * @param parentX First parent tree.
 * @param sizeX Number of nodes in the parent tree.
 * @param parentY Second parent tree.
 * @param sizeY Number of nodes in the parent tree.
 */
template <typename T, class RNG>
std::pair<Node<T, RNG>, Node<T, RNG>>
crossover(RNG &rng, const Node<T, RNG> &parentX, size_t sizeX,
          const Node<T, RNG> &parentY, size_t sizeY) {
  Node<T, RNG> childX = parentX;
  Node<T, RNG> childY = parentY;

  auto &crossNodeX = randomTreePoint(rng, childX, sizeX);
  auto &crossNodeY = randomTreePoint(rng, childY, sizeY);
  std::swap(crossNodeX, crossNodeY);

  return {childX, childY};
}

/**
 * Realizes mutation on the given tree.
 * @param rng Random number generator.
 * @param params Genetic algorithm params.
 * @param parent Parent tree to mutate.
 * @param size Number of nodes in the parent tree.
 */
template <typename T, class RNG>
Node<T, RNG> mutation(RNG &rng, const Params<T, RNG> &params,
                      const Node<T, RNG> &parent, size_t size) {
  Node<T, RNG> child = parent;

  auto &mutationNode = randomTreePoint(rng, child, size);
  mutationNode = generators::grow(rng, params.maxHeight, params.functions,
                                  params.terminals);

  return child;
}

/**
 * Generates a new population from an existing one.
 * @param rng Random number generator.
 * @param params Genetic programming params.
 * @param population Parent generation.
 * @param statistics Statistics of the parent generation.
 * @return Tuple containing the new population, the indices of crossover
 *   children and indices of mutation children.
 */
template <typename T, class RNG>
std::pair<std::vector<Node<T, RNG>>, stats::ImprovementMetadata>
newGeneration(RNG &rng, const Params<T, RNG> &params,
              const std::vector<Node<T, RNG>> &parentPopulation,
              const stats::Statistics<T, RNG> &parentStats) {
  CHECK(params.crossoverProb >= 0.0 && params.crossoverProb < 1.0);

  size_t i = 0;
  std::vector<Node<T, RNG>> newPopulation;
  if (params.elitism) {
    // Make a copy.
    newPopulation.push_back(parentPopulation[parentStats.best]);
    i++;
  }

  std::uniform_real_distribution<double> distr(0.0, 1.0);
  stats::ImprovementMetadata metadata;
  while (newPopulation.size() < parentPopulation.size()) {
    const size_t p1 =
        tournamentSelection(rng, params.tournamentSize, parentStats.fitness);
    const auto p1Fitness = parentStats.fitness[p1];
    const size_t p2 =
        tournamentSelection(rng, params.tournamentSize, parentStats.fitness);
    const auto p2Fitness = parentStats.fitness[p2];

    if (distr(rng) <= params.crossoverProb) { // Crossover
      auto[c1, c2] = crossover(rng, parentPopulation[p1], parentStats.sizes[p1],
                               parentPopulation[p2], parentStats.sizes[p2]);
      newPopulation.push_back(std::move(c1));
      newPopulation.push_back(std::move(c2));

      const auto avgParentFitness = (p1Fitness + p2Fitness) / 2.0;
      metadata.crossoverAvgParentFitness.emplace_back(i++, avgParentFitness);
      metadata.crossoverAvgParentFitness.emplace_back(i++, avgParentFitness);
    } else { // Mutation
      newPopulation.push_back(
          mutation(rng, params, parentPopulation[p1], parentStats.sizes[p1]));
      newPopulation.push_back(
          mutation(rng, params, parentPopulation[p2], parentStats.sizes[p2]));

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

#endif // !COMPNAT_TP1_OPERATORS_HPP

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

#include <random>
#include <stack>
#include <utility>
#include <vector>

#include "generators.hpp"
#include "representation.hpp"
#include "statistics.hpp"

namespace operators {

/**
 * Pushes children into a stack.
 */
template <typename T, class RNG>
void pushChildren_(std::stack<std::pair<Node<T, RNG> *, size_t>> &s,
                   Node<T, RNG> *node) {
  for (size_t i = 0; i < node->numChildren(); ++i) {
    s.push({node, i});
  }
}

/**
 * Uses a traversal to select a random tree point, except for the root.
 * @param rng Random number generator.
 * @param root Tree to select a node.
 * @param size Size of the tree.
 * @return Mutable reference to the parent's node and index into the child's
 * list to the selected point.
 */
template <typename T, class RNG>
std::pair<Node<T, RNG> *, size_t> randomTreePoint(RNG &rng, Node<T, RNG> &root,
                                                  size_t size) {
  // Start from 1 to not select the root.
  std::uniform_int_distribution<size_t> distr(1, size - 1);
  const size_t selectedPoint = distr(rng);

  std::stack<std::pair<Node<T, RNG> *, size_t>> s;
  pushChildren_(s, &root);
  for (size_t current = 1;; ++current) {
    auto[parent, childIndex] = s.top();
    s.pop();

    if (current == selectedPoint) {
      return {parent, childIndex};
    }

    pushChildren_(s, &parent->mutableChild(childIndex));
  }
}

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
    if (fitness[candidate] > fitness[best]) {
      best = candidate;
    }
  }

  return best;
}

template <typename T, class RNG>
std::pair<Node<T, RNG>, Node<T, RNG>>
crossover(RNG &rng, const Node<T, RNG> &x, size_t sizeX, const Node<T, RNG> &y,
          size_t sizeY) {
  Node<T, RNG> childX = x;
  Node<T, RNG> childY = y;
  auto[crossPointX, childIndexX] = randomTreePoint(rng, childX, sizeX);
  auto[crossPointY, childIndexY] = randomTreePoint(rng, childY, sizeY);
  std::swap(crossPointX->mutableChild(childIndexX),
            crossPointY->mutableChild(childIndexY));

  return {childX, childY};
}

template <typename T, class RNG>
Node<T, RNG> mutation(RNG &rng, const Params<T, RNG> &params,
                      const Node<T, RNG> &individual, size_t size) {
  Node<T, RNG> child = individual;
  auto[mutationPoint, childIndex] = randomTreePoint(rng, child, size);

  // TODO(renatoutsch): check if params.maxHeight here is appropriate.
  mutationPoint->setChild(childIndex,
                          generators::grow(rng, params.maxHeight,
                                           params.functions, params.terminals));
  return child;
}

/**
 * Generates a new population from an existing one.
 * @param rng Random number generator.
 * @param params Genetic programming params.
 * @param population Parent generation.
 * @param statistics Statistics of the parent generation.
 * @return Pair containing the new population and the indexes of crossover
 *   children.
 */
template <typename T, class RNG>
std::pair<std::vector<Node<T, RNG>>, std::vector<size_t>>
newGeneration(RNG &rng, const Params<T, RNG> &params,
              const std::vector<Node<T, RNG>> &parentPopulation,
              const stats::Statistics<T, RNG> &parentStats) {
  CHECK(params.crossoverProb >= 0.0 && params.crossoverProb < 1.0);

  std::vector<Node<T, RNG>> newPopulation;
  if (params.elitism) {
    // Make a copy.
    newPopulation.push_back(parentPopulation[parentStats.best]);
  }

  std::uniform_real_distribution<> distr(0.0, 1.0);
  std::vector<size_t> crossoverIndices;
  size_t i = 0;
  while (newPopulation.size() < parentPopulation.size()) {
    const size_t p1 =
        tournamentSelection(rng, params.tournamentSize, parentStats.fitness);
    const size_t p2 =
        tournamentSelection(rng, params.tournamentSize, parentStats.fitness);

    if (distr(rng) <= params.crossoverProb) { // Crossover
      auto[c1, c2] = crossover(rng, parentPopulation[p1], parentStats.sizes[p1],
                               parentPopulation[p2], parentStats.sizes[p2]);
      newPopulation.push_back(std::move(c1));
      newPopulation.push_back(std::move(c2));
      crossoverIndices.push_back(i++);
      crossoverIndices.push_back(i++);
    } else { // Mutation
      newPopulation.push_back(
          mutation(rng, params, parentPopulation[p1], parentStats.sizes[p1]));
      newPopulation.push_back(
          mutation(rng, params, parentPopulation[p2], parentStats.sizes[p2]));
      i += 2;
    }
  }

  // We added always two new individuals, so we may have exceeded the population
  // size.
  if (newPopulation.size() > parentPopulation.size()) {
    CHECK(newPopulation.size() == parentPopulation.size() + 1);
    newPopulation.pop_back();
  }
  return {newPopulation, crossoverIndices};
}

} // namespace operators

#endif // !COMPNAT_TP1_OPERATORS_HPP

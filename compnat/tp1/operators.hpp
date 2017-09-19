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
#include <utility>
#include <vector>

#include "generators.hpp"
#include "representation.hpp"

/**
 * Realizes tournament selection in the population.
 * @param rng Random number generator.
 * @param k Size of the tournament.
 * @param fitness Fitness of the individuals of the population
 * @return The index of the individual with the best fitness in the tournament.
 */
template <typename T, class RNG>
size_t tournamentSelection(RNG &rng, int k, const std::vector<T> &fitness) {
  const std::uniform_int_distribution<size_t> distr(0, fitness.size() - 1);
  size_t best = distr(rng);

  // TODO(renatoutsch): maybe take care to not repeat candidates?
  for (int i = 1; i < k; ++i) {
    size_t candidate = distr(rng);
    if (fitness[candidate] > fitness[best]) {
      best = candidate;
    }
  }

  return best;
}

template <typename T, class RNG>
std::pair<Node<T>, Node<T>> crossover(RNG &rng, const Node<T> &x, size_t sizeX,
                                      const Node<T> &y, size_t sizeY) {
  const std::uniform_int_distribution<size_t> distrX(0, sizeX - 1);
  const std::uniform_int_distribution<size_t> distrY(0, sizeY - 1);
  const size_t mutationPointX = distrX(rng);
  const size_t mutationPointY = distrY(rng);

  auto[mutationNodeX, childIndexX] = findMutationPoint(x, mutationPointX);
  auto[mutationNodeY, childIndexY] = findMutationPoint(y, mutationPointY);

  Node<T> childX = x;
  Node<T> childY = y;
  std::swap(childX.mutableChild(childIndexX), childY.mutableChild(childIndexY));

  return {childX, childY};
}

template <typename T, class RNG>
Node<T> mutation(RNG &rng, int maxHeight, const Node<T> &individual,
                 size_t size, const std::vector<Primitive<T>> &functions,
                 const std::vector<Primitive<T>> &terminals) {
  const std::uniform_int_distribution<size_t> distr(0, size - 1);
  const size_t mutationPoint = distr(rng);

  auto[mutationNode, childIndex] = findMutationPoint(individual, mutationPoint);
  mutationNode.setChild(childIndex, grow(rng, maxHeight, functions, terminals));
}

/**
 * Generates a new population from an existing one.
 * @param rng The random number generator.
 * @param k The tournament size.
 * @param crossoverProbability Probability of doing a crossover. Must be in the
 *   range [0, 1).
 * @param elitism If should use elitism or not.
 * @param population Old population.
 * @param fitness Fitness of the old population, in the same order of elements.
 * @param sizes Size of each individual.
 * @param statistics Statistics of the old population.
 */
template <typename T, class RNG>
std::vector<Node<T>>
generateNewPopulation(RNG &rng, int k, double crossoverProbability,
                      bool elitism, const std::vector<Node<T>> &population,
                      const std::vector<T> &fitness,
                      const std::vector<T> &sizes) {
  CHECK(crossoverProbability >= 0.0 && crossoverProbability < 1.0);

  std::vector<Node<T>> newPopulation;
  if (elitism) {
    /*   newPopulation.push_back(population[statistics.best]); // Make a copy.
     */
  }

  std::uniform_real_distribution<> distr(0.0, 1.0);
  while (newPopulation.size() < population.size()) {
    const auto &p1 = tournamentSelection(rng, k, fitness);
    const auto &p2 = tournamentSelection(rng, k, fitness);
    if (distr(rng) <= crossoverProbability) { // Crossover
      auto[c1, c2] = crossover(rng, p1, p2);
      newPopulation.push_back(std::move(c1));
      newPopulation.push_back(std::move(c2));
    } else { // Mutation
      newPopulation.push_back(mutation(rng, p1));
      newPopulation.push_back(mutation(rng, p2));
    }
  }

  // We added always two new individuals, so we may have exceeded the population
  // size.
  if (newPopulation.size() > population.size()) {
    CHECK(newPopulation.size() == population.size() + 1);
    newPopulation.pop_back();
  }
  return newPopulation;
}

#endif // !COMPNAT_TP1_OPERATORS_HPP

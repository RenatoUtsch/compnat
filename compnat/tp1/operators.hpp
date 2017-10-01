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

#include <tuple>
#include <utility>
#include <vector>

#include "representation.hpp"
#include "statistics.hpp"

namespace operators {

/**
 * Realizes tournament selection in the population.
 * Candidates may repeat when doing the tournament, but that's not a problem.
 * @param rng Random number generator.
 * @param tournamentSize Size of the tournament.
 * @param fitnesses Fitness of the individuals of the population
 * @return The index of the individual with the best fitness in the tournament.
 */
size_t tournamentSelection(repr::RNG &rng, size_t tournamentSize,
                           const std::vector<repr::T> &fitnesses);

/**
 * Uses a traversal to select a random tree point.
 * @param rng Random number generator.
 * @param root Tree to select a node.
 * @param size Size of the tree.
 * @return Pair containing reference to the selected node and the height of the
 *   node.
 */
std::tuple<repr::Node &, size_t> randomTreePoint(repr::RNG &rng,
                                                 repr::Node &root, size_t size);

/**
 * Returns the maximum height of a node.
 * @param root The node to have it's height calculated.
 * @param maxHeight Maximum height of the node. If it reaches this, the function
 *   doesn't need to search anymore.
 * @return the maximum height of the node.
 */
size_t maxNodeHeight(const repr::Node &root, size_t maxHeight);

/**
 * Realizes crossover on the two trees.
 * @param rng Random number generator.
 * @param params Genetic algorithm params.
 * @param parentX First parent tree.
 * @param sizeX Number of nodes in the parent tree.
 * @param parentY Second parent tree.
 * @param sizeY Number of nodes in the parent tree.
 */
std::pair<repr::Node, repr::Node>
crossover(repr::RNG &rng, const repr::Params &params, const repr::Node &parentX,
          size_t sizeX, const repr::Node &parentY, size_t sizeY);

/**
 * Realizes mutation on the given tree.
 * @param rng Random number generator.
 * @param params Genetic algorithm params.
 * @param parent Parent tree to mutate.
 * @param size Number of nodes in the parent tree.
 */
repr::Node mutation(repr::RNG &rng, const repr::Params &params,
                    const repr::Node &parent, size_t size);

/**
 * Generates a new population from an existing one.
 * @param rng Random number generator.
 * @param params Genetic programming params.
 * @param parentPopulation Parent generation.
 * @param parentFitnesses Fitnesses of the parent population.
 * @param parnentSizes Sizes of the parent population.
 * @param parentStats Statistics of the parent generation.
 * @return Tuple containing the new population, the indices of crossover
 *   children and indices of mutation children.
 */
std::pair<std::vector<repr::Node>, stats::ImprovementMetadata>
newGeneration(repr::RNG &rng, const repr::Params &params,
              const std::vector<repr::Node> &parentPopulation,
              const std::vector<double> &parentFitnesses,
              const std::vector<size_t> &parentSizes,
              const stats::Statistics &parentStats);

} // namespace operators

#endif // !COMPNAT_TP1_OPERATORS_HPP

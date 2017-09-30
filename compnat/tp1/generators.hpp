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

#ifndef COMPNAT_TP1_GENERATORS_HPP
#define COMPNAT_TP1_GENERATORS_HPP

#include <vector>

#include "representation.hpp"

namespace generators {

/// Returns a random primitive.
repr::Primitive
randomPrimitive(repr::RNG &rng,
                const std::vector<repr::PrimitiveFn> &primitiveFns);

/// Returns a random function or terminal.
repr::Primitive
randomPrimitive(repr::RNG &rng, const std::vector<repr::PrimitiveFn> &functions,
                const std::vector<repr::PrimitiveFn> &terminals);

/**
 * Implements the grow method for creating trees.
 * @param rng Random number generator.
 * @param maxHeight The maximum height of the generated tree.
 * @param functions Function primitives.
 * @param terminals Terminal primitives.
 */
repr::Node grow(repr::RNG &rng, size_t maxHeight,
                const std::vector<repr::PrimitiveFn> &functions,
                const std::vector<repr::PrimitiveFn> &terminals);

/**
 * Implements the full method for creating trees.
 * @param rng Random number generator.
 * @param maxHeight The maximum height of the generated tree.
 * @param functions Function primitives.
 * @param terminals Terminal primitives.
 */
repr::Node full(repr::RNG &rng, size_t maxHeight,
                const std::vector<repr::PrimitiveFn> &functions,
                const std::vector<repr::PrimitiveFn> &terminals);

/**
 * Generates trees using the ramped half and half method.
 * @param params Genetic Programming parameters. PopulationSize must be even and
 *   a multiple of (maxHeight - 1).
 * @param rng Random number generator.
 */
std::vector<repr::Node> rampedHalfAndHalf(repr::RNG &rng,
                                          const repr::Params &params);

} // namespace generators

#endif // !COMPNAT_TP1_GENERATORS_HPP

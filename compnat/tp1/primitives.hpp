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

#ifndef COMPNAT_TP1_PRIMITIVES_HPP
#define COMPNAT_TP1_PRIMITIVES_HPP

#include <string>

#include "representation.hpp"

namespace primitives {

/// Sum function.
repr::Primitive sumFn([[maybe_unused]] repr::RNG &rng);

/// Subtraction function.
repr::Primitive subFn([[maybe_unused]] repr::RNG &rng);

/// Multiplication function.
repr::Primitive multFn([[maybe_unused]] repr::RNG &rng);

/// Division function. Returns 0 if division is by 0.
repr::Primitive divFn([[maybe_unused]] repr::RNG &rng);

/// Logarithm function.
repr::Primitive logFn([[maybe_unused]] repr::RNG &rng);

/// Constant terminal. Returns a random value between -1 and 1.
repr::Primitive constTerm(repr::RNG &rng);

/// Literal terminal. Returns the given input value.
std::function<repr::Primitive(repr::RNG &)> literalTerm(repr::T value);

/// Variable terminal. Returns the value of the given variable.
std::function<repr::Primitive(repr::RNG &)> makeVarTerm(size_t var);

} // namespace primitives

#endif // !COMPNAT_TP1_PRIMITIVES_HPP

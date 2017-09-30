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

#include "primitives.hpp"

#include <random>

#include "utils.hpp"

namespace primitives {

repr::Primitive sumFn([[maybe_unused]] repr::RNG &rng) {
  return repr::Primitive( // Keep formatting
      2,
      [](const auto &input, const auto &children) {
        return children[0].eval(input) + children[1].eval(input);
      },
      [](const auto &children) {
        return utils::strCat('(', children[0].str(), " + ", children[1].str(),
                             ')');
      });
}

repr::Primitive subFn([[maybe_unused]] repr::RNG &rng) {
  return repr::Primitive( // Keep formatting
      2,
      [](const auto &input, const auto &children) {
        return children[0].eval(input) - children[1].eval(input);
      },
      [](const auto &children) {
        return utils::strCat('(', children[0].str(), " - ", children[1].str(),
                             ')');
      });
}

repr::Primitive multFn([[maybe_unused]] repr::RNG &rng) {
  return repr::Primitive( // Keep formatting
      2,
      [](const auto &input, const auto &children) {
        return children[0].eval(input) * children[1].eval(input);
      },
      [](const auto &children) {
        return utils::strCat('(', children[0].str(), " * ", children[1].str(),
                             ')');
      });
}

repr::Primitive divFn([[maybe_unused]] repr::RNG &rng) {
  return repr::Primitive( // Keep formatting
      2,
      [](const auto &input, const auto &children) {
        return utils::safeDiv(children[0].eval(input), children[1].eval(input));
      },
      [](const auto &children) {
        return utils::strCat('(', children[0].str(), " / ", children[1].str(),
                             ')');
      });
}

repr::Primitive logFn([[maybe_unused]] repr::RNG &rng) {
  return repr::Primitive( // Keep formatting
      1,
      [](const auto &input, const auto &children) {
        return std::log2(children[0].eval(input));
      },
      [](const auto &children) {
        return utils::strCat("log2(", children[0].str(), ')');
      });
}

repr::Primitive constTerm(repr::RNG &rng) {
  std::uniform_real_distribution<repr::T> distr(-1, 1);
  const repr::T value = distr(rng);

  return repr::Primitive( // Keep formatting
      0,
      [value]([[maybe_unused]] const auto &input,
              [[maybe_unused]] const auto &children) { return value; },
      [value]([[maybe_unused]] const auto &children) {
        return utils::strCat(value);
      });
}

std::function<repr::Primitive(repr::RNG &)> literalTerm(repr::T value) {
  return [value]([[maybe_unused]] auto &rng) {
    return repr::Primitive( // Keep formatting
        0,
        [value]([[maybe_unused]] const auto &input,
                [[maybe_unused]] const auto &children) { return value; },
        [value]([[maybe_unused]] const auto &children) {
          return utils::strCat(value);
        });
  };
}

std::function<repr::Primitive(repr::RNG &)> makeVarTerm(size_t var) {
  return [var]([[maybe_unused]] auto &rng) {
    return repr::Primitive( // Keep formatting
        0,
        [var](const auto &input, [[maybe_unused]] const auto &children) {
          return input[var];
        },
        [var]([[maybe_unused]] const auto &children) {
          return utils::strCat("x", var);
        });
  };
}

} // namespace primitives

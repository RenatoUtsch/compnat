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

#ifndef COMPNAT_TP1_REPRESENTATION_HPP
#define COMPNAT_TP1_REPRESENTATION_HPP

#include <cmath>
#include <functional>
#include <string>
#include <vector>

#include <glog/logging.h>

template <typename T, class RNG> class Node;

/**
 * Input for evaluation of a node.
 * Each index in the vector represents the corresponding variable's value.
 */
template <typename T> using EvalInput = std::vector<T>;

/// Vector representing children.
template <typename T, class RNG> using Children = std::vector<Node<T, RNG>>;

/// Function that evaluates an operator.
template <typename T, class RNG>
using EvalFn =
    std::function<T(const EvalInput<T> &input, const Children<T, RNG> &)>;

/// Function that converts an operator to string.
template <typename T, class RNG>
using StrFn = std::function<std::string(const Children<T, RNG> &)>;

/// Pair mapping inputs to output.
template <typename T> using Sample = std::pair<EvalInput<T>, T>;

/// Dataset of samples.
template <typename T> using Dataset = std::vector<Sample<T>>;

/// Represents an primitive.
template <typename T, class RNG> struct Primitive {
  int numRequiredChildren;
  EvalFn<T, RNG> evalFn;
  StrFn<T, RNG> strFn;

  operator bool() const { return evalFn && strFn; }

  Primitive() {}

  Primitive(int numRequiredChildren, EvalFn<T, RNG> evalFn, StrFn<T, RNG> strFn)
      : numRequiredChildren(numRequiredChildren), evalFn(evalFn), strFn(strFn) {
  }
};

/// Function that creates a new primitive.
template <typename T, class RNG>
using PrimitiveFn = std::function<Primitive<T, RNG>(RNG &rng)>;

/**
 * Represents the parameters used in the program.
 * TODO(renatoutsch): add accessors to always be sure populationSize is correct.
 */
template <typename T, class RNG> struct Params {
  /// RNG seed.
  unsigned seed;

  /// Number of generations to run.
  size_t numGenerations;

  /// Population size.
  size_t populationSize;

  /// Tournament size.
  size_t tournamentSize;

  /// Maximum tree height.
  size_t maxHeight;

  /// Maximum number of elements in the tree (aka 2^maxHeight).
  size_t maxElements;

  /// Multiplier to control bloating.
  double bloatFactor;

  /// Crossover probability.
  double crossoverProb;

  /// If using elitism.
  bool elitism;

  /// Available function primitives.
  std::vector<PrimitiveFn<T, RNG>> functions;

  /// Available terminal primitives.
  std::vector<PrimitiveFn<T, RNG>> terminals;

  Params(unsigned seed_, size_t numGenerations_, size_t populationSize_,
         size_t tournamentSize_, size_t maxHeight_, double bloatFactor_,
         double crossoverProb_, bool elitism_,
         const std::vector<PrimitiveFn<T, RNG>> &functions_,
         const std::vector<PrimitiveFn<T, RNG>> &terminals_)
      : seed(seed_), numGenerations(numGenerations_),
        populationSize(populationSize_), tournamentSize(tournamentSize_),
        maxHeight(maxHeight_), maxElements(std::pow(2, maxHeight) - 1),
        bloatFactor(bloatFactor_), crossoverProb(crossoverProb_),
        elitism(elitism_), functions(functions_), terminals(terminals_) {
    if (populationSize < maxHeight - 1) {
      LOG(WARNING) << "params: populationSize changed to maxHeight - 1";
      populationSize = maxHeight - 1;
    }

    const size_t exceeding = populationSize % (maxHeight - 1);
    if (exceeding) { // Make the population a multiple of (maxHeight - 1).
      const size_t increase = (maxHeight - 1) - exceeding;
      LOG(WARNING) << "params: population increase - " << increase;
      populationSize += increase;
    }

    if (populationSize % 2) {
      // Params needs to be even.
      LOG(WARNING) << "params: population increase to make buckets even - "
                   << maxHeight - 1;
      populationSize += maxHeight - 1;
    }

    LOG(INFO) << "Params:";
    LOG(INFO) << "seed: " << seed;
    LOG(INFO) << "numGenerations: " << numGenerations;
    LOG(INFO) << "populationSize: " << populationSize;
    LOG(INFO) << "tournamentSize: " << tournamentSize;
    LOG(INFO) << "maxHeight: " << maxHeight;
    LOG(INFO) << "crossoverProb: " << crossoverProb;
    LOG(INFO) << "elitism: " << elitism;
  }
};

/**
 * A tree node.
 * A node is represented by it's current children and it's Primitive, which
 * represents what it does and the required number of children.
 */
template <typename T, class RNG> class Node {

public:
  /**
   * Creates a tree node from the given primitive.
   */
  Node(const Primitive<T, RNG> &op = Primitive<T, RNG>())
      : numRequiredChildren_(op.numRequiredChildren), evalFn_(op.evalFn),
        strFn_(op.strFn) {
    if (evalFn_) {
      CHECK(strFn_);
      children_.resize(numRequiredChildren_);
    }
  }

  /// Evaluates the value of the node.
  T eval(const EvalInput<T> &input) const { return evalFn_(input, children_); }

  /// Returns the string equivalent of the tree.
  std::string str() const { return strFn_(children_); }

  /// Replaces a given child.
  void setChild(int i, const Node<T, RNG> &&newChild) {
    children_[i] = newChild;
  }

  /// Returns the child at index i.
  const Node<T, RNG> &child(int i) const { return children_[i]; }

  /// Returns the mutable child at index i.
  Node<T, RNG> &mutableChild(int i) { return children_[i]; }

  /// Number of children the node currently has.
  size_t numChildren() const { return children_.size(); }

  /// If the node is terminal.
  bool isTerminal() const { return evalFn_ && numRequiredChildren_ == 0; }

  /**
   * Size of the tree, aka number of elements in this entire subtree.
   * If using this multiple times, best to pre-compute it once and reuse it.
   */
  size_t size() const {
    size_t size = 1;
    for (const auto &child : children_) {
      size += child.size();
    }
    return size;
  }

private:
  int numRequiredChildren_;
  EvalFn<T, RNG> evalFn_;
  StrFn<T, RNG> strFn_;
  Children<T, RNG> children_;
};

#endif // !COMPNAT_TP1_REPRESENTATION_HPP

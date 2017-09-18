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

#ifndef COMPNAT_TP1_REPRESENTATION_H
#define COMPNAT_TP1_REPRESENTATION_H

#include <functional>
#include <glog/logging.h>
#include <string>
#include <unordered_map>
#include <vector>

template <typename T> class Node;

/**
 * Input for evaluation of a node.
 * It's a simple map from each input variable to it's value.
 */
template <typename T> using EvalInput = std::unordered_map<std::string, T>;

/// Vector representing children.
template <typename T> using Children = std::vector<Node<T>>;

template <typename T>
using EvalFn = std::function<T(const EvalInput<T> &input, const Children<T> &)>;

/// Represents an primitive.
template <typename T> struct Primitive {
  int numRequiredChildren;
  EvalFn<T> evalFn;

  operator bool() const { return evalFn; }

  Primitive() {}

  Primitive(int numRequiredChildren, EvalFn<T> evalFn)
      : numRequiredChildren(numRequiredChildren), evalFn(evalFn) {}
};

/**
 * A tree node.
 * A node is represented by it's current children and it's Primitive, which
 * represents what it does and the required number of children.
 */
template <typename T> class Node {

public:
  /**
   * Creates a tree node from the given primitive.
   */
  Node(const Primitive<T> &op = Primitive<T>()) : op_(op) {
    if (op_) {
      children_.resize(op.numRequiredChildren);
    }
  }

  /// Evaluates the value of the node.
  T eval(const EvalInput<T> &input) const {
    return op_.evalFn(input, children_);
  }

  /// Replaces a given child.
  void setChild(int i, const Node<T> &&newChild) { children_[i] = newChild; }

  /// Returns the child at index i.
  const Node<T> &child(int i) const { return children_[i]; }

  /// Number of children the node currently has.
  int numChildren() const { return children_.size(); }

  /// If the node is terminal.
  bool isTerminal() const { return op_ && op_.numRequiredChildren == 0; }

private:
  Primitive<T> op_;
  Children<T> children_;
};

#endif // !COMPNAT_TP1_REPRESENTATION_H

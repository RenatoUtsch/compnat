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

#ifndef COMPNAT_TP1_PARSER_HPP
#define COMPNAT_TP1_PARSER_HPP

#include <fstream>
#include <sstream>
#include <string>

#include <glog/logging.h>

#include "representation.hpp"
#include "utils.hpp"

namespace parser {

// https://stackoverflow.com/a/7408245/1099010
std::vector<std::string> splitLine(const std::string &text, char sep) {
  std::vector<std::string> tokens;
  std::size_t start = 0, end = 0;
  while ((end = text.find(sep, start)) != std::string::npos) {
    tokens.push_back(text.substr(start, end - start));
    start = end + 1;
  }
  tokens.push_back(text.substr(start));
  return tokens;
}

/**
 * Loads a dataset from a CSV file.
 */
template <typename T> Dataset<T> loadDataset(const std::string &filename) {
  Dataset<T> dataset;

  std::ifstream in(filename);
  CHECK(in.is_open());

  std::string line;
  while (std::getline(in, line)) {
    const auto &tokens = splitLine(line, ',');
    const size_t numInputs = tokens.size() - 1;

    EvalInput<T> input(numInputs);
    for (size_t i = 0; i < numInputs; ++i) {
      utils::strSplit(tokens[i], input[i]);
    }

    T expectedValue;
    utils::strSplit(tokens[tokens.size() - 1], expectedValue);
    dataset.push_back({input, expectedValue});
  }

  return dataset;
}

} // namespace parser

#endif // !COMPNAT_TP1_PARSER_HPP

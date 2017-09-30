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

#include <string>

#include "representation.hpp"

namespace parser {

// https://stackoverflow.com/a/7408245/1099010
std::vector<std::string> splitLine(const std::string &text, char sep);

/**
 * Loads a dataset from a CSV file.
 */
repr::Dataset loadDataset(const std::string &filename);

} // namespace parser

#endif // !COMPNAT_TP1_PARSER_HPP

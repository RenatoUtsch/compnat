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

#include <random>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "parser.hpp"
#include "primitives.hpp"
#include "representation.hpp"

DEFINE_string(dataset, "", "File containing the dataset.");
DEFINE_int32(seed, -1, "Initial seed (-1 to select at random).");
DEFINE_int32(num_generations, 50, "Number of generations to run.");
DEFINE_int32(population_size, 100, "Size of the population.");
DEFINE_int32(tournament_size, 7, "Size of the tournament.");
DEFINE_int32(max_height, 7, "Maximum tree height.");
DEFINE_double(crossover_prob, 0.9,
              "Crossover probability. Will use mutation otherwise.");
DEFINE_bool(elitism, false, "Whether to use elitism or not.");

int main(int argc, char **argv) {
  using T = double;
  using RNG = std::mt19937;

  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  if (FLAGS_seed == -1) {
    std::random_device rd;
    FLAGS_seed = rd();
  }

  const auto &dataset = parser::loadDataset<T>(FLAGS_dataset);

  const std::vector<PrimitiveFn<T, RNG>> functions = {
      primitives::sumFn<T, RNG>,  primitives::subFn<T, RNG>,
      primitives::multFn<T, RNG>, primitives::divFn<T, RNG>,
      primitives::logFn<T, RNG>,
  };

  std::vector<PrimitiveFn<T, RNG>> terminals;
  terminals.push_back(primitives::constTerm<T, RNG>);
  const auto &datasetSample = dataset[0].first;
  for (const auto & [ key, _ ] : datasetSample) {
    terminals.push_back(primitives::makeVarTerm<T, RNG>(key));
  }

  Params<> params(FLAGS_seed, FLAGS_num_generations, FLAGS_population_size,
                  FLAGS_tournament_size, FLAGS_max_height, FLAGS_crossover_prob,
                  FLAGS_elitism, functions, terminals);

  return 0;
}

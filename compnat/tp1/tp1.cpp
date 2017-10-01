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
#include "simulation.hpp"

DEFINE_string(dataset_train, "", "File containing the train dataset.");
DEFINE_string(dataset_test, "", "File containing the test dataset.");
DEFINE_string(output_file, "",
              "Output file for the execution data. "
              "Extension should be '.cnat'.");
DEFINE_int32(seed, -1, "Initial seed (-1 to select at random).");
DEFINE_int32(num_instances, 30, "Number of instances.");
DEFINE_int32(num_generations, 50, "Number of generations to run.");
DEFINE_int32(population_size, 100, "Size of the population.");
DEFINE_int32(tournament_size, 7, "Size of the tournament.");
DEFINE_int32(max_height, 7, "Maximum tree height.");
DEFINE_double(crossover_prob, 0.9,
              "Crossover probability. Will use mutation otherwise.");
DEFINE_bool(elitism, false, "Whether to use elitism or not.");
DEFINE_bool(always_test, false, "Run test dataset on all generations.");

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  if (FLAGS_seed == -1) {
    std::random_device rd;
    FLAGS_seed = rd();
  }

  const auto &trainDataset = parser::loadDataset(FLAGS_dataset_train);
  const auto &testDataset = parser::loadDataset(FLAGS_dataset_test);

  const std::vector<repr::PrimitiveFn> functions = {
      primitives::sumFn,
      primitives::subFn,
      primitives::multFn,
      primitives::divFn,
  };

  // Add the correct number of variable terminals.
  std::vector<repr::PrimitiveFn> terminals;
  terminals.push_back(primitives::constTerm);
  const auto &datasetInput = trainDataset[0].first;
  for (size_t i = 0; i < datasetInput.size(); ++i) {
    terminals.push_back(primitives::makeVarTerm(i));
  }

  repr::Params params(FLAGS_output_file, FLAGS_seed, FLAGS_num_instances,
                      FLAGS_num_generations, FLAGS_population_size,
                      FLAGS_tournament_size, FLAGS_max_height,
                      FLAGS_crossover_prob, FLAGS_elitism, FLAGS_always_test,
                      functions, terminals);

  simulation::simulate(params, trainDataset, testDataset);

  return 0;
}

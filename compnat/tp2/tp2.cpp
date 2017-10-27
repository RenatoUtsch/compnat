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

#include "representation.hpp"

DEFINE_string(dataset, "", "File containing the dataset.");
DEFINE_int32(seed, -1, "Initial seed (-1 to select at random).");
DEFINE_int32(num_instances, 30, "Number of instances.");
DEFINE_int32(num_iterations, 50, "Number of iterations of the algorithm.");

namespace {
/**
 * Returns a vector with the seeds for all instances of the algorithm.
 * @param seed Initial seed to use in the RNG. If < 0, selects a seed at random.
 * @param numSeeds Number of seeds to generate.
 * @return Vector with the seeds.
 */
std::vector<unsigned> generateSeeds_(int seed, int numSeeds) {
  std::random_device rd;
  tp2::RNG rng(seed == -1 ? rd() : (unsigned)seed);
  std::uniform_int_distribution<unsigned> distr;

  std::vector<unsigned> seeds;
  for (int i = 0; i < numSeeds; ++i) {
    seeds.push_back(distr(rng));
  }

  return seeds;
}

} // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  const auto seeds = generateSeeds_(FLAGS_seed, FLAGS_num_instances);
  const auto dataset = tp2::Dataset(FLAGS_dataset.c_str());

  for (int i = 0; i < FLAGS_num_instances; ++i) {
  }

  return 0;
}

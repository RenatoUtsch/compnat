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

#include <fstream>
#include <random>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "aco.hpp"
#include "compnat/tp2/results/results_generated.h"
#include "representation.hpp"

DEFINE_string(dataset, "", "File containing the dataset.");
DEFINE_string(output_file, "", "Output file with the results.");
DEFINE_int32(seed, -1, "Initial seed (-1 to select at random).");
DEFINE_int32(num_ants, -1, "Number of ants (-1 for n - p).");
DEFINE_int32(num_executions, 30, "Number of executions.");
DEFINE_int32(num_iterations, 50, "Number of iterations of the algorithm.");
DEFINE_double(decay, 0.01f, "Pheromone decay rate.");

namespace {
/**
 * Returns a vector with the seeds for all instances of the algorithm.
 * @param seed Initial seed to use in the RNG. If < 0, selects a seed at random.
 * @param numSeeds Number of seeds to generate.
 * @return Vector with the seeds.
 */
std::vector<unsigned> generateSeeds_(int seed, int numSeeds) {
  std::random_device rd;
  const unsigned realSeed = seed == -1 ? rd() : (unsigned)seed;
  LOG(INFO) << "seed: " << realSeed;

  tp2::RNG rng(realSeed);
  std::uniform_int_distribution<unsigned> distr;

  std::vector<unsigned> seeds;
  for (int i = 0; i < numSeeds; ++i) {
    seeds.push_back(distr(rng));
  }

  return seeds;
}

void saveToFile_(const std::string &outputFile, const uint8_t *buf,
                 size_t size) {
  std::ofstream out(outputFile, std::ofstream::out | std::ofstream::trunc |
                                    std::ofstream::binary);
  CHECK(out.is_open());

  out.write((const char *)buf, size);
}

flatbuffers::Offset<tp2::results::Params>
buildParams_(flatbuffers::FlatBufferBuilder &builder) {
  tp2::results::ParamsBuilder paramsBuilder(builder);
  paramsBuilder.add_seed(FLAGS_seed);
  paramsBuilder.add_numExecutions(FLAGS_num_executions);
  paramsBuilder.add_numIterations(FLAGS_num_iterations);
  paramsBuilder.add_numAnts(FLAGS_num_ants);
  paramsBuilder.add_decay(FLAGS_decay);
  return paramsBuilder.Finish();
}

flatbuffers::Offset<
    flatbuffers::Vector<flatbuffers::Offset<tp2::results::Iteration>>>
buildIterations_(flatbuffers::FlatBufferBuilder &builder,
                 const std::vector<tp2::Result> &results) {
  std::vector<flatbuffers::Offset<tp2::results::Iteration>> iterations;
  for (int i = 0; i < FLAGS_num_iterations; ++i) {
    std::vector<float> globalBests(FLAGS_num_executions);
    std::vector<float> localBests(FLAGS_num_executions);
    std::vector<float> localWorsts(FLAGS_num_executions);
    for (int j = 0; j < FLAGS_num_executions; ++j) {
      globalBests[j] = results[j].globalBests[i];
      localBests[j] = results[j].localBests[i];
      localWorsts[j] = results[j].localWorsts[i];
    }

    auto globalBestsVector = builder.CreateVector(globalBests);
    auto localBestsVector = builder.CreateVector(localBests);
    auto localWorstsVector = builder.CreateVector(localWorsts);

    tp2::results::IterationBuilder iterationBuilder(builder);
    iterationBuilder.add_globalBests(globalBestsVector);
    iterationBuilder.add_localBests(localBestsVector);
    iterationBuilder.add_localWorsts(localWorstsVector);
    iterations.push_back(iterationBuilder.Finish());
  }

  return builder.CreateVector(iterations);
}

void buildAndWriteResults_(const std::string &outputFile,
                           const std::vector<tp2::Result> &results) {
  flatbuffers::FlatBufferBuilder builder;

  auto params = buildParams_(builder);
  auto iterations = buildIterations_(builder, results);

  tp2::results::ResultsBuilder resultsBuilder(builder);
  resultsBuilder.add_params(params);
  resultsBuilder.add_iterations(iterations);
  builder.Finish(resultsBuilder.Finish());

  saveToFile_(outputFile, builder.GetBufferPointer(), builder.GetSize());
}

} // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  const auto seeds = generateSeeds_(FLAGS_seed, FLAGS_num_executions);
  const auto dataset = tp2::Dataset(FLAGS_dataset.c_str());

  std::vector<tp2::Result> results(FLAGS_num_executions);
  for (int i = 0; i < FLAGS_num_executions; ++i) {
    LOG(INFO) << "Execution " << i;
    tp2::RNG rng(seeds[i]);
    results[i] =
        aco(rng, dataset, FLAGS_num_iterations, FLAGS_num_ants, FLAGS_decay);
    LOG(INFO) << "";
  }

  float best = 0.0f;
  for (const auto &result : results) {
    best += result.globalBests[FLAGS_num_iterations - 1];
  }
  best /= (float)FLAGS_num_executions;

  LOG(INFO) << "Mean global best: " << best;

  buildAndWriteResults_(FLAGS_output_file, results);

  return 0;
}

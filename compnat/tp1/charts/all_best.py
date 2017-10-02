#!/usr/bin/env python3
#
# Copyright 2017 Renato Utsch
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import sys
import flatbuffers
import stats.results.Results
import matplotlib.pyplot as plt

# Args: input files


def result_str(result):
    params = result.Params()
    return ('numGens: {} | popSize: {} | '.format(params.NumGenerations(),
                                                  params.PopulationSize()) +
            'tourSize: {} | crossProb: {:.2f} | elitism: {}'.format(
                params.TournamentSize(),
                params.CrossoverProb(), True if params.Elitism() else False))


def plot_charts(all_results, train, sizes, numRepeated, numCrossBetter,
                numMutBetter):
    for result in all_results:
        if train:
            stats = result.TrainStats
            size = result.TrainStatsLength()
        else:
            stats = result.TestStats
            size = result.TestStatsLength()

        if numRepeated:
            y = [
                stats(i).NumRepeated().Mean() /
                result.Params().PopulationSize() for i in range(size)
            ]
        elif sizes:
            y = [stats(i).BestSize().Mean() for i in range(size)]
        elif numCrossBetter:
            y = [
                stats(i).NumCrossBetter().Mean() /
                result.Params().PopulationSize() for i in range(1, size)
            ]
        elif numMutBetter:
            y = [
                stats(i).NumMutBetter().Mean() /
                result.Params().PopulationSize() for i in range(1, size)
            ]
        else:
            y = [stats(i).BestFitness().Mean() for i in range(size)]

        if numCrossBetter or numMutBetter:
            plt.plot(range(1, size), y, label=result_str(result))
        else:
            plt.plot(range(size), y, label=result_str(result))

    plt.legend(loc='upper right')
    plt.xlabel('Generation')
    if numRepeated:
        plt.ylabel('Normalized number of repeated individuals')
    elif sizes:
        plt.ylabel('Number of elements of the best individual')
    elif numCrossBetter:
        plt.ylabel(
            'Normalized number of crossover individuals better than parents')
    elif numMutBetter:
        plt.ylabel(
            'Normalized number of mutation individuals better than parents')
    else:
        plt.ylabel('Fitness')
    #plt.title('Houses number of elements of the best individual')
    plt.show()


def main():
    assert len(sys.argv) > 1
    filenames = sys.argv[1:]
    all_results = []
    best_result = None
    for filename in filenames:
        with open(filename, 'rb') as f:
            buf = f.read()

        result = stats.results.Results.Results.GetRootAsResults(buf, 0)
        all_results.append(result)

        if best_result is None:
            best_result = result
        elif (best_result.FinalStats().BestIndividualFitness() >
              result.FinalStats().BestIndividualFitness()):
            best_result = result

    print('Best individual: {}'.format(result_str(best_result)))
    print('  Average Fitness: {} +/- {}'.format(best_result.FinalStats(
    ).BestFitness().Mean(), best_result.FinalStats().BestFitness().Stddev()))
    print('  Fitness: {}'.format(best_result.FinalStats()
                                 .BestIndividualFitness()))
    print('  Size: {}'.format(best_result.FinalStats().BestIndividualSize()))
    print('  Str: {}'.format(best_result.FinalStats().BestIndividualStr()))

    plot_charts(
        all_results,
        train=True,
        numRepeated=False,
        sizes=False,
        numCrossBetter=False,
        numMutBetter=False)


if __name__ == '__main__':
    main()

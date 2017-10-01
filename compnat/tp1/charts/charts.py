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

# Arg 1: input file


def plot_chart(stats, size):
    best_y = [stats(i).BestFitness().Mean() for i in range(1, size)]
    best_e = [stats(i).BestFitness().Stddev() for i in range(1, size)]
    avg_y = [stats(i).AvgFitness().Mean() for i in range(1, size)]
    avg_e = [stats(i).AvgFitness().Stddev() for i in range(1, size)]
    worst_y = [stats(i).WorstFitness().Mean() for i in range(1, size)]
    worst_e = [stats(i).WorstFitness().Stddev() for i in range(1, size)]

    #plt.yscale('log')
    plt.errorbar(range(1, size), best_y, best_e)
    #plt.plot(range(1, size), avg_y)
    #plt.plot(range(1, size), worst_y)
    plt.show()


def main():
    assert len(sys.argv) == 2
    filename = sys.argv[1]
    with open(filename, 'rb') as f:
        buf = f.read()

    results = stats.results.Results.Results.GetRootAsResults(buf, 0)
    print('Best individual:')
    print('  Average Fitness: {} +/- {}'.format(results.FinalStats(
    ).BestFitness().Mean(), results.FinalStats().BestFitness().Stddev()))
    print('  Fitness: {}'.format(results.FinalStats().BestIndividualFitness()))
    print('  Size: {}'.format(results.FinalStats().BestIndividualSize()))
    print('  Str: {}'.format(results.FinalStats().BestIndividualStr()))

    plot_chart(results.TrainStats, results.TrainStatsLength())


if __name__ == '__main__':
    main()

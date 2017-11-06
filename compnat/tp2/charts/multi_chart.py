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
import tp2.results.Results
import matplotlib.pyplot as plt
import numpy as np
import intra_chart

# Args: input files


def results_str(result):
    params = result.Params()
    return ('numAnts: {} | numIterations: {} | decay: {:.2f}'.format(
        params.NumAnts(), params.NumIterations(), params.Decay()))


def plot_chart(all_results):
    for results in all_results:
        iterations = results.Iterations
        size = results.IterationsLength()

        intra_chart.plot_line(
            size,
            lambda i, j: iterations(i).GlobalBests(j),
            label=results_str(results))

        intra_chart.plot_line(
            size,
            lambda i, j: iterations(i).GlobalBests(j),
            stddev=False,
            label=results_str(results) + ' | best solution')

    plt.legend(loc='upper right')
    plt.xlabel('Iteration')
    plt.ylabel('Fitness')
    plt.title('SJC3b iteration count variation')
    plt.show()


def main():
    assert len(sys.argv) > 1
    filenames = sys.argv[1:]
    all_results = []
    for filename in filenames:
        with open(filename, 'rb') as f:
            buf = f.read()

        result = tp2.results.Results.Results.GetRootAsResults(buf, 0)
        all_results.append(result)

    plot_chart(all_results)


if __name__ == '__main__':
    main()

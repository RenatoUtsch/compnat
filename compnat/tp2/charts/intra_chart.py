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

# Arg 1: input file


def plot_line(size, accessor, stddev=True, label=None):
    values = [[accessor(i, j) for j in range(0, 30)] for i in range(1, size)]

    if stddev:
        avg = np.average(values, axis=1)  # All executions
    else:
        avg = [min(values[i]) for i in range(0, len(values))]

    plt.plot(range(1, size), avg, label=label)

    # To show stddev
    if stddev:
        std = np.std(values, axis=1)
        plt.fill_between(range(1, size), avg - std, avg + std, alpha=0.2)


def plot_chart(iterations, size):
    plot_line(size, lambda i, j: iterations(i).GlobalBests(j), stddev=False)
    plot_line(size, lambda i, j: iterations(i).LocalBests(j), stddev=False)
    #plot_line(size, lambda i, j: iterations(i).LocalWorsts(j))
    plt.xlabel('Iteration')
    plt.ylabel('Fitness')
    plt.title('Global best and local best results, best')
    plt.show()


def main():
    assert len(sys.argv) == 2
    filename = sys.argv[1]
    with open(filename, 'rb') as f:
        buf = f.read()

    results = tp2.results.Results.Results.GetRootAsResults(buf, 0)
    plot_chart(results.Iterations, results.IterationsLength())


if __name__ == '__main__':
    main()

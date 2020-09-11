#!/usr/bin/env python3

import sys
from os import listdir
import matplotlib
import matplotlib.pyplot as plt 
import numpy as np 
import sys


def main():
    result_dir = sys.argv[1]
    try:
        plt_name = sys.argv[2]
    except IndexError:
        plt_name = ''
    print('Reading files from', result_dir)
    file_set = listdir(result_dir)
    accum = []
    i = 0
    for file in file_set:
        with open(result_dir + '/' + file) as f:
            j = 0
            for line in f.readlines():
                if i == 0:
                    accum.append(float(line.split()[1]))
                else:
                    accum[j] += float(line.split()[1])
                j += 1
        i += 1
    avg = []
    for val in accum:
        avg.append(val / len(file_set))
    matplotlib.use('Qt5Agg')
    plt.plot(avg, '-', color='chartreuse')
    if plt_name != '':
        plt.savefig(plt_name)
    plt.show()

if __name__ == "__main__":
    main()
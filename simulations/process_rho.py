#!/usr/bin/env python3

import sys
from os import listdir
import matplotlib
import matplotlib.pyplot as plt 
import numpy as np 
import sys
import glob


def main():
    result_dir = sys.argv[1]
    prefix = sys.argv[2]
    try:
        plt_name = sys.argv[3]
    except IndexError:
        plt_name = ''
    print('Reading files from', result_dir)
    file_set = glob.glob(result_dir + '/' + prefix + '*.txt')
    accum = []
    i = 0
    for file in file_set:
        with open(file) as f:
            j = 0
            for line in f.readlines():
                if i == 0:
                    accum.append(float(line.split()[1]))
                else:
                    accum[j] += float(line.split()[1])
                j += 1
        i += 1
    avg = []
    rho_file = open(prefix + '_avg', 'w')
    for val in accum:
        rho = val / len(file_set)
        rho_file.write(str(rho) + '\n')
        avg.append(rho)

    matplotlib.use('Qt5Agg')
    plt.plot(avg, '-', color='chartreuse')
    if plt_name != '':
        plt.savefig(plt_name)
    plt.show()

if __name__ == "__main__":
    main()
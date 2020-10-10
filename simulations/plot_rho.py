#!/usr/bin/env python3

import csv
import matplotlib
import matplotlib.pyplot as plt 
import numpy as np 
import sys

def read_file(filename):
    x = []
    y = []
    with open(filename) as f:
        reader = csv.reader(f, delimiter=' ')
        for row in reader:
            x.append(float(row[0]))
            y.append(float(row[1]))
    return x, y


def plot(x, y, plt_name = ''):
    matplotlib.use('Qt5Agg')
    # plt.plot(x, y, '-', color='chartreuse')
    plt.plot(y, '-', color='chartreuse')
    if plt_name != '':
        plt.savefig(plt_name)
    plt.show()

def main():
    x, y = read_file(sys.argv[1])
    try:
        pltfile = sys.argv[2]
        plot(x, y, pltfile)
    except IndexError:
        plot(x, y)

if __name__ == "__main__":
    main()
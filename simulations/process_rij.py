#!/usr/bin/env python3
import sys
import glob
import numpy as np

def main():
    work_dir = sys.argv[1]
    prefix = sys.argv[2]
    suffix = sys.argv[3]
    dim = int(sys.argv[4])
    print('Reading files from', work_dir)
    path = work_dir + '/' + prefix + '*' + suffix + '*'
    file_set = glob.glob(path)
    mat = np.zeros(shape=(dim, dim))
    print(file_set)
    for file in file_set:
        print(file)
        with open(file) as f:
            i = 0
            for line in f.readlines():
                j = 0
                for token in line.split():
                    mat[i][j] += float(token)
                    j += 1
                i += 1
    print(mat)
    f_out = open(suffix+'_avg.txt', 'w')
    for i in range(0, dim):
        for j in range(0, dim):
            mat[i][j] /= len(file_set)
            f_out.write("{:.2f}".format(mat[i][j]) + ' ')
        f_out.write('\n')
    f_out.close()

if __name__ == "__main__":
    main()
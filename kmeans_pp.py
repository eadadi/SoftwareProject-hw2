import sys
import numpy as np
import pandas as pd

def input():
    line = sys.argv
    if len(line) < 4 or len(line) > 5:
        raise ("Wrong number of arguments")
    k = line[1]

    if len(line) == 4:
        max_iter = "200"
    else: max_iter = line[2]
    k, max_iter = validate_int(k, max_iter)

    file_name1 = line[-1]
    file_name2 = line[-2]

    if not(k > 0 and max_iter > 0):
        raise("One or more of the outputs were negative")

    return (k, max_iter, file_name1, file_name2)

def validate_int(k, max_iter):
    if not (k.isdigit()):
        raise("k.isdigit exception")
    if not (max_iter.isdigit()):
        raise("max_iter.isdigit exception")
    return int(k), int(max_iter)

def read_file(path):
    m = []
    stop = False
    file = open(path, 'r')
    while not stop:
        try:
            line = file.readline()
            if line != '':
                vector = [float(x) for x in line.split(",")]
                m.append(vector)
            else: stop = True
        except EOFError:
            stop = True
    return m

def load_data(p1, p2):
    m1 = np.array(read_file(p1))
    m2 = np.array(read_file(p2))
    df1 = pd.DataFrame(m1, index=[m1[i][0] for i in range(len(m1))])
    df2 = pd.DataFrame(m2, index=[m2[i][0] for i in range(len(m1))])
    df1 = df1.drop(0, axis=1)
    df2 = df2.drop(0, axis=1)
    frames = [df1, df2]
    print(df1)
    print(df2)
    result = pd.concat(frames, join="inner", axis=1, )
    print(result)
    #print(df1[0])
    #print(df2)


def main():
    (k, max_iter, file_name1, file_name2) = input()
    load_data(file_name1,file_name2)


main()

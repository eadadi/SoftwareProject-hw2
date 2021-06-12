import sys
import numpy as np
import pandas as pd

'''
C Stuff
'''
import mykmeanssp as km

'''
The code:
'''


def input():
    line = sys.argv
    if len(line) < 4 or len(line) > 5:
        raise ("Wrong number of arguments")
    k = line[1]

    if len(line) == 4:
        max_iter = "200"
    else:
        max_iter = line[2]
    k, max_iter = validate_int(k, max_iter)

    file_name1 = line[-1]
    file_name2 = line[-2]

    if not (k > 0 and max_iter > 0):
        raise ("One or more of the outputs were negative")

    return (k, max_iter, file_name1, file_name2)


def validate_int(k, max_iter):
    if not (k.isdigit()):
        raise ("k.isdigit exception")
    if not (max_iter.isdigit()):
        raise ("max_iter.isdigit exception")
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
            else:
                stop = True
        except EOFError:
            stop = True
    return m


def load_data(p1, p2):
    m1 = np.array(read_file(p1))
    m2 = np.array(read_file(p2))

    df1 = pd.DataFrame(m1, index=[m1[i][0] for i in range(len(m1))])
    df2 = pd.DataFrame(m2, index=[m2[i][0] for i in range(len(m2))])

    df1 = df1.drop(0, axis=1)
    df2 = df2.drop(0, axis=1)

    result = pd.concat([df2, df1], join="inner", axis=1)
    result = result.sort_index()
    # print(result)

    # result.columns = ["1","2","3","4"]

    d = result.to_numpy()
    return d


def select_initial(datapoints, observations):
    np.random.seed(0)
    rand_index = np.random.choice(len(datapoints))
    u1 = np.copy(datapoints[rand_index])
    observations[0] = rand_index
    return u1


def build_probabilities(D):
    size = len(D)
    probabilities = [0] * size
    s = sum(D)
    for i in range(size):
        probabilities[i] = D[i] / s
    return probabilities


def distance(u, v):
    res = 0
    for i in range(len(u)):
        res += (u[i] - v[i]) ** 2
    return res


def computeDi(data, xi, Z, centroids):
    Di = distance(xi, centroids[0])
    for j in range(1, Z):
        uj = centroids[j]
        d = distance(xi, uj)
        Di = min(d, Di)
    return Di


def kmeanspp(datapoints, k):
    initial_centroids = [0] * k
    observations = [0] * k
    initial_centroids[0] = select_initial(datapoints, observations)
    size = len(datapoints)
    Z = 1
    while Z < k:
        D = [[0] for el in range(size)]
        for i in range(size):
            xi = np.copy(datapoints[i])
            D[i] = computeDi(datapoints, xi, Z, initial_centroids)
        probabilities = build_probabilities(D)
        j = np.random.choice(size, p=probabilities)
        observations[Z] = j
        initial_centroids[Z] = np.copy(datapoints[j])
        Z += 1
    return (observations, [array.tolist() for array in initial_centroids])


def arr_to_str(arr):
    # arr = [num1, num2, ...] -> [str(num1), str(num2), ...]
    lst = [str(x) for x in arr]
    res = ",".join(lst)
    return res


def main():
    (k, max_iter, file_name1, file_name2) = input()
    data = load_data(file_name1, file_name2)
    data_len = len(data)
    vector_len = len(data[0])
    (initial_indexes, initial_vectors) = kmeanspp(data, k)

    result = km.fit(k, max_iter, data_len, vector_len, initial_vectors, data.tolist())
    round_result = np.round(result, 4)

    #print(initial_indexes)
    #print(initial_indexes)
    #print(round_result)

    print(arr_to_str(initial_indexes))
    for i in range(len(result)):
        if i == len(result)-1:
            print(arr_to_str(round_result[i]), end = "")
        else:
            print(arr_to_str(round_result[i]))

main()

import sys
import numpy as np
import pandas as pd

'''
C Stuff
'''
import mykmeanssp as km
km.fit(32,85,[4.4,9.2,1.1],[2.4,3.2,4.9])


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

    result = pd.concat([df1, df2], join="inner", axis=1, )

    result.columns = ["1","2","3","4"]

    d = result.to_numpy()
    return d

def select_initial(datapoints):
    np.random.seed(0)
    rand_index = np.random.choice(len(datapoints))
    u1 = np.copy(datapoints[rand_index])
    return u1

def build_probabilities(D):
    size = len(D)
    probabilities = [0]*size
    s = sum(D)
    for i in range(size):
        probabilities[i] = D[i]/s
    return probabilities

def distance(u, v):
    res = 0
    for i in range(len(u)):
        res += (u[i] - v[i]) ** 2
    return res

def buildDi (data, u, Z, centroids):
    res = distance(u, centroids[0])
    for i in range(1,Z):
        d = distance(u, centroids[i])
        res = min(d, res)
    return res

def kmeanspp (datapoints, k):
    initial_centroids = [0]*k
    initial_centroids[0] = select_initial(datapoints)
    Z = 1
    while Z<k:
        D = [0]*len(datapoints)
        for i in range(len(datapoints)):
            xi = np.copy(datapoints[i])
            D[i] = buildDi(datapoints, xi, Z, initial_centroids)
        probabilities = build_probabilities(D)
        j = np.random.choice(len(datapoints),p=probabilities)
        initial_centroids[Z] = np.copy(datapoints[j])
        Z+=1
    return initial_centroids

def main():
    (k, max_iter, file_name1, file_name2) = input()
    data = load_data(file_name1,file_name2)
    #print(kmeanspp(data, k))


main()

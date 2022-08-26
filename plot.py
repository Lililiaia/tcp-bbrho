from matplotlib import pyplot as plt
import os


dir = "./results/"
cwndTracesPath="cwndTraces/n0.dat"
configPath=""



with open(dir+cwndTracesPath) as f:
    s = f.read()
    lines = s.split('\n')
    x = []
    y = []
    for line in lines[:]:
        if not line:
            continue
        a, b = line.split()
        x.append(float(a))
        y.append(float(b))

import numpy as np

xx = np.array(x)
yy = np.array(y)

plt.figure(dpi=256)

plt.plot(
    xx, yy,
    '-'
)

plt.scatter(
    xx, yy, s=0.1
)

plt.xlabel(r'time(s)')
plt.ylabel(r'Congestion window')
plt.savefig(dir+'cwnd.png')
import sys
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--fileName", help="file name", default="cwnd.csv")
parser.add_argument("--plotName", help="plot name", default="cwnd.png")
parser.add_argument("--timestep", help="timestep resolution in seconds (default 0.1)")
# Optional timestep argument
parser.add_argument("--title", help="title string", default="LTE handover TCP CWND")
args = parser.parse_args()

time=[]
value=[]
timestep = 0.0

if args.timestep is not None:
    timestep = float(args.timestep)

fd = open(args.fileName,'r')
next(fd)

for line in fd:
    l = line.strip().split(",")
    if line == "":
        continue
    time.append(float(l[0].strip()))
    value.append(float(l[1].strip()))
fd.close()


if len(time) == 0:
    print("No data points found, exiting ... ")
    sys.exit(1)

plt.scatter(time, value, marker='.', label='cell 1', color='red')
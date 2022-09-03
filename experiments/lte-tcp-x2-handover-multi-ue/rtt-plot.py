import sys
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--fileName", help="file name", default="rtt.csv")
parser.add_argument("--plotName", help="plot name", default="rtt.png")
parser.add_argument("--timestep", help="timestep resolution in seconds (default 0.1)")
# Optional timestep argument
parser.add_argument("--title", help="title string", default="LTE handover TCP RTT")
args = parser.parse_args()

times=[]
value=[]
current_time = 0

if args.timestep is not None:
    timestep = float(args.timestep)

fd = open(args.fileName,'r')
next(fd)

for line in fd:
    l = line.strip().split(",")
    if line == "":
        continue
    timestamp = float(l[0].strip())
    times.append(timestamp)
    value.append(int(l[1].strip()))
    
fd.close()


if len(times) == 0:
    print("No data points found, exiting ... ")
    sys.exit(1)

plt.plot(times, value)
plt.xlabel('Time (s)')
plt.ylabel('RTT(ms)')
plt.ylim([-1,max(value)*1.5])
plt.title(args.title)
plotname = args.plotName
plt.savefig(plotname, format='png')
plt.close()

sys.exit (0)

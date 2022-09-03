import sys
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
import argparse

parser = argparse.ArgumentParser()
# Positional arguments
parser.add_argument("--fileName", help="file name", default="tcp-receive.csv")
parser.add_argument("--plotName", help="plot name", default="tcp-rx-throughput.png")
# Optional timestep argument
parser.add_argument("--timestep", help="timestep resolution in seconds (default 0.1)")
parser.add_argument("--title", help="title string", default="Lte Handover TCP Receiver throughput")
args = parser.parse_args()

timestep = 0.1

if args.timestep is not None:
    timestep = float(args.timestep)

times=[]
bits_per_timestep=[]
fd = open(args.fileName, 'r')
current_time = 0
current_bits = 0 

next(fd)
for line in fd:
    l = line.strip().split(",")
    if line == "":
        continue
    timestamp = float(l[0].strip())
    if (timestamp < (current_time + timestep)):
        current_bits = current_bits + int(l[1].strip())*8
    else:
        times.append(current_time + timestep)
        bits_per_timestep.append(current_bits)
        current_time = current_time + timestep
        while (current_time + timestep <= timestamp):
            times.append(current_time + timestep)
            bits_per_timestep.append(0)
            current_time = current_time + timestep
        current_bits = int(l[1].strip())*8

# finish last sample
times.append(current_time + timestep)
bits_per_timestep.append(current_bits)
fd.close()

if len(times) == 0:
    print("No data points found, exiting...")
    sys.exit(1)

# Convert observed bits per timestep to Mb/s and plot
rate_in_mbps = [float(x/timestep)/1e6 for x in bits_per_timestep]
plt.plot(times, rate_in_mbps)
plt.xlabel('Time (s)')
plt.ylabel('Rate (Mb/s)')
plt.ylim([0,max(rate_in_mbps)*1.5])
plt.title(args.title)
#plt.show()
plotname = args.plotName
plt.savefig(plotname, format='png')
plt.close()

sys.exit (0)
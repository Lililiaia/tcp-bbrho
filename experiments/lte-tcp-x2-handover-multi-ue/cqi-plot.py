import sys
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--fileName", help="file name", default="cqi.csv")
parser.add_argument("--plotName", help="plot name", default="cqi.png")
parser.add_argument("--title", help="title string", default="LTE handover CQI")
args = parser.parse_args()

times1=[]
times2=[]
values1=[]
values2=[]
fd = open(args.fileName, 'r')
for line in fd:
    l = line.strip().split(",")
    if line=="":
        continue
    if l[1].strip() == "3":
        times1.append(float(l[0]))
        values1.append(int(l[3]))
    elif l[1].strip() == "4":
        times2.append(float(l[0]))
        values2.append(int(l[3]))
fd.close()

if len(times1) == 0:
    print("No data points found, exiting ....")
    sys.exit(1)

plt.scatter(times1, values1, marker='.', label='cell 1', color='red')
if len(times2) != 0:
    plt.scatter(times2, values2, marker='.', label='cell 2', color='blue')
plt.xlabel('Time (s)')
plt.ylabel('CQI')
plt.ylim([0,16])
plt.title(args.title)
#plt.show()
plotname = args.plotName
plt.savefig(plotname, format='png')
plt.close()

sys.exit (0)
    


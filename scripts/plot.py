# !pip install brewer2mpl
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import csv
import os


def draw_csv_file(path):
    basename=os.path.basename(path).split(".")[0]
    _dir=os.path.dirname(path)+'/'
    plt.figure(figsize=(16,10),dpi=80)
    x=[]
    y=[]
    with open(path,encoding='utf-8') as f:
        reader=csv.reader(f)
        header=next(reader)
        for row in reader:
            x.append(float(row[0]))
            y.append(float(row[1]))
    
    fig,ax=plt.subplots()
    ax.plot(x,y)
    ax.set(xlabel=header[0],ylabel=header[1],title=header[2])
    ax.grid()
    fig.savefig(_dir+basename)
        
    

    


def draw_trace_file(path):
    print(path)

def process_file(path):
    assert(os.path.isdir(path))
    print("processing "+path)
    _files=os.listdir(path)
    for _file in _files:
        extension=_file.split(".")[1]
        if extension=="csv":
            draw_csv_file(path+"/"+_file)
        

dir="results/"

if not os.path.exists(dir):
    exit(1)
if not os.path.isdir(dir):
    exit(1)


files = os.listdir(dir)
files.sort(reverse=True)

num_file=0
for file in files:
    num_file+=1

if num_file!=0 and os.path.isdir(dir+files[0]):
    process_file(dir+files[0])
else:
    print("empty directory")
    exit(1)



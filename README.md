# Introduction
This a human-friendly configurable and automated 5G NSA network simulation subroutine running on [mmWave-ns3](https://github.com/nyuwireless-unipd/ns3-mmwave).
There are three ranked motivation to write such a program:
- Used as the testbed for TCP-CNHA algorithm.
- A human-friendly configuration system can do speed up simulation enviornment deployment.
- The one-click-to-run program design can reduce unnecessary and error-prone manual operation and make less effort to be reproduced by others.

# A cellular network handover-aware TCP congestion control algorithm (TCP-CNHA)
 wait to introduce

# Simulation Experiments
Although TCP-CNHA looks like a heuristic algorithm at present, we will still spare no effort to dig its potential value in other reliable transmission scenarios with cellular network handover-like overhead. Whether to compare TCP-CNHA with other existing congestion algorithms from different metrics or to show TCP-CNHA's performance in other network topologies, we need lots of simulation experiments to support or prove our theories and guesses. However, the native simulation interface provided by ns-3 is still too tedious that we have to draw figures and prepare inputs mutually after each simulation iteration. Obviously, such an experiment mode is not efficient enough. That is why we spent some time writing this program —— it can alleviate our subsequent workload. As for the remaining parts, we will introduce our several experiment designs.
## 

# How to use
- `Step 1`: Clone this repository to local: `git clone https://github.com/Lililiaia/tcp-wsa.git`
- `Step 2`: deploy simulation environment: `./install.sh`
- `Step 3`: after execute the install shell script, a execute file named as `waf` will be created, then run `./waf`

# Introduction
This a human-friendly configurable and automated 5G NSA network simulation subroutine running on [mmWave-ns3](https://github.com/nyuwireless-unipd/ns3-mmwave).
There are three ranked motivation to write such a program:
- Used as the testbed for TCP-WSA algorithm.
- A human-friendly configuration system can do speed up simulation enviornment deployment.
- The one-click-to-run program design can reduce unnecessary and error-prone manual operation and make less effort to be reproduced by others.

# A wireless handover-aware TCP congestion control algorithm (TCP-WSA)

# How to use
- `Step 1`: Clone mmWave-ns3 repository: `git clone https://github.com/nyuwireless-unipd/ns3-mmwave`
- `Step 2`: Create a new directory <ROOT_DIR> at mmWave-ns3/examples/
- `Step 3`: Copy all files from this repository to <ROOT_DIR>
- `Step 4`: Back to mmWave-ns3 directory and run command: `./waf --run "lte examples/nsa/config.cfg"`

#!/bin/bash 

if [ -f installed.ok ]; then
    echo "nothing to do"
    exit
fi

# check git tool
if ! command -v git &> /dev/null; then
    echo "please install git"
fi

# download mmwave https://github.com/nyuwireless-unipd/ns3-mmwave.git 
MMWAVE_REPOS_URL="--branch v5.0 https://github.com/nyuwireless-unipd/ns3-mmwave.git"
EXAMPLES_DIR="ns3-mmwave/examples/tcp-chna/"

if [ ! -d "ns3-mmwave/" ];then
    echo "Downloading ns-mmwave(5.0)..."
    git clone $MMWAVE_REPOS_URL || exit "$?"
fi

# copy examples into ns3-mmwave/examples/tcp-cnha/
if [ ! -d $EXAMPLES_DIR ];then
    echo "Create tcp-chna directory.."
    mkdir ${EXAMPLES_DIR} || exit "$?"
fi

cp examples/* $EXAMPLES_DIR



# configurating waf and put a patch
cd ns3-mmwave/
git am ../0001-dpdk.patch || exit "can not put dpdk patch into mmwave"
./waf configure --disable-python --enable-examples && ./waf build || exit "$?"
cd ../

# create a execute file named as waf
if [ ! -f "waf" ]; then
echo "#!/bin/bash
cp examples/* ${EXAMPLES_DIR}
cd ns3-mmwave
./waf \$*" > waf
chmod +x waf
fi

touch installed.ok
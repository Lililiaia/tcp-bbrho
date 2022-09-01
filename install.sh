#!/bin/bash 
PKT_M_TOOL=""
RED='\033[0;31m'
NC='\033[0m'

# check os type
if [[ "$OSTYPE" == "linux-gnu"* ]];then
    release_msg=$(sudo cat /etc/*release | grep "CentOS")
    if [[ $release_msg != "" ]];then
        PKT_M_TOOL="sudo yum"
    else
        PKT_M_TOOL="sudo apt-get"
    fi
else 
    echo -e "${RED}Error${NC}: Only support Linux platform"
    exit
fi

if [ -f installed.ok ]; then
    echo "Nothing to do"
    exit
fi

# check python3
if ! command -v python3 &> /dev/null; then
    echo "Install python3 ..... ..... "
    $PKT_M_TOOL -y install python3 || exit -e "${RED}Error${NC}: $?"
    exit
fi

# check python3 pip
if ! command -v pip3 &> /dev/null; then
    echo "please install pip3"
    $PKT_M_TOOL -y install python3-pip || exit -e "${RED}Error${NC}: $?"
    exit
fi

# install python module
pip3 install numpy matplotlib || exit -e "${RED}Error${NC}: $?"

# download mmwave https://github.com/nyuwireless-unipd/ns3-mmwave.git 
MMWAVE_REPOS_URL="--branch v5.0 https://github.com/nyuwireless-unipd/ns3-mmwave.git"
EXAMPLES_DIR="ns3-mmwave/examples/tcp-chna/"

if [ ! -d "ns3-mmwave/" ];then
    echo "Downloading ns-mmwave(5.0)..."
    git clone $MMWAVE_REPOS_URL || exit -e "${RED}Error${NC}: $?"
fi


# configurating waf and put a patch
cd ns3-mmwave/
git am ../0001-dpdk.patch || exit -e "${RED}Error${NC}: can not put dpdk patch into mmwave"
git am ../0002-cqi_report.patch || exit -e "${RED}Error${NC}: can not put cqi report patch into mmwave"
cd ../
# copy examples into ns3-mmwave/examples/tcp-cnha/
if [ ! -d $EXAMPLES_DIR ];then
    echo "Create tcp-chna directory.."
    mkdir ${EXAMPLES_DIR} || exit -e "${RED}Error${NC}: $?"
fi

cp -r examples/* $EXAMPLES_DIR

cd ns3-mmwave/
./waf configure --disable-python --enable-examples -d optimized && ./waf build || exit "$?"
cd ../

touch installed.ok
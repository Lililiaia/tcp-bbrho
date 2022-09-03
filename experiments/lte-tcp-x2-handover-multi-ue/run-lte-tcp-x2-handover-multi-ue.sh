DEBUG=""
WORKSPACE=$OUTPUT_PATH/lte-tcp-x2-handover-multi-ue/`date +%Y%m%d-%H%M%S`
export CONFIG_FILE=$CONFIG_FILE_PATH/lte-tcp-x2-handover-multi-ue.conf


parse_args(){
    case $1 in
        "debug")
            DEBUG="--gdb"
            ;;
        *)
            ;;
    esac
}

parse_args $RUN_ARGS

cp $ROOT_DIR/examples/* $ROOT_DIR/ns3-mmwave/examples/tcp-chna/ -r
cd $ROOT_DIR/ns3-mmwave/

if [[ ! -d $WORKSPACE ]];then
    mkdir -p $WORKSPACE
fi

# create subdir 
python3 ./waf --run lte-tcp-x2-handover-multi-ue --cwd=$WORKSPACE $DEBUG || exit "$?"


# move file to subsir
mkdir -p $WORKSPACE/stats
mkdir -p $WORKSPACE/traces
mkdir -p $WORKSPACE/images

mv $WORKSPACE/*.csv $WORKSPACE/stats/
mv $WORKSPACE/*.txt $WORKSPACE/traces/
mv $WORKSPACE/*.pcap $WORKSPACE/traces/


cp $CONFIG_FILE $WORKSPACE/

# plot
echo "ploting LTE handover cqi ..."
python3 $ROOT_DIR/experiments/lte-tcp-x2-handover-multi-ue/cqi-plot.py --fileName=$WORKSPACE/stats/cqi.csv --plotName=$WORKSPACE/images/cqi.png || echo "failed to plot LTE handover cqi"
echo "ploting LTE handover MCS ..."
python3 $ROOT_DIR/experiments/lte-tcp-x2-handover-multi-ue/mcs-plot.py --fileName=$WORKSPACE/traces/DlMacStats.txt --plotName=$WORKSPACE/images/mcs.png || echo "failed to plot LTE handover MCS"
echo "ploting LTE handover RSRP ..."
python3 $ROOT_DIR/experiments/lte-tcp-x2-handover-multi-ue/rsrp-plot.py --fileName=$WORKSPACE/stats/ue-measurements.csv --plotName=$WORKSPACE/images/rsrp.png || echo "failed to plot LTE handover RSRP"
echo "ploting LTE handover RSRQ ..."
python3 $ROOT_DIR/experiments/lte-tcp-x2-handover-multi-ue/rsrq-plot.py --fileName=$WORKSPACE/stats/ue-measurements.csv --plotName=$WORKSPACE/images/rsrq.png || echo "failed to plot LTE handover RSRQ"
echo "ploting TCP Receiver throughput ..."
python3 $ROOT_DIR/experiments/lte-tcp-x2-handover-multi-ue/tcp-throughput-plot.py --fileName=$WORKSPACE/stats/tcp-receive.csv --plotName=$WORKSPACE/images/tcp-rx-throughput.png --title="Lte Handover TCP Receiver throughput"|| echo "failed to plot TCP Receiver throughput"
echo "ploting TCP Sender throughput ..."
python3 $ROOT_DIR/experiments/lte-tcp-x2-handover-multi-ue/tcp-throughput-plot.py --fileName=$WORKSPACE/stats/tcp-send.csv --plotName=$WORKSPACE/images/tcp-tx-throughput.png --title="Lte Handover TCP Sender throughput"|| echo "failed to plot TCP Sender throughput"
echo "ploting TCP CongestionWindow ..."
python3 $ROOT_DIR/experiments/lte-tcp-x2-handover-multi-ue/cwnd-plot.py --fileName=$WORKSPACE/stats/cwnd.csv --plotName=$WORKSPACE/images/cwnd.png --title="Lte Handover TCP CWND"|| echo "failed to plot TCP CongestionWindow"
echo "ploting TCP RTT ..."
python3 $ROOT_DIR/experiments/lte-tcp-x2-handover-multi-ue/rtt-plot.py --fileName=$WORKSPACE/stats/rtt.csv --plotName=$WORKSPACE/images/rtt.png --title="Lte Handover TCP RTT"|| echo "failed to plot LTE handover TCP RTT"




# make all files readonly
chmod a=r $WORKSPACE/stats/*
chmod a=r $WORKSPACE/traces/*
chmod a=r $WORKSPACE/images/*
chmod a=r $WORKSPACE/*.conf
#include "mmwave-tcp-x2-handover.h"




int
main(int argv,char**argc)
{
    init_config();

    NodeContainer lteNodes;
    NodeContainer mmNodes;
    NodeContainer mcUeNodes;
    NodeContainer remoteNodes;
    lteNodes.Create(1);
    mmNodes.Create(4);
    mcUeNodes.Create(1);
    remoteNodes.Create(1);
    Config::SetDefault ("ns3::MmWaveHelper::UseIdealRrc", BooleanValue(useIdealRrc));
    Config::SetDefault ("ns3::MmWaveHelper::RlcAmEnabled", BooleanValue(rlcAmEnabled));
    Config::SetDefault ("ns3::MmWaveHelper::UsePdschForCqiGeneration", BooleanValue(usePdschForCqiGeneration));

    Config::SetDefault ("ns3::MmWavePointToPointEpcHelper::X2LinkDelay", TimeValue (x2Latency));
    Config::SetDefault ("ns3::MmWavePointToPointEpcHelper::S1apLinkDelay", TimeValue (mmeLatency));

    Config::SetDefault ("ns3::LteEnbRrc::FixedTttValue", UintegerValue(fixedTttValue));
    Config::SetDefault ("ns3::LteEnbRrc::MinDynTttValue", UintegerValue(minDynTttValue));
    Config::SetDefault ("ns3::LteEnbRrc::MaxDynTttValue", UintegerValue(maxDynTttValue));
    Config::SetDefault ("ns3::LteEnbRrc::MinDiffValue", DoubleValue(minDiffTttValue));
    Config::SetDefault ("ns3::LteEnbRrc::MaxDiffValue", DoubleValue(maxDiffValue));
    Config::SetDefault ("ns3::LteEnbRrc::CrtPeriod", IntegerValue(crtPeriod));
    Config::SetDefault ("ns3::LteEnbRrc::OutageThreshold", DoubleValue(outageThreshold));

    //MmWaveEnbPhy
    Config::SetDefault ("ns3::MmWaveEnbPhy::UpdateSinrEstimatePeriod", IntegerValue (updateSinrEstimatePeriod));
    Config::SetDefault ("ns3::MmWaveEnbPhy::UpdateUeSinrEstimatePeriod", DoubleValue (updateUeSinrEstimatePeriod));
    Config::SetDefault ("ns3::MmWaveEnbPhy::NoiseAndFilter", BooleanValue (noiseAndFilter));

    //ThreeGppChannelModel
    
    //TCP
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::" + tcpType));
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1448));
    

    switch (handoverMode)
    {
    case 1:
      Config::SetDefault ("ns3::LteEnbRrc::SecondaryCellHandoverMode", EnumValue (LteEnbRrc::THRESHOLD));
      break;
    case 2:
      Config::SetDefault ("ns3::LteEnbRrc::SecondaryCellHandoverMode", EnumValue (LteEnbRrc::FIXED_TTT));
      break;
    case 3:
      Config::SetDefault ("ns3::LteEnbRrc::SecondaryCellHandoverMode", EnumValue (LteEnbRrc::DYNAMIC_TTT));
      break;
    default:
      NS_ABORT_MSG("Not supported handover mode :"<<handoverMode);
      break;
    }

    // PathLossModel
    //ThreeGppUmaPropagationLossModel


    Ptr<MmWaveHelper>  mmWaveHelper=CreateObject<MmWaveHelper>();
    mmWaveHelper->SetSnrTest(snrTest);
    
    Ptr<MmWavePointToPointEpcHelper> epcHelper=CreateObject<MmWavePointToPointEpcHelper>();
    mmWaveHelper->SetEpcHelper(epcHelper);

    /* Ptr<Node> leftLteNode=lteNodes.Get(0);
    Ptr<Node> rightLteNode=lteNodes.Get(1);
    Ptr<Node> leftMNode=mmNodes.Get(0);
    Ptr<Node> rightMNode=mmNodes.Get(1);
    Ptr<Node> mcUeNode=mcUeNodes.Get(0); */
    
    Ptr<Node> pgw=epcHelper->GetPgwNode();
    Ptr<Node> remoteNode=remoteNodes.Get(0);

    InternetStackHelper internet;
    internet.Install(remoteNode);

    PointToPointHelper p2ph;
    p2ph.SetChannelAttribute("Delay",TimeValue(internetDelay));
    p2ph.SetDeviceAttribute("Mtu",UintegerValue(internetMtu));
    p2ph.SetDeviceAttribute("DataRate",DataRateValue(internetDataRate));
    NetDeviceContainer internetDevs=p2ph.Install(pgw,remoteNode);
    Ptr<NetDevice> sDev=internetDevs.Get(1);

    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0","255.0.0.0");
    ipv4h.Assign(internetDevs);

    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting(remoteNode->GetObject<Ipv4>());
    // interface 0 is localhost, 1 is the p2p device
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);

    // Install Mobiltiy in eNBs
    Ptr<ListPositionAllocator> enbPositionAlloc=CreateObject<ListPositionAllocator>();
    enbPositionAlloc->Add(leftLtePos);
    enbPositionAlloc->Add(rightLtePos);
    Ptr<ListPositionAllocator> mmEnbPositionAlloc=CreateObject<ListPositionAllocator>();
    mmEnbPositionAlloc->Add(MNbPos1);
    mmEnbPositionAlloc->Add(MNbPos2);
    mmEnbPositionAlloc->Add(MNbPos3);
    mmEnbPositionAlloc->Add(MNbPos4);
    Ptr<ListPositionAllocator> uePositionAlloc=CreateObject<ListPositionAllocator>();
    uePositionAlloc->Add(startPos);

    MobilityHelper lteEnbMobility;
    lteEnbMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    lteEnbMobility.SetPositionAllocator(enbPositionAlloc);
    lteEnbMobility.Install(lteNodes);

    MobilityHelper mmEnbMobility;
    mmEnbMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mmEnbMobility.SetPositionAllocator(mmEnbPositionAlloc);
    mmEnbMobility.Install(mmNodes);

    MobilityHelper ueMobility;
    ueMobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    ueMobility.SetPositionAllocator(uePositionAlloc);
    ueMobility.Install(mcUeNodes.Get(0));
    mcUeNodes.Get(0)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(speed);

    // Install lte enb and mmwave device
    mmWaveHelper->SetLteHandoverAlgorithmType("ns3::"+lteHandoverAlgorithm);
    NetDeviceContainer lteDevs=mmWaveHelper->InstallLteEnbDevice(lteNodes); //LteEnbNetDevice
    NetDeviceContainer mmwaveDevs=mmWaveHelper->InstallEnbDevice(mmNodes); //MmWaveEnbNetDevice

    
    internet.Install(mcUeNodes);
    NetDeviceContainer mcUeDevs=mmWaveHelper->InstallMcUeDevice(mcUeNodes);//McUeNetDevice

    // 
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (mcUeDevs));

    Ptr<Node> mcUeNode = mcUeNodes.Get (0);
    Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (mcUeNode->GetObject<Ipv4> ());
    ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);

    //Add X2 interface
    mmWaveHelper->AddX2Interface (lteNodes, mmNodes);


    //attach 
    mmWaveHelper->AttachToClosestEnb(mcUeDevs,mmwaveDevs,lteDevs);


    //Install APP
    MyBulkSendHelper ftpServer("ns3::TcpSocketFactory", Address());
    AddressValue remoteAddress(InetSocketAddress(ueIpIface.GetAddress(0), 5000));
    ftpServer.SetAttribute("Remote", remoteAddress);
    ftpServer.SetAttribute("MaxBytes", UintegerValue(20000000000));
    NS_LOG_UNCOND("setting up TCP flow from remote host to UE");
    ApplicationContainer sourceApp = ftpServer.Install(remoteNode);
    sourceApp.Start(Seconds(0.9));
    sourceApp.Stop(simTime+Seconds(1));

    Address sinkLocalAddress(InetSocketAddress(ueIpIface.GetAddress(0), 5000));
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", sinkLocalAddress);
    ApplicationContainer sinkApp = sinkHelper.Install(mcUeNodes.Get(0));
    sinkApp.Start(Seconds(1));
    sinkApp.Stop(simTime+Seconds(1));

    /* Ptr<EpcTft> tft = Create<EpcTft>();
    EpcTft::PacketFilter dlpf;
    dlpf.localPortStart = 5000;
    dlpf.localPortEnd = 5000;
    tft->Add(dlpf);

    // Qos
    int qci = str_to_Qci("NGBR_VIDEO_TCP_DEFAULT");
    if (qci == -1)
        NS_ABORT_MSG("Invalid qos class indicator : NGBR_VIDEO_TCP_DEFAULT");
    EpsBearer bearer((EpsBearer::Qci)qci); */

    //mmWaveHelper->EnableTraces();


    // Trace LTE eNB for RRC connection establishment and handover notification
    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
                    MakeCallback(&NotifyConnectionEstablishedEnb));
    Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished",
                    MakeCallback(&NotifyConnectionEstablishedUe));
    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverStart",
                    MakeCallback(&NotifyHandoverStartEnb));
    Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart",
                    MakeCallback(&NotifyHandoverStartUe));
    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
                    MakeCallback(&NotifyHandoverEndOkEnb));
    Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/HandoverEndOk",
                    MakeCallback(&NotifyHandoverEndOkUe));

    Config::Connect("/NodeList/"+std::to_string(mcUeNodes.Get(0)->GetId())+"/ApplicationList/*/$ns3::PacketSink/Rx",
                    MakeCallback(&NotifyPacketSinkRx));
    
    Simulator::Schedule(Seconds(0), &ConnectTcpTrace,remoteNode->GetId());
    Simulator::Schedule(Seconds(10.0), &ReportProgress, Seconds(10.0));
    Simulator::Stop(simTime);
    Simulator::Run();
    Simulator::Destroy();

    // g_ueMeasurements.close();
    g_packetSinkRx.close();
    g_bulkSendTx.close();
    g_cwndTrace.close();
    // g_cqiTrace.close();
    g_tcpCongStateTrace.close();
    // g_positionTrace.close();
    g_rttTrace.close();
    return 0;
}


void init_config(){
    std::string _internetDelay="0ms";
    std::string _internetDataRate="10Gb/s";
    std::string _leftLtePos="0,100";
    std::string _rightLtePos="0,100";
    std::string _MNbPos1="0,50";
    std::string _MNbPos2="0,50";
    std::string _MNbPos3="0,50";
    std::string _MNbPos4="0,50";
    std::string _startPos="0,0";
    std::string _endPos="0,1000";
    std::string _x2Latency="0us";
    std::string _mmeLatency="0us";
    double _speed=10.0;
    CommandLine cmd;

    cmd.AddValue("internetDelay","The delay of internet transmission",_internetDelay);
    cmd.AddValue("internetDataRate","The datarate of internet transmission",_internetDataRate);
    cmd.AddValue("internetMtu","The mtu of internet",internetMtu);
    cmd.AddValue("leftLtePos","the left lte enb position",_leftLtePos);
    cmd.AddValue("rightLtePos","the left lte enb position",_rightLtePos);
    cmd.AddValue("MNbPos1","the left lte enb position",_MNbPos1);
    cmd.AddValue("MNbPos2","the left lte enb position",_MNbPos2);
    cmd.AddValue("MNbPos3","the left lte enb position",_MNbPos3);
    cmd.AddValue("MNbPos4","the left lte enb position",_MNbPos4);
    cmd.AddValue("startPos","the left lte enb position",_startPos);
    cmd.AddValue("endPos","the left lte enb position",_endPos);
    cmd.AddValue("speed","the left lte enb position",_speed);
    cmd.AddValue("lteHandoverAlgorithm","The type of handover algorithm to be used for LTE eNBs",lteHandoverAlgorithm);
    cmd.AddValue("useIdealRrc","Use Ideal or Real RRC",useIdealRrc);
    cmd.AddValue("rlcAmEnabled","Enable RLC acknowledged Mode",rlcAmEnabled);
    cmd.AddValue("snrTest","Enable snr Test to generate data cqi report",snrTest);
    cmd.AddValue("usePdschForCqiGeneration","If true, DL-CQI will be calculated from PDCCH as signal",usePdschForCqiGeneration);
    cmd.AddValue("x2Latency","Latency on X2 interface",_x2Latency);
    cmd.AddValue("mmeLatency","Latency on MME interface",_mmeLatency);
    cmd.AddValue("handoverMode","Select the secondary cell handover mode used in 5G and LTE handover",handoverMode);
    cmd.AddValue("fixedTttValue","The value of TTT in case of fixed TTT handover(in ms)",fixedTttValue);
    cmd.AddValue("minDynTttValue","The minimum value of TTT in case of dynamic TTT handover(in ms)",minDynTttValue);
    cmd.AddValue("maxDynTttValue","The maximum value of TTT in case of dynamic TTT handover(in ms)",maxDynTttValue);
    cmd.AddValue("minDiffTttValue","The maximum value of TTT in case of dynamic TTT handover(in ms)",minDiffTttValue);
    cmd.AddValue("maxDiffValue","The maximum value of TTT in case of dynamic TTT handover(in ms)",maxDiffValue);
    cmd.AddValue("crtPeriod","The maximum value of TTT in case of dynamic TTT handover(in ms)",crtPeriod);
    cmd.AddValue("outageThreshold","SNR threshold for outage events [dB]",outageThreshold);
    cmd.AddValue("mmWaveEnb1TxPower","MmWave eNB's Transmission power in dBm",mmWaveEnb1TxPower);
    cmd.AddValue("mmWaveEnb2TxPower","MmWave eNB's Transmission power in dBm",mmWaveEnb2TxPower);
    cmd.AddValue("mmWaveEnb3TxPower","MmWave eNB's Transmission power in dBm",mmWaveEnb3TxPower);
    cmd.AddValue("mmWaveEnb4TxPower","MmWave eNB's Transmission power in dBm",mmWaveEnb4TxPower);
    cmd.AddValue("updateSinrEstimatePeriod","Period(ms) of update of SINR estimate of all the UE",updateSinrEstimatePeriod);
    cmd.AddValue("updateUeSinrEstimatePeriod","Period(ms) of reporting of SINR estimate of all the UE",updateUeSinrEstimatePeriod);
    cmd.AddValue("noiseAndFilter","If true, use noisy SINR samples, filtered If false, just use the SINR measure",noiseAndFilter);
    cmd.AddValue("tcpType","The type of Tcp",tcpType);

    cmd.Parse(parse_config_file());

    internetDataRate=DataRate(_internetDataRate);
    internetDelay=Time(_internetDelay);

    leftLtePos=to_vec(_leftLtePos);
    rightLtePos=to_vec(_rightLtePos);
    MNbPos1=to_vec(_MNbPos1);
    MNbPos2=to_vec(_MNbPos2);
    MNbPos3=to_vec(_MNbPos3);
    MNbPos4=to_vec(_MNbPos4);
    startPos=to_vec(_startPos);
    endPos=to_vec(_endPos);
    speed=endPos-startPos;
    x2Latency=Time(_x2Latency);
    mmeLatency=Time(_mmeLatency);
    double distance=speed.GetLength();
    NS_ASSERT_MSG(distance>0.0,"UE movement distance must be large than 0m");
    double _simTime=distance/_speed;
    NS_ASSERT_MSG(_simTime>=10.0,"simTime must be large than 10s");
    simTime=Seconds(std::ceil(_simTime));
    speed.x=speed.x/distance*_speed;
    speed.y=speed.y/distance*_speed;
    speed.z=speed.z/distance*_speed;

    
    g_packetSinkRx.open("tcp-receive.csv", std::ofstream::out);
    g_packetSinkRx << "\"time\",\"bytesRx\"" << std::endl;
    g_bulkSendTx.open("tcp-send.csv", std::ofstream::out);
    g_bulkSendTx << "\"time\",\"bytesTx\"" << std::endl;
    g_tcpCongStateTrace.open("tcp-state.csv", std::ofstream::out);
    g_tcpCongStateTrace << "\"time\",\"congState\"" << std::endl;
    g_cwndTrace.open("cwnd.csv", std::ofstream::out);
    g_cwndTrace << "\"time\",\"cwnd\"" << std::endl;
    g_cwndTrace << std::setw(7) << std::setprecision(3) << std::fixed << 1.0
                << "," << std::setw(5) << 10 << std::endl;
    g_rttTrace.open("rtt.csv", std::ofstream::out);
    g_rttTrace << "\"time\",\"rtt\"" << std::endl;

    

    NS_LOG_UNCOND("Total simulation time: "<<simTime.GetSeconds());

    
}

Vector to_vec(const std::string &s){
    std::vector<std::string> vec=split(s,",");
    double res[3];

    for(size_t i=0;i<3;++i){
        if(i<vec.size()){
            res[i]=std::stod(vec[i]);
        }
        else{
            res[i]=0.0;
        }
    }
    
    return Vector(res[0],res[1],res[3]);
}

std::vector<std::string> split(const std::string &s,const std::string & delimiter){
    int pos=s.find(delimiter);
    size_t start=0;
    std::vector<std::string> ret;
    while(pos!=-1){
        ret.push_back(s.substr(start,pos-start));
        start=pos+1;
        pos=s.find(delimiter,start);
    }
    if(start<s.size()){
        ret.push_back(s.substr(start));
    }
    return ret;
}


void tx_trace(ns3::Ptr<const ns3::Packet> p, const ns3::TcpHeader &header, ns3::Ptr<const ns3::TcpSocketBase> socket)
{
    g_bulkSendTx << std::setw(7) << std::setprecision(3) << std::fixed << Simulator::Now().GetSeconds()
                 << "," << std::setw(5) << p->GetSize() << std::endl;
}

void rto_trace(ns3::Time old_val, ns3::Time new_val)
{
}
void rtt_trace(ns3::Time old_val, ns3::Time new_val)
{
    static bool first=true;
    if(first){
        g_rttTrace << std::setw(7) << std::setprecision(3) << std::fixed << 1.0
                << "," << std::setw(5) << old_val.GetMilliSeconds() << std::endl;
        first=false;
    }
    g_rttTrace << std::setw(7) << std::setprecision(3) << std::fixed << Simulator::Now().GetSeconds()
                << "," << std::setw(5) << new_val.GetMilliSeconds() << std::endl;
}
void advwnd_trace(uint32_t old_val, uint32_t new_val)
{
}
void cwnd_trace(uint32_t old_val, uint32_t new_val)
{
    g_cwndTrace << std::setw(7) << std::setprecision(3) << std::fixed << Simulator::Now().GetSeconds()
                << "," << std::setw(5) << new_val / 1448 << std::endl;
}
void cwnd_inflated_trace(uint32_t old_val, uint32_t new_val)
{
}

void ConnectTcpTrace(int nodeid)
{
    Config::ConnectWithoutContext("/NodeList/"+std::to_string(nodeid)+"/$ns3::TcpL4Protocol/SocketList/0/CongState", MakeCallback(&CongStateTrace));
}

void CongStateTrace(const TcpSocketState::TcpCongState_t oldValue, const TcpSocketState::TcpCongState_t newValue)
{
    g_tcpCongStateTrace << std::setw(7) << std::setprecision(3) << std::fixed << Simulator::Now().GetSeconds() << ","
                        << std::setw(4) << TcpSocketState::TcpCongStateName[newValue] << std::endl;
}

uint32_t
ContextToNodeId (std::string context)
{
  std::string sub = context.substr (10);  // skip "/NodeList/"
  uint32_t pos = sub.find ("/Device");
  return atoi (sub.substr (0,pos).c_str ());
}

void NotifyConnectionEstablishedEnb(std::string context,
                                    uint64_t imsi,
                                    uint16_t cellid,
                                    uint16_t rnti)
{
    NS_LOG_UNCOND(
        Simulator::Now().GetSeconds() << " node "
                                      << ContextToNodeId(context)
                                      << " eNB CellId " << cellid
                                      << ": successful connection of UE with IMSI " << imsi
                                      << " RNTI " << rnti);
}

void NotifyConnectionEstablishedUe(std::string context,
                                   uint64_t imsi,
                                   uint16_t cellid,
                                   uint16_t rnti)
{
    NS_LOG_UNCOND(Simulator::Now().GetSeconds() << " node "
                                              << ContextToNodeId(context)
                                              << " UE IMSI " << imsi
                                              << ": connected to CellId " << cellid
                                              << " with RNTI " << rnti);
}

void NotifyHandoverStartUe(std::string context,
                           uint64_t imsi,
                           uint16_t cellid,
                           uint16_t rnti,
                           uint16_t targetCellId)
{
    NS_LOG_UNCOND(
        Simulator::Now().GetSeconds() << " node "
                                      << ContextToNodeId(context)
                                      << " UE IMSI " << imsi
                                      << ": previously connected to CellId " << cellid
                                      << " with RNTI " << rnti
                                      << ", doing handover to CellId " << targetCellId);
}

void NotifyHandoverEndOkUe(std::string context,
                           uint64_t imsi,
                           uint16_t cellid,
                           uint16_t rnti)
{
    NS_LOG_UNCOND(
        Simulator::Now().GetSeconds() << " node "
                                      << ContextToNodeId(context)
                                      << " UE IMSI " << imsi
                                      << ": successful handover to CellId " << cellid
                                      << " with RNTI " << rnti);
}

void NotifyHandoverStartEnb(std::string context,
                            uint64_t imsi,
                            uint16_t cellid,
                            uint16_t rnti,
                            uint16_t targetCellId)
{
    NS_LOG_UNCOND(
        Simulator::Now().GetSeconds() << " node "
                                      << ContextToNodeId(context)
                                      << " eNB CellId " << cellid
                                      << ": start handover of UE with IMSI " << imsi
                                      << " RNTI " << rnti
                                      << " to CellId " << targetCellId);
}

void NotifyHandoverEndOkEnb(std::string context,
                            uint64_t imsi,
                            uint16_t cellid,
                            uint16_t rnti)
{
    NS_LOG_UNCOND(
        Simulator::Now().GetSeconds() << " node "
                                      << ContextToNodeId(context)
                                      << " eNB CellId " << cellid
                                      << ": completed handover of UE with IMSI " << imsi
                                      << " RNTI " << rnti);
}

void NotifyPacketSinkRx(std::string context, Ptr<const Packet> packet, const Address &address)
{
    g_packetSinkRx << std::setw(7) << std::setprecision(3) << std::fixed << Simulator::Now().GetSeconds()
                   << "," << std::setw(5) << packet->GetSize() << std::endl;
}

void ReportProgress(Time reportingInterval)
{
    NS_LOG_INFO("*** Simulation time: " << std::fixed << std::setprecision(1) << Simulator::Now().GetSeconds() << "s");
    Simulator::Schedule(reportingInterval, &ReportProgress, reportingInterval);
}
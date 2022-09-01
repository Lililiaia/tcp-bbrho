#include <fstream>
#include <string>
#include <iomanip>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "../config.h"
#include "../mybulk-send-helper.h"
#include "../mybulk-send-application.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("LteTcpX2Handover");

std::ofstream g_ueMeasurements;
std::ofstream g_packetSinkRx;
std::ofstream g_bulkSendTx;
std::ofstream g_cwndTrace;
std::ofstream g_cqiTrace;
std::ofstream g_tcpCongStateTrace;
std::ofstream g_positionTrace;
std::ofstream g_rttTrace;

const uint16_t numberOfUes = 1;
const uint16_t numberOfEnbs = 2;

double x2Distance = 500.0;      // m
double yDistanceForUe = 1000.0; // m
double speed = 20;              // m/s
double enbTxPowerDbm = 46.0;
std::string handoverType = "A2A4";
std::string tcpType = "TcpBbr";
/*
    # RlcSmAlways 1
    # RlcUmAlways 2
    # RlcAmAlways 3
    # PacketErrorRateBased 4
    # MmwRlcUmAlways 5
 */
std::string rlcBearer = "RlcSmAlways";
bool verbose = false;
bool pcap = false;
bool useIdealRrc = true;

const std::string traceFilePrefix = "lte-tcp-x2-handover";
Time positionTracingInterval = Seconds(5);
std::string _positionTracingInterval = "5s";
Time reportingInterval = Seconds(10);
std::string _reportingInterval = "10s";
uint32_t ftpSize = 200000000; // 200MB
uint16_t port = 4000;
double simTime = 50.0;
std::string lteSchedulerType = "RrFfMacScheduler";
Time internetDelay = Seconds(0.010);
std::string _internetDelay = "0.010s";
int internetMtu = 1500;
DataRate internetDataRate = DataRate("100Gb/s");
std::string _internetDataRate = "100Gb/s";
std::string bearerQos = "NGBR_VIDEO_TCP_DEFAULT";
Time epochDuration = Seconds(1.0);
std::string _epochDuration = "1.0s";
int intialWindowSize = 10;
int tcpMss = 1448;
int enbAUlBandWidth=100;
int enbBUlBandWidth=100;
int enbADlBandWidth=100;
int enbBDlBandWidth=100;


void ReportProgress(Time reportingInterval);
uint32_t ContextToNodeId(std::string context);
void NotifyUdpReceived(std::string context, Ptr<const Packet> p, const Address &addr);
void NotifyConnectionEstablishedUe(std::string context,
                                   uint64_t imsi,
                                   uint16_t cellid,
                                   uint16_t rnti);
void NotifyHandoverStartUe(std::string context,
                           uint64_t imsi,
                           uint16_t cellid,
                           uint16_t rnti,
                           uint16_t targetCellId);
void NotifyHandoverEndOkUe(std::string context,
                           uint64_t imsi,
                           uint16_t cellid,
                           uint16_t rnti);
void NotifyConnectionEstablishedEnb(std::string context,
                                    uint64_t imsi,
                                    uint16_t cellid,
                                    uint16_t rnti);

void NotifyHandoverStartEnb(std::string context,
                            uint64_t imsi,
                            uint16_t cellid,
                            uint16_t rnti,
                            uint16_t targetCellId);

void NotifyHandoverEndOkEnb(std::string context,
                            uint64_t imsi,
                            uint16_t cellid,
                            uint16_t rnti);

void TracePosition(Ptr<Node> ue, Time interval);

void NotifyUeMeasurements(std::string context, uint16_t rnti, uint16_t cellId, double rsrpDbm, double rsrqDbm, bool servingCell, uint8_t ccId);

void NotifyPacketSinkRx(std::string context, Ptr<const Packet> packet, const Address &address);

void NotifyCqiReport(std::string context, uint16_t rnti, uint8_t cqi);

void ConnectTcpTrace(int);

void CongStateTrace(const TcpSocketState::TcpCongState_t oldValue, const TcpSocketState::TcpCongState_t newValue);

void TracePosition(Ptr<Node> ue, Time interval);

void check_parse();

int main(int argc, char *argv[])
{

    CommandLine cmd;
    cmd.AddValue("speed", "Speed fo the UE (m/s)", speed);
    cmd.AddValue("x2Distance", "Distance between eNB at X2 (meters)", x2Distance);
    cmd.AddValue("yDistanceForUe", "y value(meters) for UE", yDistanceForUe);
    cmd.AddValue("enbTxPowerDbm", "Tx power(dBm) used by eNBs", enbTxPowerDbm);
    cmd.AddValue("rlcBearer", "LTE RLC Bearer mode", rlcBearer);
    cmd.AddValue("handoverType", "Handover type (A2A4 or A3Rsrp)", handoverType);
    cmd.AddValue("tcpType", "TCP congestion control algorithm(TcpBbr|TcpCubic|TcpNewReno|TcpBic)", tcpType);
    cmd.AddValue("verbose", "Enable verbose logging", verbose);
    cmd.AddValue("pcap", "Enable pcap tracing", pcap);
    cmd.AddValue("useIdealRrc", "Whether to use ideal Rrc", useIdealRrc);
    cmd.AddValue("positionTracingInterval", "The position interval ", _positionTracingInterval);
    cmd.AddValue("lteSchedulerType", "Set the type of scheduler to be used by eNodeB devices", lteSchedulerType);
    cmd.AddValue("internetDelay", "Set the transmission delay of the notional internet ", _internetDelay);
    cmd.AddValue("internetMtu", "Set the MTU of the notional internet ", internetMtu);
    cmd.AddValue("internetDataRate", "Set the DataRate of the notional internet", _internetDataRate);
    cmd.AddValue("bearerQos", "QoS Class Indicator. See 3GPP 23.203 Section 6.1.7.2 for standard values", bearerQos);
    cmd.AddValue("epochDuration", "Epoch duration.", _epochDuration);
    cmd.AddValue("intialWindowSize", "Initial window size.", intialWindowSize);
    cmd.AddValue("tcpMss", "TCP segment size.", tcpMss);
    cmd.AddValue("enbAUlBandWidth", "uplink bandwidth(RB) of enb A.", enbAUlBandWidth);
    cmd.AddValue("enbBUlBandWidth", "uplink bandwidth(RB) of enb B.", enbBUlBandWidth);
    cmd.AddValue("enbADlBandWidth", "downlink bandwidth(RB) of enb A.", enbADlBandWidth);
    cmd.AddValue("enbBDlBandWidth", "downlink bandwidth(RB) of enb B.", enbBDlBandWidth);

    cmd.Parse(parse_config_file("lte-tcp-x2-handover.conf"));

    check_parse();

    positionTracingInterval = Time(_positionTracingInterval);
    reportingInterval = Time(_reportingInterval);
    internetDataRate = DataRate(_internetDataRate);
    internetDelay = Time(_internetDelay);
    epochDuration = Time(_epochDuration);

    Config::SetDefault("ns3::LteHelper::UseIdealRrc", BooleanValue(useIdealRrc));
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::" + tcpType));
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(tcpMss));
    Config::SetDefault("ns3::TcpSocket::InitialCwnd", UintegerValue(intialWindowSize));

    // Adjust based on speed, if motion is enabled
    if (speed < 10 && speed != 0)
    {
        NS_ABORT_MSG("Select a speed at least 10 m/s, or zero");
    }
    else if (speed >= 10)
    {
        // Handover around the middle of the total simTime
        simTime = (double)(numberOfEnbs + 1) * x2Distance / speed;
    }

    LogLevel logLevel = (LogLevel)(LOG_PREFIX_ALL | LOG_LEVEL_ALL);
    if (verbose)
    {
        LogComponentEnable("EpcX2", logLevel);
        LogComponentEnable("LteTcpX2Handover", logLevel);
        LogComponentEnable("A2A4RsrqHandoverAlgorithm", logLevel);
        LogComponentEnable("A3RsrpHandoverAlgorithm", logLevel);
        NS_LOG_UNCOND("enable verbose");
    }
    if (rlcBearer == "RlcSmAlways")
    {   
        NS_LOG_UNCOND("RlcSmAlways");
        Config::SetDefault("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue(LteEnbRrc::RLC_SM_ALWAYS));
    }
    else if (rlcBearer == "RlcUmAlways")
    {
        Config::SetDefault("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue(LteEnbRrc::RLC_UM_ALWAYS));
    }
    else if (rlcBearer == "RlcAmAlways")
    {
        Config::SetDefault("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue(LteEnbRrc::RLC_AM_ALWAYS));
    }
    else if (rlcBearer == "PacketErrorRateBased")
    {
        Config::SetDefault("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue(LteEnbRrc::PER_BASED));
    }
    else if (rlcBearer == "MmwRlcUmAlways")
    {
        Config::SetDefault("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue(LteEnbRrc::RLC_UM_LOWLAT_ALWAYS));
    }
    else
    {
        NS_ABORT_MSG("The type of EPS bearer must be one of the following: RlcUmAlways,RlcUmAlways,RlcAmAlways,PacketErrorRateBased,and MmwRlcUmAlways");
    }

    g_ueMeasurements.open("ue-measurements.csv", std::ofstream::out);
    // time   cellId   isServingCell?  RSRP(dBm)  RSRQ(dB)
    g_ueMeasurements << "\"time\",\"cellId\",\"isServingCell\",\"RSRP\",\"RSRQ\"" << std::endl;
    g_packetSinkRx.open("tcp-receive.csv", std::ofstream::out);
    g_packetSinkRx << "\"time\",\"bytesRx\"" << std::endl;
    g_bulkSendTx.open("tcp-send.csv", std::ofstream::out);
    g_bulkSendTx << "\"time\",\"bytesTx\"" << std::endl;
    g_cqiTrace.open("cqi.csv", std::ofstream::out);
    g_cqiTrace << "\"time\",\"nodeId\",\"rnti\",\"cqi\"" << std::endl;
    g_tcpCongStateTrace.open("tcp-state.csv", std::ofstream::out);
    g_tcpCongStateTrace << "\"time\",\"congState\"" << std::endl;
    g_positionTrace.open("position.csv", std::ofstream::out);
    g_positionTrace << "\"time\",\"X\",\"Y\",\"Z\"" << std::endl;
    g_cwndTrace.open("cwnd.csv", std::ofstream::out);
    g_cwndTrace << "\"time\",\"cwnd\"" << std::endl;
    g_cwndTrace << std::setw(7) << std::setprecision(3) << std::fixed << 1.0
                << "," << std::setw(5) << intialWindowSize << std::endl;

    g_rttTrace.open("rtt.csv", std::ofstream::out);
    g_rttTrace << "\"time\",\"rtt\"" << std::endl;

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);
    lteHelper->SetSchedulerType("ns3::" + lteSchedulerType);

    if (handoverType == "A2A4")
    {
        lteHelper->SetHandoverAlgorithmType("ns3::A2A4RsrqHandoverAlgorithm");
        lteHelper->SetHandoverAlgorithmAttribute("ServingCellThreshold",
                                                 UintegerValue(30));
        lteHelper->SetHandoverAlgorithmAttribute("NeighbourCellOffset",
                                                 UintegerValue(1));
    }
    else if (handoverType == "A3Rsrp")
    {
        lteHelper->SetHandoverAlgorithmType("ns3::A3RsrpHandoverAlgorithm");
        lteHelper->SetHandoverAlgorithmAttribute("Hysteresis",
                                                 DoubleValue(3.0));
        lteHelper->SetHandoverAlgorithmAttribute("TimeToTrigger",
                                                 TimeValue(MilliSeconds(256)));
    }
    else
    {
        NS_ABORT_MSG("Unknown handover type: " << handoverType);
    }

    // Create a single RemoteHost
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    // Create the notional Internet
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(internetDataRate));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(internetMtu));
    p2ph.SetChannelAttribute("Delay", TimeValue(internetDelay));
    NetDeviceContainer internetDevices = p2ph.Install(epcHelper->GetPgwNode(), remoteHost);
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);

    // Routing of the Internet Host(towards the LTE network)
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    // interface 0 is localhost, 1 is the p2p device
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);

    NodeContainer ueNodes;
    NodeContainer enbNodes;
    enbNodes.Create(numberOfEnbs);
    ueNodes.Create(numberOfUes);

    // Install Mobility Model in eNB
    Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator>();
    for (uint16_t i = 0; i < numberOfEnbs; i++)
    {
        Vector enbPosition(x2Distance * (i + 1), x2Distance, 0);
        enbPositionAlloc->Add(enbPosition);
    }
    MobilityHelper enbMobility;
    enbMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    enbMobility.SetPositionAllocator(enbPositionAlloc);
    enbMobility.Install(enbNodes);

    // Install Mobility Model in UE
    MobilityHelper ueMobility;
    ueMobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    ueMobility.Install(ueNodes);
    ueNodes.Get(0)->GetObject<MobilityModel>()->SetPosition(Vector(0, yDistanceForUe, 0));
    ueNodes.Get(0)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(Vector(speed, 0, 0));

    // Install LTE Devices in eNB and UEs
    Config::SetDefault("ns3::LteEnbPhy::TxPower", DoubleValue(enbTxPowerDbm));
    NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice(enbNodes);
    DynamicCast<LteEnbNetDevice,NetDevice>(enbLteDevs.Get(0))->SetAttribute("UlBandwidth",UintegerValue(enbAUlBandWidth));
    DynamicCast<LteEnbNetDevice,NetDevice>(enbLteDevs.Get(0))->SetAttribute("DlBandwidth",UintegerValue(enbADlBandWidth));
    DynamicCast<LteEnbNetDevice,NetDevice>(enbLteDevs.Get(1))->SetAttribute("UlBandwidth",UintegerValue(enbBUlBandWidth));
    DynamicCast<LteEnbNetDevice,NetDevice>(enbLteDevs.Get(1))->SetAttribute("DlBandwidth",UintegerValue(enbBDlBandWidth));
    NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice(ueNodes);

    // Install the IP stack on the UEs
    internet.Install(ueNodes);
    Ipv4InterfaceContainer ueIpIfaces;
    ueIpIfaces = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueLteDevs));

    // Attach all UEs to the first eNodeB
    for (uint16_t i = 0; i < numberOfUes; ++i)
    {
        lteHelper->Attach(ueLteDevs.Get(i), enbLteDevs.Get(0));
        Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting(ueNodes.Get(0)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);
    }

    MyBulkSendHelper ftpServer("ns3::TcpSocketFactory", Address());
    AddressValue remoteAddress(InetSocketAddress(ueIpIfaces.GetAddress(0), port));
    ftpServer.SetAttribute("Remote", remoteAddress);
    ftpServer.SetAttribute("MaxBytes", UintegerValue(ftpSize));
    NS_LOG_INFO("setting up TCP flow from remote host to UE");
    ApplicationContainer sourceApp = ftpServer.Install(remoteHost);
    sourceApp.Start(Seconds(1));
    sourceApp.Stop(Seconds(simTime));

    Address sinkLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), port));
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", sinkLocalAddress);
    ApplicationContainer sinkApp = sinkHelper.Install(ueNodes.Get(0));
    sinkApp.Start(Seconds(1));
    sinkApp.Stop(Seconds(simTime));

    // EPS bearer Traffic Flow
    Ptr<EpcTft> tft = Create<EpcTft>();
    EpcTft::PacketFilter dlpf;
    dlpf.localPortStart = port;
    dlpf.localPortEnd = port;
    tft->Add(dlpf);

    // Qos
    int qci = str_to_Qci(bearerQos);
    if (qci == -1)
        NS_ABORT_MSG("Invalid qos class indicator : " << bearerQos);
    EpsBearer bearer((EpsBearer::Qci)qci);
    lteHelper->ActivateDedicatedEpsBearer(ueLteDevs.Get(0), bearer, tft);

    // Add X2 interface
    lteHelper->AddX2Interface(enbNodes);

    // Tracing
    if (pcap)
    {
        p2ph.EnablePcapAll(traceFilePrefix);
    }

    lteHelper->EnablePhyTraces();
    lteHelper->EnableMacTraces();
    lteHelper->EnableRlcTraces();
    lteHelper->EnablePdcpTraces();

    Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats();
    rlcStats->SetAttribute("EpochDuration", TimeValue(epochDuration));
    Ptr<RadioBearerStatsCalculator> pdcpStats = lteHelper->GetPdcpStats();
    pdcpStats->SetAttribute("EpochDuration", TimeValue(epochDuration));

    // connect custom trace sinks for RRC connection establishment and handover notification
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

    
    Config::Connect("/NodeList/"+std::to_string(ueNodes.Get(0)->GetId())+"/DeviceList/*/ComponentCarrierMapUe/*/LteUePhy/ReportUeMeasurements",
                    MakeCallback(&NotifyUeMeasurements));

    Config::Connect("/NodeList/"+std::to_string(ueNodes.Get(0)->GetId())+"/ApplicationList/*/$ns3::PacketSink/Rx",
                    MakeCallback(&NotifyPacketSinkRx));
    
    Config::Connect("/NodeList/*/DeviceList/*/$ns3::LteEnbNetDevice/ComponentCarrierMap/*/FfMacScheduler/$ns3::RrFfMacScheduler/WidebandCqiReport",
                    MakeCallback(&NotifyCqiReport));

    // Delay trace connection until TCP socket comes into existence
    Simulator::Schedule(Seconds(1.001), &ConnectTcpTrace,remoteHost->GetId());

    // Initiate position tracing
    Simulator::Schedule(Seconds(0), &TracePosition, ueNodes.Get(0), positionTracingInterval);

    Vector vUe = ueNodes.Get(0)->GetObject<MobilityModel>()->GetPosition();
    Vector vEnb1 = enbNodes.Get(0)->GetObject<MobilityModel>()->GetPosition();
    Vector vEnb2 = enbNodes.Get(1)->GetObject<MobilityModel>()->GetPosition();

    NS_LOG_INFO(
        "Initial positions:  UE: (" << vUe.x << "," << vUe.y << "), "
        << "eNB1: (" << vEnb1.x << "," << vEnb1.y << "), "
        << "eNB2: (" << vEnb2.x << "," << vEnb2.y << ")");
    NS_LOG_INFO(
        "Simulation time: " << simTime << " sec");
    Simulator::Schedule(reportingInterval, &ReportProgress, reportingInterval);
    Simulator::Stop(Seconds(simTime));
    Simulator::Run();
    Simulator::Destroy();

    // Close any open file descriptors
    g_ueMeasurements.close();
    g_packetSinkRx.close();
    g_bulkSendTx.close();
    g_cwndTrace.close();
    g_cqiTrace.close();
    g_tcpCongStateTrace.close();
    g_positionTrace.close();
    return 0;
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
    NS_LOG_INFO(
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
    NS_LOG_INFO(Simulator::Now().GetSeconds() << " node "
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
    NS_LOG_INFO(
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
    NS_LOG_INFO(
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
    NS_LOG_INFO(
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
    NS_LOG_INFO(
        Simulator::Now().GetSeconds() << " node "
                                      << ContextToNodeId(context)
                                      << " eNB CellId " << cellid
                                      << ": completed handover of UE with IMSI " << imsi
                                      << " RNTI " << rnti);
}

void NotifyUeMeasurements(std::string context, uint16_t rnti, uint16_t cellId, double rsrpDbm, double rsrqDbm, bool servingCell, uint8_t ccId)
{
    g_ueMeasurements << std::setw(7) << std::setprecision(3) << std::fixed << Simulator::Now().GetSeconds() << ","
                     << std::setw(3) << cellId << ","
                     << std::setw(3) << (servingCell ? "1" : "0") << ","
                     << std::setw(8) << rsrpDbm << ","
                     << std::setw(8) << rsrqDbm << std::endl;
}

void NotifyPacketSinkRx(std::string context, Ptr<const Packet> packet, const Address &address)
{
    g_packetSinkRx << std::setw(7) << std::setprecision(3) << std::fixed << Simulator::Now().GetSeconds()
                   << "," << std::setw(5) << packet->GetSize() << std::endl;
}

void NotifyCqiReport(std::string context, uint16_t rnti, uint8_t cqi)
{
    g_cqiTrace << std::setw(7) << std::setprecision(3) << std::fixed << Simulator::Now().GetSeconds() << ","
               << std::setw(4) << ContextToNodeId(context) << ","
               << std::setw(4) << rnti << ","
               << std::setw(3) << static_cast<uint16_t>(cqi) << std::endl;
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
                << "," << std::setw(5) << new_val / tcpMss << std::endl;
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

void TracePosition(Ptr<Node> ue, Time interval)
{
    Vector v = ue->GetObject<MobilityModel>()->GetPosition();
    g_positionTrace << std::setw(7) << std::setprecision(3) << std::fixed << Simulator::Now().GetSeconds() << ","
                    << v.x << "," << v.y << "," << v.z << std::endl;
    Simulator::Schedule(interval, &TracePosition, ue, interval);
}

void ReportProgress(Time reportingInterval)
{
    NS_LOG_INFO("*** Simulation time: " << std::fixed << std::setprecision(1) << Simulator::Now().GetSeconds() << "s");
    Simulator::Schedule(reportingInterval, &ReportProgress, reportingInterval);
}

void check_parse(){

}
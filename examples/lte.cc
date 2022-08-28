#include "lte.h"

NS_LOG_COMPONENT_DEFINE("ltetest");

int
main(int argc, char* argv[]){
    config_init(argc,argv);
    default_config();
    

    Ptr<LteHelper> lteHepler =CreateObject<LteHelper>();
    lteHepler->SetPathlossModelType(TypeId::LookupByName("ns3::"+lte_path_loss_model_type));
    Ptr<PointToPointEpcHelper> epcHepler=CreateObject<PointToPointEpcHelper>();
    lteHepler->SetEpcHelper(epcHepler);
    
    NodeContainer ueNodes;
    NodeContainer enbNodes;
    ueNodes.Create(1);
    enbNodes.Create(2);

    Ptr<Node> pgw = epcHepler->GetPgwNode();
    Ptr<Node> sNode = CreateObject<Node>();
    Ptr<Node> ueNode=ueNodes.Get(0);
    Ptr<Node> enbNodeA=enbNodes.Get(0);
    Ptr<Node> enbNodeB=enbNodes.Get(0);

    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute ("DataRate", DataRateValue (link1_data_rate));
    p2ph.SetDeviceAttribute ("Mtu", UintegerValue (link1_mtu));
    p2ph.SetChannelAttribute ("Delay", TimeValue (link1_delay));

    NetDeviceContainer devices=p2ph.Install(pgw,sNode);
    int pgw_device_id=get_device_id("pgw");
    int remote_host_device_id=get_device_id("remoteHost");

    Ptr<NetDevice> pDev=devices.Get(0);
    Ptr<NetDevice> sDev=devices.Get(1);

    if(enable_packet_drop_stats){
        devices.Get(0)->TraceConnectWithoutContext("MacTxDrop",MakeBoundCallback(&mac_tx_drop_callback,pgw_device_id));
        devices.Get(0)->TraceConnectWithoutContext("PhyTxDrop",MakeBoundCallback(&phy_tx_drop_callback,pgw_device_id));
        devices.Get(0)->TraceConnectWithoutContext("PhyRxDrop",MakeBoundCallback(&phy_rx_drop_callback,pgw_device_id));
        devices.Get(1)->TraceConnectWithoutContext("MacTxDrop",MakeBoundCallback(&mac_tx_drop_callback,remote_host_device_id));
        devices.Get(1)->TraceConnectWithoutContext("PhyTxDrop",MakeBoundCallback(&phy_tx_drop_callback,remote_host_device_id));
        devices.Get(1)->TraceConnectWithoutContext("PhyRxDrop",MakeBoundCallback(&phy_rx_drop_callback,remote_host_device_id));
    }

    InternetStackHelper internet;
    internet.Install(sNode);

    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0","255.0.0.0");
    auto ifaces=ipv4h.Assign(devices);
    Ipv4Address sAddr=ifaces.GetAddress(1);

    Ipv4StaticRoutingHelper routingHelper;
    Ptr<Ipv4StaticRouting> staticRouting = routingHelper.GetStaticRouting(sNode->GetObject<Ipv4>());
    staticRouting->AddNetworkRouteTo(Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
    positionAlloc->Add (Vector(0, 0, 0));
    positionAlloc->Add (Vector(10, 0, 0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.Install(enbNodes); //(0 0 0) (10 0 0)
    mobility.Install(ueNodes); // (0 0 0)

    NetDeviceContainer enbDevs=lteHepler->InstallEnbDevice(enbNodes);
    NetDeviceContainer ueDevs=lteHepler->InstallUeDevice(ueNodes);
    if(enable_packet_drop_stats){
        NS_LOG_UNCOND("enbDevice and ueDevice packet loss has not been implemented");
    }

    internet.Install(ueNodes);
    


    print_stats();


    
}
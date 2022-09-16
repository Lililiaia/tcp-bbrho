#include "ns3/mmwave-helper.h"
#include "ns3/epc-helper.h"
#include "ns3/node-list.h"
#include "ns3/isotropic-antenna-model.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store-module.h"
#include "ns3/mmwave-point-to-point-epc-helper.h"
#include "ns3/buildings-helper.h"
#include "ns3/buildings-module.h"
#include "ns3/global-value.h"
#include "ns3/command-line.h"
#include <ns3/random-variable-stream.h>
#include <ns3/lte-ue-net-device.h>
#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <list>
#include <iomanip>
#include "../mybulk-send-helper.h"
#include "../mybulk-send-application.h"

using namespace ns3;
using namespace mmwave;

NS_LOG_COMPONENT_DEFINE ("MmwaveTcpX2HandoverSimple");

// ** a pointer pointing a pointer
int
main(int argc, char**argv)
{
    Ptr<MmWaveHelper> mmwaveHelper = CreateObject<MmWaveHelper> ();
    Ptr<MmWavePointToPointEpcHelper> epcHelper = CreateObject<MmWavePointToPointEpcHelper> ();
    mmwaveHelper->SetEpcHelper (epcHelper);
    mmwaveHelper->Initialize ();

    Ptr<Node> pgw = epcHelper->GetPgwNode ();
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create (1);
    Ptr<Node> remoteHost = remoteHostContainer.Get (0);
    InternetStackHelper internet;
    internet.Install (remoteHostContainer);

    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
    p2ph.SetDeviceAttribute ("Mtu", UintegerValue (2500));
    p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));

    NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  NodeContainer ueNodes;
  NodeContainer mmWaveEnbNodes;
  NodeContainer lteEnbNodes;
  NodeContainer allEnbNodes;
  mmWaveEnbNodes.Create (2);
  lteEnbNodes.Create (1);
  ueNodes.Create (1);
  allEnbNodes.Add (lteEnbNodes);
  allEnbNodes.Add (mmWaveEnbNodes);

  Vector mmw1Position = Vector (50, 70, 3); // lte and mmWave
  Vector mmw2Position = Vector (150, 70, 3);

  Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
  //enbPositionAlloc->Add (Vector ((double)mmWaveDist/2 + streetWidth, mmw1Dist + 2*streetWidth, mmWaveZ));
  enbPositionAlloc->Add (mmw1Position); // LTE BS, out of area where buildings are deployed
  enbPositionAlloc->Add (mmw1Position);
  enbPositionAlloc->Add (mmw2Position);

  MobilityHelper enbmobility;
  enbmobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  enbmobility.SetPositionAllocator (enbPositionAlloc);
  enbmobility.Install (allEnbNodes);
  
  double ueInitialPosition = 90;
  MobilityHelper uemobility;
  Ptr<ListPositionAllocator> uePositionAlloc = CreateObject<ListPositionAllocator> ();
  //uePositionAlloc->Add (Vector (ueInitialPosition, -5, 0));
  uePositionAlloc->Add (Vector (ueInitialPosition, -5, 1.6));
  uemobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  uemobility.SetPositionAllocator (uePositionAlloc);
  uemobility.Install (ueNodes);

  ueNodes.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (ueInitialPosition, -5, 1.6));
  ueNodes.Get (0)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (0, 0, 0));

  NetDeviceContainer lteEnbDevs = mmwaveHelper->InstallLteEnbDevice (lteEnbNodes);
  NetDeviceContainer mmWaveEnbDevs = mmwaveHelper->InstallEnbDevice (mmWaveEnbNodes);
  NetDeviceContainer mcUeDevs;
  mcUeDevs = mmwaveHelper->InstallMcUeDevice (ueNodes);

  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (mcUeDevs));
  // Assign IP address to UEs, and install applications
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  mmwaveHelper->AddX2Interface (lteEnbNodes, mmWaveEnbNodes);
  mmwaveHelper->AttachToClosestEnb (mcUeDevs, mmWaveEnbDevs, lteEnbDevs);

  ApplicationContainer clientApps;
  ApplicationContainer serverApps;
  //bool dl = 1;
  //bool ul = 0;

    MyBulkSendHelper ftpServer("ns3::TcpSocketFactory", Address());
    AddressValue remoteAddress(InetSocketAddress(ueIpIface.GetAddress(0), 5000));
    ftpServer.SetAttribute("Remote", remoteAddress);
    ftpServer.SetAttribute("MaxBytes", UintegerValue(20000000000));
    NS_LOG_UNCOND("setting up TCP flow from remote host to UE");
    serverApps = ftpServer.Install(remoteHost);

    Address sinkLocalAddress(InetSocketAddress(ueIpIface.GetAddress(0), 5000));
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", sinkLocalAddress);
    clientApps = sinkHelper.Install(ueNodes.Get(0));
}
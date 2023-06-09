/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2007 University of Washington
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author:  Tom Henderson (tomhend@u.washington.edu)
 */
#include <fstream>
#include "ns3/address.h"
#include "ns3/address-utils.h"
#include "ns3/log.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/node.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/udp-socket-factory.h"
#include "my-packet-sink.h"
#include "ns3/boolean.h"
#include "ns3/ipv4-packet-info-tag.h"
#include "ns3/ipv6-packet-info-tag.h"
#include "ns3/tcp-socket-base.h"

extern uint64_t rx_bytes;
extern bool enable_rx_throughput_stats;
extern bool enable_rwnd_trace;
extern std::string root_dir;
extern int tcp_mss;
extern double stats_interval;
static void rx_stats(ns3::Time prevtime){
  static std::ofstream thr(root_dir + "rx_throutghput.csv", std::ios::out | std::ios::app);
  static bool is_first=true;
  if(is_first){
    thr<<"\"time(s)\",\"throughput(Kbps)\",\"Rx throughput\""<<std::endl;
    is_first=false;
  }
  ns3::Time curtime=ns3::Now();
  thr<<curtime.GetSeconds() << ","<< 8*rx_bytes/(1000*(curtime.GetSeconds()-prevtime.GetSeconds()))<<std::endl;
  rx_bytes=0;
  ns3::Simulator::Schedule(ns3::Seconds(stats_interval),rx_stats,curtime);
}
static void rx_trace(ns3::Ptr<const ns3::Packet> p, const ns3::TcpHeader& header,ns3::Ptr<const ns3::TcpSocketBase> socket){
    static bool started_stats=false;
    if(!started_stats){
       ns3::Simulator::Schedule(ns3::Seconds(stats_interval),rx_stats,ns3::Now());
       started_stats=true;
    }
    rx_bytes+=p->GetSize();
}
static void rwnd_trace(uint32_t old_val, uint32_t new_val){
    
    static std::ofstream thr(root_dir + "rwnd.trace", std::ios::out | std::ios::app);
    
    ns3::Time curtime=ns3::Now();
    thr<<curtime << " " << old_val/tcp_mss << " "<<new_val/tcp_mss<<std::endl;
}

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MyPacketSink");

NS_OBJECT_ENSURE_REGISTERED (MyPacketSink);



TypeId 
MyPacketSink::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MyPacketSink")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<MyPacketSink> ()
    .AddAttribute ("Local",
                   "The Address on which to Bind the rx socket.",
                   AddressValue (),
                   MakeAddressAccessor (&MyPacketSink::m_local),
                   MakeAddressChecker ())
    .AddAttribute ("Protocol",
                   "The type id of the protocol to use for the rx socket.",
                   TypeIdValue (UdpSocketFactory::GetTypeId ()),
                   MakeTypeIdAccessor (&MyPacketSink::m_tid),
                   MakeTypeIdChecker ())
    .AddAttribute ("EnableSeqTsSizeHeader",
                   "Enable optional header tracing of SeqTsSizeHeader",
                   BooleanValue (false),
                   MakeBooleanAccessor (&MyPacketSink::m_enableSeqTsSizeHeader),
                   MakeBooleanChecker ())
    .AddTraceSource ("Rx",
                     "A packet has been received",
                     MakeTraceSourceAccessor (&MyPacketSink::m_rxTrace),
                     "ns3::Packet::AddressTracedCallback")
    .AddTraceSource ("RxWithAddresses", "A packet has been received",
                     MakeTraceSourceAccessor (&MyPacketSink::m_rxTraceWithAddresses),
                     "ns3::Packet::TwoAddressTracedCallback")
    .AddTraceSource ("RxWithSeqTsSize",
                     "A packet with SeqTsSize header has been received",
                     MakeTraceSourceAccessor (&MyPacketSink::m_rxTraceWithSeqTsSize),
                     "ns3::MyPacketSink::SeqTsSizeCallback")
  ;
  return tid;
}

MyPacketSink::MyPacketSink ()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_totalRx = 0;
}

MyPacketSink::~MyPacketSink()
{
  NS_LOG_FUNCTION (this);
}

uint64_t MyPacketSink::GetTotalRx () const
{
  NS_LOG_FUNCTION (this);
  return m_totalRx;
}

Ptr<Socket>
MyPacketSink::GetListeningSocket (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socket;
}

std::list<Ptr<Socket> >
MyPacketSink::GetAcceptedSockets (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socketList;
}

void MyPacketSink::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_socketList.clear ();

  // chain up
  Application::DoDispose ();
}


// Application Methods
void MyPacketSink::StartApplication ()    // Called at time specified by Start
{
  NS_LOG_FUNCTION (this);
  // Create the socket if not already
  if (!m_socket)
    {
      m_socket = Socket::CreateSocket (GetNode (), m_tid);
      if (m_socket->Bind (m_local) == -1)
        {
          NS_FATAL_ERROR ("Failed to bind socket");
        }
      m_socket->Listen ();
      m_socket->ShutdownSend ();
      if (addressUtils::IsMulticast (m_local))
        {
          Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket);
          if (udpSocket)
            {
              // equivalent to setsockopt (MCAST_JOIN_GROUP)
              udpSocket->MulticastJoinGroup (0, m_local);
            }
          else
            {
              NS_FATAL_ERROR ("Error: joining multicast on a non-UDP socket");
            }
        }
    }

  if (InetSocketAddress::IsMatchingType (m_local))
    {
      m_localPort = InetSocketAddress::ConvertFrom (m_local).GetPort ();
    }
  else if (Inet6SocketAddress::IsMatchingType (m_local))
    {
      m_localPort = Inet6SocketAddress::ConvertFrom (m_local).GetPort ();
    }
  else
    {
      m_localPort = 0;
    }
  m_socket->SetRecvCallback (MakeCallback (&MyPacketSink::HandleRead, this));
  m_socket->SetRecvPktInfo (true);
  m_socket->SetAcceptCallback (
    MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
    MakeCallback (&MyPacketSink::HandleAccept, this));
  m_socket->SetCloseCallbacks (
    MakeCallback (&MyPacketSink::HandlePeerClose, this),
    MakeCallback (&MyPacketSink::HandlePeerError, this));
}

void MyPacketSink::StopApplication ()     // Called at time specified by Stop
{
  NS_LOG_FUNCTION (this);
  while(!m_socketList.empty ()) //these are accepted sockets, close them
    {
      Ptr<Socket> acceptedSocket = m_socketList.front ();
      m_socketList.pop_front ();
      acceptedSocket->Close ();
    }
  if (m_socket) 
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
}

void MyPacketSink::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
  Address from;
  Address localAddress;
  while ((packet = socket->RecvFrom (from)))
    {
      if (packet->GetSize () == 0)
        { //EOF
          break;
        }
      m_totalRx += packet->GetSize ();
      if (InetSocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO ("At time " << Simulator::Now ().As (Time::S)
                       << " packet sink received "
                       <<  packet->GetSize () << " bytes from "
                       << InetSocketAddress::ConvertFrom(from).GetIpv4 ()
                       << " port " << InetSocketAddress::ConvertFrom (from).GetPort ()
                       << " total Rx " << m_totalRx << " bytes");
        }
      else if (Inet6SocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO ("At time " << Simulator::Now ().As (Time::S)
                       << " packet sink received "
                       <<  packet->GetSize () << " bytes from "
                       << Inet6SocketAddress::ConvertFrom(from).GetIpv6 ()
                       << " port " << Inet6SocketAddress::ConvertFrom (from).GetPort ()
                       << " total Rx " << m_totalRx << " bytes");
        }

      if (!m_rxTrace.IsEmpty () || !m_rxTraceWithAddresses.IsEmpty () ||
          (!m_rxTraceWithSeqTsSize.IsEmpty () && m_enableSeqTsSizeHeader))
        {
          Ipv4PacketInfoTag interfaceInfo;
          Ipv6PacketInfoTag interface6Info;
          if (packet->RemovePacketTag (interfaceInfo))
            {
              localAddress = InetSocketAddress (interfaceInfo.GetAddress (), m_localPort);
            }
          else if (packet->RemovePacketTag (interface6Info))
            {
              localAddress = Inet6SocketAddress (interface6Info.GetAddress (), m_localPort);
            }
          else
            {
              socket->GetSockName (localAddress);
            }
          m_rxTrace (packet, from);
          m_rxTraceWithAddresses (packet, from, localAddress);

          if (!m_rxTraceWithSeqTsSize.IsEmpty () && m_enableSeqTsSizeHeader)
            {
              PacketReceived (packet, from, localAddress);
            }
        }
    }
}

void
MyPacketSink::PacketReceived (const Ptr<Packet> &p, const Address &from,
                            const Address &localAddress)
{
  SeqTsSizeHeader header;
  Ptr<Packet> buffer;

  auto itBuffer = m_buffer.find (from);
  if (itBuffer == m_buffer.end ())
    {
      itBuffer = m_buffer.insert (std::make_pair (from, Create<Packet> (0))).first;
    }

  buffer = itBuffer->second;
  buffer->AddAtEnd (p);
  buffer->PeekHeader (header);

  NS_ABORT_IF (header.GetSize () == 0);

  while (buffer->GetSize () >= header.GetSize ())
    {
      NS_LOG_DEBUG ("Removing packet of size " << header.GetSize () << " from buffer of size " << buffer->GetSize ());
      Ptr<Packet> complete = buffer->CreateFragment (0, static_cast<uint32_t> (header.GetSize ()));
      buffer->RemoveAtStart (static_cast<uint32_t> (header.GetSize ()));

      complete->RemoveHeader (header);

      m_rxTraceWithSeqTsSize (complete, from, localAddress, header);

      if (buffer->GetSize () > header.GetSerializedSize ())
        {
          buffer->PeekHeader (header);
        }
      else
        {
          break;
        }
    }
}

void MyPacketSink::HandlePeerClose (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}
 
void MyPacketSink::HandlePeerError (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}

void MyPacketSink::HandleAccept (Ptr<Socket> s, const Address& from)
{
  NS_LOG_FUNCTION (this << s << from);
  s->SetRecvCallback (MakeCallback (&MyPacketSink::HandleRead, this));
  Ptr<TcpSocketBase> tcpSocket=DynamicCast<TcpSocketBase,Socket>(s);
  if(tcpSocket&&enable_rx_throughput_stats){
    tcpSocket->TraceConnectWithoutContext("Rx",MakeCallback(&rx_trace));
  }
  if(tcpSocket&&enable_rwnd_trace){
    tcpSocket->TraceConnectWithoutContext("RWND",MakeCallback(&rwnd_trace));
  }
  m_socketList.push_back (s);
}

} // Namespace ns3

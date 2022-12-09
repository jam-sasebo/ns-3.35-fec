/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*                                       
 * Copyright (c) 2016 Koga Laboratory    
 *                                       
 * Author: Yurino Sato <yuri@kogalab.net>
 */    

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/tcp-header.h"
#include "ns3/packet-sink.h"
#include "ns3/traffic-control-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/fec-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TcpFecSample");

///*
void
CwndTracer (Ptr<OutputStreamWrapper>stream, uint32_t oldcwnd, uint32_t newcwnd)
{
  *stream->GetStream () << "time " << Simulator::Now ().GetSeconds ()
                        << " new " << newcwnd 
                        << " old " << oldcwnd;
  if (newcwnd >= oldcwnd)
    {
      *stream->GetStream () << " diff + " << newcwnd-oldcwnd 
                            << std::endl;
    }
  else
   {
      *stream->GetStream () << " diff - " << oldcwnd-newcwnd 
                            << std::endl;
   }
}

void
MyEventHandller (Ptr<Application> app, Ptr<OutputStreamWrapper> stream)
{
  Ptr<Socket> src_socket  = app->GetObject<BulkSendApplication>()->GetSocket();
  src_socket->TraceConnectWithoutContext("CongestionWindow",
                                         MakeBoundCallback(&CwndTracer, stream));
}
//*/

int main (int argc, char *argv[])
{
  uint16_t    port = 5000;
  
  uint32_t    nFlows = 2;
  uint32_t    nRouters = 2;
  uint32_t    RngSeed = 1; 
  uint32_t    maxbytes = 0;
  double      runTime = 220.0; 
  double      coredelayvalue = 10;
  double      corebandvalue = 100;
  uint32_t    segmentSize = 1000;
  uint32_t    corebuffer = 100;
  uint32_t    buffer = segmentSize*corebuffer*20;
  uint32_t    run = 0;
  bool sack = true;

  bool fecmode = true;
  uint32_t fwin = 10;
  uint8_t nGroups = 1;
  std::string sendRxRequest = "ON";
  std::string storeMode = "USE";

  std::string tcpCong = "ns3::TcpNewReno"; 
  std::string recovery = "ns3::TcpClassicRecovery";
  std::string bandwidth = "1000Mbps";
  std::string delay = "5ms";
  
  /*
  LogComponentEnable("BulkSendApplication",LOG_LEVEL_FUNCTION);
  LogComponentEnable("PacketSink",LOG_LEVEL_FUNCTION);
  LogComponentEnable("TcpSocket",LOG_LEVEL_FUNCTION);
  LogComponentEnable("TcpSocketBase",LOG_LEVEL_FUNCTION);
  LogComponentEnable("SocketFactory",LOG_LEVEL_FUNCTION);
  LogComponentEnable("IpL4Protocol",LOG_LEVEL_FUNCTION);
  LogComponentEnable("TcpL4Protocol",LOG_LEVEL_FUNCTION);
  LogComponentEnable("Socket",LOG_LEVEL_FUNCTION);
  LogComponentEnable("Ipv4L3Protocol",LOG_LEVEL_FUNCTION);
  LogComponentEnable("Ipv4L3ProtocolwithFecDecode",LOG_LEVEL_FUNCTION); 
  LogComponentEnable("TrafficControlLayer",LOG_LEVEL_FUNCTION);
  LogComponentEnable("NetDevice",LOG_LEVEL_FUNCTION);
  LogComponentEnable("Queue",LOG_LEVEL_FUNCTION);
  LogComponentEnable("DropTailQueue",LOG_LEVEL_FUNCTION);
  LogComponentEnable("QueueDisc",LOG_LEVEL_FUNCTION);
  LogComponentEnable("FifoQueueDisc",LOG_LEVEL_FUNCTION);
  LogComponentEnable("FecQueueDisc",LOG_LEVEL_FUNCTION);
  */

  CommandLine cmd;
  cmd.AddValue ("RngSeed","Set seed for random value",RngSeed);
  cmd.AddValue ("nFlows","Number of flows",nFlows);
  cmd.AddValue ("nRouters","Number of intermediate routers",nRouters);
  cmd.AddValue ("coredelay", "coredelay", coredelayvalue);
  cmd.AddValue ("coreband", "coreband", corebandvalue);
  cmd.AddValue ("bandwidth", "bandwidth", bandwidth);
  cmd.AddValue ("corebuffer", "corebuffer", corebuffer);
  cmd.AddValue ("run", "Run index (for setting repeatable seeds)", run);
  cmd.AddValue ("sack", "Enable or disable SACK option", sack);
  cmd.AddValue ("recovery", "Recovery algorithm type to use (e.g., ns3::TcpPrrRecovery", recovery);
  cmd.AddValue ("fecmode", "Enable or disable FEC Mode", fecmode);
  cmd.AddValue ("fwin","fec window size", fwin);
  cmd.AddValue ("nGroups","Number of groups", nGroups);
  cmd.AddValue ("sendRxRequest","Send Retransmission Request", sendRxRequest);
  cmd.AddValue ("storeMode","USE or DISUSE storeMode", storeMode);
  cmd.Parse(argc, argv);

  std::ostringstream corebandStream;
  corebandStream << corebandvalue << "Mbps";
  std::string coreband = corebandStream.str ();
  
  std::ostringstream coredelayStream;
  coredelayStream << coredelayvalue << "ms";
  std::string coredelay = coredelayStream.str ();

  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue (tcpCong)); 
  Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (buffer));
  Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (buffer));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (segmentSize));
  Config::SetDefault ("ns3::TcpSocket::InitialCwnd", UintegerValue (2));
  Config::SetDefault ("ns3::TcpSocket::DelAckCount", UintegerValue (1)); 
  Config::SetDefault ("ns3::FifoQueueDisc::MaxSize", StringValue ("1000p")); 
  Config::SetDefault ("ns3::TcpSocketBase::Sack", BooleanValue (sack));
  Config::SetDefault ("ns3::TcpSocketBase::UseEcn", StringValue ("On"));
  Config::SetDefault ("ns3::TcpL4Protocol::RecoveryType", TypeIdValue (TypeId::LookupByName (recovery)));

  Config::SetDefault ("ns3::FecRouterAgent::FecWin", UintegerValue (fwin));
  Config::SetDefault ("ns3::FecRouterAgent::NumberofGroups", UintegerValue (nGroups));
  Config::SetDefault ("ns3::FecRouterAgent::SegmentSize", UintegerValue (segmentSize));
  Config::SetDefault ("ns3::FecRouterAgent::SendRxRequest", StringValue (sendRxRequest));
  Config::SetDefault ("ns3::FecRouterAgent::StoreMode", StringValue (storeMode));
  
  Config::SetDefault ("ns3::FecAgent::FecWin", UintegerValue (0));
  Config::SetDefault ("ns3::FecAgent::NumberofGroups", UintegerValue (1));
  Config::SetDefault ("ns3::FecAgent::SegmentSize", UintegerValue (segmentSize));

  //RngSeed
  SeedManager::SetSeed(RngSeed);
  SeedManager::SetRun(run);
  Ptr<UniformRandomVariable> random = CreateObject<UniformRandomVariable> ();
  random->SetAttribute ("Min",DoubleValue(0));
  random->SetAttribute ("Max",DoubleValue(1));
  double start_time[nFlows];
  double startTime[nFlows];

  NS_LOG_DEBUG("Creating Topology");

  /*
    NODE
  */
  NodeContainer sourcesNodes;
  sourcesNodes.Create(nFlows);
  
  NodeContainer routersNodes;
  routersNodes.Create(3);
  
  NodeContainer destinationsNodes;
  destinationsNodes.Create(nFlows);
  
  std::vector<NodeContainer> S_R1Nodes;
  std::vector<NodeContainer> R3_DNodes;  
  
  //client - router1
  for (uint32_t i = 0; i < nFlows; i++)
    {
      NodeContainer s_rnodes;
      s_rnodes.Add (sourcesNodes.Get (i));
      s_rnodes.Add (routersNodes.Get (0));
      S_R1Nodes.push_back (s_rnodes);
    }
  
  //router1-router2
  NodeContainer R1_R2Nodes;
  R1_R2Nodes.Add (routersNodes.Get (0));
  R1_R2Nodes.Add (routersNodes.Get (1));

  //router2-router3
  NodeContainer R2_R3Nodes;
  R2_R3Nodes.Add (routersNodes.Get (1));
  R2_R3Nodes.Add (routersNodes.Get (2));
  
  //router3-Destination
  for (uint32_t i = 0; i < nFlows; i++)
    {
      NodeContainer r_dnodes;
      r_dnodes.Add (routersNodes.Get (2));
      r_dnodes.Add (destinationsNodes.Get (i));
      R3_DNodes.push_back (r_dnodes);
    }
  
  /*
    DEVICES
  */
  std::vector<NetDeviceContainer> S_R1NetDevices;
  std::vector<NetDeviceContainer> R3_DNetDevices;
 
  /* 
  InternetStackHelper stack;
  stack.InstallAll ();
  */
  InternetStackHelper internetStack;                                                   
  internetStack.Install (sourcesNodes);                                                  
  internetStack.Install (destinationsNodes);  


  if (fecmode)                                                                          
    {                                              
     internetStack.InstallwithFec (routersNodes.Get (0), 0); //0はencodeという意味である    
     internetStack.InstallwithFec (routersNodes.Get (2), 1); //1はdecodeという意味である
     internetStack.Install (routersNodes.Get (1)); //
     FecRouterAgentHelper FecRouterAgent;
     FecRouterAgent.Create (1);
     FecRouterAgent.Set (0, routersNodes.Get(0),0);
     FecRouterAgent.Set (0, routersNodes.Get(2),1);
    }                                                                                
  else                                                                                  
    {                                                                                  
     internetStack.Install (routersNodes);                                                  
    }    

  TrafficControlHelper tch; 
  tch.SetRootQueueDisc ("ns3::FifoQueueDisc", 
                        "MaxSize", StringValue (std::to_string (buffer) + "p"));

  TrafficControlHelper fectch; 
  fectch.SetRootQueueDisc ("ns3::FifoQueueDisc", 
                           "MaxSize", StringValue (std::to_string (corebuffer) + "p"));


  //source - router1
  PointToPointHelper S_R1p2p;
  for (uint32_t i = 0; i < nFlows; i++)
    {
      //S_R1p2p.SetQueue ("ns3::DropTailQueue", 
      //                  "MaxSize", StringValue (std::to_string (buffer) + "p"));
      S_R1p2p.SetDeviceAttribute ("DataRate", StringValue (bandwidth));      
      S_R1p2p.SetChannelAttribute ("Delay", StringValue (delay));    
      NetDeviceContainer s_rdevices;
      s_rdevices = S_R1p2p.Install (S_R1Nodes.at (i));
      tch.Install (s_rdevices);
      S_R1NetDevices.push_back (s_rdevices);
    }

  //router1 - router2
  PointToPointHelper R1_R2p2p;
/*  R1_R2p2p.SetDeviceAttribute ("DataRate", StringValue (coreband)); below is R1_R2 bw  */     
  /*
  ボトルネックを作る
  */
/*  std::ostringstream bottleneckBandStream;
  bottleneckBandStream << nFlows*1000 << "Mbps";   
  std::string bottleneckBand = bottleneckBandStream.str ();*/

  R1_R2p2p.SetDeviceAttribute ("DataRate", StringValue (bandwidth)); 
  R1_R2p2p.SetChannelAttribute ("Delay", StringValue (delay));      
  NetDeviceContainer R1_R2NetDevices;
  R1_R2NetDevices = R1_R2p2p.Install (R1_R2Nodes);
  tch.Install(R1_R2NetDevices);

  //router2 - router3
  PointToPointHelper R2_R3p2p;
  R2_R3p2p.SetDeviceAttribute ("DataRate", StringValue (coreband));      
  R2_R3p2p.SetChannelAttribute ("Delay", StringValue (coredelay));      
  NetDeviceContainer R2_R3NetDevices;
  R2_R3NetDevices = R2_R3p2p.Install (R2_R3Nodes);
  fectch.Install(R2_R3NetDevices);  

  //router3 - destination
  PointToPointHelper R3_Dp2p;
  for ( uint32_t i = 0; i < nFlows; i++)
    {
      //R3_Dp2p.SetQueue ("ns3::DropTailQueue", 
      //                  "MaxSize", StringValue (std::to_string (buffer) + "p"));
      R3_Dp2p.SetDeviceAttribute ("DataRate", StringValue (bandwidth));
      R3_Dp2p.SetChannelAttribute ("Delay", StringValue (delay)); 
      NetDeviceContainer r_ddevices;
      r_ddevices = R3_Dp2p.Install (R3_DNodes.at (i));
      tch.Install (r_ddevices);
      R3_DNetDevices.push_back (r_ddevices);
    }
  /*
    ADDRESS 
  */
  Ipv4AddressHelper address;
  std::vector <Ipv4InterfaceContainer> S_R1Interfaces;
  std::vector <Ipv4InterfaceContainer> R1_R2Interfaces;
  std::vector <Ipv4InterfaceContainer> R2_R3Interfaces;
  std::vector <Ipv4InterfaceContainer> R3_DInterfaces;
  
  //source - router1
  address.SetBase ("10.1.1.0", "255.255.255.0");
  for (uint32_t i = 0; i < nFlows; i++)
    {
      Ipv4InterfaceContainer s_rinterfaces;
      s_rinterfaces = address.Assign (S_R1NetDevices.at(i));
      S_R1Interfaces.push_back (s_rinterfaces);
      address.NewNetwork ();
    }
  
  //router1 - router2
  address.SetBase ("10.2.1.0","255.255.255.0");
  Ipv4InterfaceContainer r1_r2Interfaces;
  r1_r2Interfaces = address.Assign (R1_R2NetDevices);
  R1_R2Interfaces.push_back (r1_r2Interfaces);
  address.NewNetwork ();

  //router2 - router3
  address.SetBase ("10.3.1.0","255.255.255.0");
  Ipv4InterfaceContainer r2_r3Interfaces;
  r2_r3Interfaces = address.Assign (R2_R3NetDevices);
  R2_R3Interfaces.push_back (r2_r3Interfaces);
  address.NewNetwork ();
  //router3 - destination
  address.SetBase ("10.4.1.0","255.255.255.0");
  for (uint32_t i = 0; i < nFlows; i++)
    {
      Ipv4InterfaceContainer r_dinterfaces;
      r_dinterfaces = address.Assign (R3_DNetDevices.at (i));
      R3_DInterfaces.push_back (r_dinterfaces);
      address.NewNetwork ();
    }
  
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  FecAgentContainer fec;
  fec.Create (nFlows);

  /*SOCKET*/
  for (uint32_t i = 0; i < nFlows; i++)
    {
      BulkSendHelper src ("ns3::TcpSocketFactory", InetSocketAddress (R3_DInterfaces.at (i).GetAddress (1), port));
      src.SetAttribute ("MaxBytes", UintegerValue (maxbytes));
      src.SetAttribute ("SendSize", UintegerValue (segmentSize));
      ApplicationContainer sourceApp = src.Install (S_R1Nodes.at (i).Get (0));
      fec.Install (i, sourceApp);
      fec.Get (i)->SetAttribute ("srcId", UintegerValue (i));
   
      start_time[i] = random->GetValue ();
      startTime[i] = start_time[i];
      sourceApp.Start (Seconds (startTime[i]));
      //sourceApp.Start (Seconds (0.0));
      sourceApp.Stop (Seconds (runTime));


    }

  for (uint32_t i = 0; i < nFlows; i++)
    {
      PacketSinkHelper sink ("ns3::TcpSocketFactory", InetSocketAddress (R3_DInterfaces.at (i).GetAddress (1), port));
      ApplicationContainer sinkApp = sink.Install (R3_DNodes.at (i).Get (1));
      fec.Install (i, sinkApp);
      fec.Get (i)->SetAttribute ("dstId", UintegerValue (nFlows+2+i));

      sinkApp.Start (Seconds (0.0));
      sinkApp.Stop (Seconds (runTime));
    } 


   /*
    TRACE
  */
 AsciiTraceHelper ascii;

 FecAgent::EnableTcpDump (ascii.CreateFileStream ("FecRouter.TcpDump"));
 FecAgent::EnablePrintTimeout (ascii.CreateFileStream ("FecRouter.Timeout"));
 FecAgent::EnablePrintFastRecovery (ascii.CreateFileStream ("FecRouter.FastRecovery"));
 FecAgent::EnablePrintRetransmit (ascii.CreateFileStream ("FecRouter.Retransmit"));
 FecRouterAgent::EnablePrintRoundFecTable (ascii.CreateFileStream ("FecRouter.RoundFecTable"));
 FecRouterAgent::EnablePrintGroupFecTable (ascii.CreateFileStream ("FecRouter.GroupFecTable"));
 FecRouterAgent::EnablePrintRecovery (ascii.CreateFileStream ("FecRouter.Recovery"));
 FecRouterAgent::EnablePrintLostPacket (ascii.CreateFileStream ("FecRouter.LostPacket"));
 FecRouterAgent::EnablePrintSentPacket (ascii.CreateFileStream ("FecRouter.SentPacket"));
 

 char str[100];
 char str1[]="fec-router-cwnd";
 char str3[]=".dat";
 
 for (uint32_t i = 0; i < nFlows; i++)
 {
    sprintf(str,"%s%d%s",str1,i,str3);
    Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream(str);
    Simulator::Schedule (Seconds (startTime[i])+NanoSeconds (1.0),
      &MyEventHandller, S_R1Nodes.at (i).Get(0)->GetApplication(0), stream);
 }

  /* 
    Flow monitor
  */
  FlowMonitorHelper flowHelper;
  Ptr<FlowMonitor> monitor = flowHelper.InstallAll ();
  /*
    Simulator
  */

  Simulator::Stop (Seconds(runTime));
  Simulator::Run ();
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

      std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
      std::cout << "  Tx Packets:   " << i->second.txPackets << "\n";
      std::cout << "  Tx Bytes:     " << i->second.txBytes << "\n";
      std::cout << "  TxOffered:    " << i->second.txBytes * 8.0 / runTime / 1000 / 1000  << " Mbps\n";
      std::cout << "  Rx Packets:   " << i->second.rxPackets << "\n";
      std::cout << "  Rx Bytes:     " << i->second.rxBytes << "\n";
      std::cout << "  Lost Packets: " << i->second.lostPackets << "\n";
      std::cout << "  Throughput:   " << i->second.rxBytes * 8.0 / runTime / 1000 / 1000  << " Mbps\n";
    }


  Simulator::Destroy ();
  
}

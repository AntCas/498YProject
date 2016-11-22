#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/athstats-helper.h"

#include "ns3/data-rate.h"
#include "ns3/flow-monitor-helper.h"

#include <iostream>
#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("PartOne");

std::ofstream g_cur_logs;


void run_experiment_2 (bool do_rts, int cbr, int seconds, std::string log_name) {
  g_cur_logs.open(log_name);

  printf(" **** Running for %d seconds with %dkb/s cbr and %s ****\n",
    seconds, cbr,
    do_rts ? "RTS enabled" : "RTS disabled");
  
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("999992200"));
  if (do_rts)
    Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("0"));
  else
    Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("999999999"));


  // Nodes
  NS_LOG_INFO("Nodes.");
  NodeContainer staNodes;
  NodeContainer apNodes;
  staNodes.Create(4);
  apNodes.Create(1);

  NS_LOG_INFO("Wifi.");
  YansWifiChannelHelper chan = YansWifiChannelHelper::Default();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
  phy.SetChannel(chan.Create());

  phy.Set("TxGain", DoubleValue(-3.0));

  //WifiHelper wifi = WifiHelper::Default();
  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
    "DataMode",StringValue ("DsssRate11Mbps"), 
    "ControlMode",StringValue ("DsssRate11Mbps"));
  WifiMacHelper mac;

  NS_LOG_INFO("SSID.");
  Ssid ssid = Ssid("tst");
  mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install(phy,mac, staNodes);

  // AP
  NS_LOG_INFO("AP.");
  mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
  NetDeviceContainer apDevices;
  apDevices = wifi.Install(phy,mac,apNodes);

  // Positioning
  // With TxGain:-3.0, falloff is around 40-60 meters
  double d1= 5.0; // meters
  double d2= 40.0; // meters
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add(Vector(0.0,0.0,0.0)); // ap
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(apNodes);
  positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add(Vector(d2,d2,0.0)); // A
  positionAlloc->Add(Vector(d1,-d1,0.0)); // B
  positionAlloc->Add(Vector(-d2,-d2,0.0)); // C
  positionAlloc->Add(Vector(d1,-d1,0.0)); // D
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(staNodes);

  // Protocol stack
  NS_LOG_INFO("Stack.");
  InternetStackHelper stack;
  stack.Install(staNodes);
  stack.Install(apNodes);

  Ipv4AddressHelper addr;
  addr.SetBase("10.0.0.0", "255.255.255.0");
  addr.Assign(staDevices);
  addr.Assign(apDevices);

  int port = 12345;
  // A -> B
  OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.2"), port)); // to node 1
  onoff.SetAttribute ("PacketSize", UintegerValue (2000));
  onoff.SetConstantRate(DataRate( std::to_string(cbr) + "kb/s"));
  ApplicationContainer apps;
  apps.Add(onoff.Install(staNodes.Get(0))); // from node 0
  // C -> D
  OnOffHelper onoff2("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.4"), port)); // to node 1
  onoff2.SetAttribute ("PacketSize", UintegerValue (2000));
  onoff2.SetConstantRate(DataRate( std::to_string(cbr) + "kb/s"));
  apps.Add(onoff2.Install(staNodes.Get(2))); // from node 2


  NS_LOG_INFO("Sock Sink.");
  // CBR 1
  PacketSinkHelper sink = PacketSinkHelper("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.2"), port));
  apps.Add( sink.Install(staNodes.Get(1)) );
  // CBR 2
  PacketSinkHelper sink2 = PacketSinkHelper("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.2"), port));
  apps.Add( sink2.Install(staNodes.Get(3)) );
  apps.Start(Seconds(0.0));
  apps.Stop(Seconds((int)seconds));

  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();

  Simulator::Stop(Seconds((int)seconds + 50));
  Simulator::Run();
  Simulator::Destroy();
  flowMonitor->SerializeToXmlFile(log_name + ".xml", true, true);
  NS_LOG_INFO("Done!");
}


int main(int argc, char** argv) {
  Time::SetResolution(Time::NS);
  std::cout << "Main starting.\n";

#define S 5 // seconds
#define M 50 // freq
#define MM 2 // freq
#define L 25 // num loops

  for ( int ii = 1; ii < L; ii++ ) {
    if ( ii > L/2)
      run_experiment_2(true, ii*M*MM, S, "rtscts/" + std::to_string(ii));
    else
      run_experiment_2(true, ii*M, S, "rtscts/" + std::to_string(ii));
  }
  for ( int ii = 1; ii < L; ii++ ) {
    if ( ii > L/2)
      run_experiment_2(false, ii*M*MM, S, "no_rtscts/" + std::to_string(ii));
    else
      run_experiment_2(false, ii*M, S, "no_rtscts/" + std::to_string(ii));
  }

}


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
#include <cmath>
#include <sys/types.h>
#include <sys/stat.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("PartOne");



void run_experiment_3 (int cbr_12, int cbr_3, int seconds, std::string log_name) {

  printf(" **** Running for %d seconds with %dkb/s cbr_12 and %dkb/s cbr_3 ****\n", seconds, cbr_12, cbr_3);
  
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("999992200"));
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("999999999"));


  // Nodes
  NS_LOG_INFO("Nodes.");
  NodeContainer staNodes;
  NodeContainer staHackedNodes;
  NodeContainer apNodes;
  staNodes.Create(4);
  staHackedNodes.Create(2);
  apNodes.Create(1);

  NS_LOG_INFO("Wifi.");
  YansWifiChannelHelper chan = YansWifiChannelHelper::Default();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
  phy.SetChannel(chan.Create());

  phy.Set("TxGain", DoubleValue(-3.0));

  // Wifi
  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
    "DataMode",StringValue ("DsssRate11Mbps"), 
    "ControlMode",StringValue ("DsssRate11Mbps"));
  // Wifi Hacked
  WifiHelper wifiHacked;
  wifiHacked.SetStandard (WIFI_PHY_STANDARD_80211b_HACKED);
  wifiHacked.SetRemoteStationManager("ns3::ConstantRateWifiManager",
    "DataMode",StringValue ("DsssRate11Mbps"), 
    "ControlMode",StringValue ("DsssRate11Mbps"));
  WifiMacHelper mac;

  NS_LOG_INFO("SSID.");
  Ssid ssid = Ssid("tst");
  mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install(phy,mac, staNodes);

  NetDeviceContainer staHackedDevices;
  staHackedDevices = wifiHacked.Install(phy,mac, staHackedNodes);

  mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
  NetDeviceContainer apDevices;
  apDevices = wifi.Install(phy,mac,apNodes);

  // Positioning
  // For part three, the positions are in a circle with winding: A B C D X Y
  // With TxGain:-3.0, falloff is around 40-60 meters
  double d= 9.0; // meters
  MobilityHelper mobility;
  // AP
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add(Vector(0.0,0.0,0.0)); // ap
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(apNodes);
  // Standard STA
  positionAlloc = CreateObject<ListPositionAllocator> ();
   // A B C D
  for (int ii = 0; ii < 4; ii++) {
    double theta = 2.0*M_PI*((double)ii)/6.0;
    positionAlloc->Add(Vector( cos(theta)*d , sin(theta)*d ,0.0));
  }
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(staNodes);
  // Hacked STA
  positionAlloc = CreateObject<ListPositionAllocator> ();
   // X Y
  for (int ii = 4; ii < 6; ii++) {
    double theta = 2.0*M_PI*((double)ii)/6.0;
    positionAlloc->Add(Vector( cos(theta)*d , sin(theta)*d ,0.0));
  }
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(staHackedNodes);

  // Protocol stack
  NS_LOG_INFO("Stack.");
  InternetStackHelper stack;
  stack.Install(staNodes);
  stack.Install(staHackedNodes);
  stack.Install(apNodes);

  Ipv4AddressHelper addr;
  addr.SetBase("10.0.0.0", "255.255.255.0");
  addr.Assign(staDevices);
  addr.Assign(staHackedDevices);
  addr.Assign(apDevices);

  int port = 12345;
  // A -> B
  OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.2"), port)); // to node 1
  onoff.SetAttribute ("PacketSize", UintegerValue (2000));
  onoff.SetConstantRate(DataRate( std::to_string(cbr_12) + "kb/s"));
  ApplicationContainer apps;
  apps.Add(onoff.Install(staNodes.Get(0))); // from node 0
  // C -> D
  OnOffHelper onoff2("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.4"), port)); // to node 1
  onoff2.SetAttribute ("PacketSize", UintegerValue (2000));
  onoff2.SetConstantRate(DataRate( std::to_string(cbr_12) + "kb/s"));
  apps.Add(onoff2.Install(staNodes.Get(2))); // from node 2
  // X -> Y
  OnOffHelper onoff3("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.5"), port)); // to node 1
  onoff3.SetAttribute ("PacketSize", UintegerValue (2000));
  onoff3.SetConstantRate(DataRate( std::to_string(cbr_3) + "kb/s"));
  apps.Add(onoff3.Install(staHackedNodes.Get(0))); // from node 0


  NS_LOG_INFO("Sock Sink.");
  // CBR 1
  PacketSinkHelper sink = PacketSinkHelper("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.2"), port));
  apps.Add( sink.Install(staNodes.Get(1)) );
  // CBR 2
  PacketSinkHelper sink2 = PacketSinkHelper("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.4"), port));
  apps.Add( sink2.Install(staNodes.Get(3)) );
  // CBR 3
  PacketSinkHelper sink3 = PacketSinkHelper("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.6"), port));
  apps.Add( sink3.Install(staHackedNodes.Get(1)) );

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

  // cbr 1,2
#define S 5 // seconds
#define M 900 // freq
#define L 3 // num loops

  // cbr 3
#define MM 100 // freq
#define LL 25 // num loops

  for (int ii = 1; ii <= L; ii++) {
    for ( int jj = 1; jj < LL; jj++ ) {
      //std::string fn = "./three/" + std::to_string(ii*M) + "/" + std::to_string(jj*MM);
      std::string fn = "three/" + std::to_string(ii*M) + "_" + std::to_string(jj*MM);
      //boost::filesystem::path dir(fn);
      //boost::filesystem::create_directory(dir);
      run_experiment_3(ii*M, jj*MM, S, fn);
    }
  }
}


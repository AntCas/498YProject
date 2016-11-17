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

#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("PartOne");

static int g_cbr = 500;
static bool g_do_rts = false;
static int g_seconds = 10;


static int num_pkts = 0;
static void udp_rx(std::string path, Ptr<const Packet> p, const Address &addr) {
  //std::cout << p->GetSize() << std::endl;
  Time t (Simulator::Now());
  std::cout << num_pkts << "^" << t.As(Time::MS) << std::endl;
  num_pkts++;
}


int main(int argc, char** argv) {
  CommandLine cmd;
  cmd.AddValue("cbr", "Constant Bit Rate (kb/s)", g_cbr);
  cmd.AddValue("do_rts", "Enable RTS/CTS", g_do_rts);
  cmd.AddValue("seconds", "How long to run simulation", g_seconds);
  cmd.Parse(argc, argv);

  printf(" **** \n   Running for %d seconds with %dkb/s cbr and %s \n ****\n\n",
    g_seconds, g_cbr,
    g_do_rts ? "RTS enabled" : "RTS disabled");


  std::cout << "Main starting.\n";
  Time::SetResolution(Time::NS);
  
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
  if (g_do_rts)
    Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("0"));
  else
    Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("999999999"));


  /*
  NodeContainer nodes;
  nodes.Create(2);

  PointToPointHelper ptp;
  ptp.setDeviceAttribute("DataRate", StringValue("5Mbps"));
  ptp.setChannelAttribute("Delay", StringValue("2ms"));

  NetDeviceContainer devices;
  devices = ptp.Install(nodes);
  */

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

  //WifiHelper wifi = WifiHelper::Default();
  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
    "DataMode",StringValue ("DsssRate2Mbps"), 
    "ControlMode",StringValue ("DsssRate1Mbps"));
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
  MobilityHelper mobility;
  mobility.SetPositionAllocator("ns3::GridPositionAllocator",
    "MinX", DoubleValue (0.0),
    "MinY", DoubleValue (0.0),
    "DeltaX", DoubleValue (10.0),
    "DeltaY", DoubleValue (10.0),
    "GridWidth", UintegerValue (2),
    "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(staNodes);
  mobility.SetPositionAllocator("ns3::GridPositionAllocator",
    "MinX", DoubleValue (5.0),
    "MinY", DoubleValue (5.0),
    "DeltaX", DoubleValue (10.0),
    "DeltaY", DoubleValue (10.0),
    "GridWidth", UintegerValue (1),
    "LayoutType", StringValue ("RowFirst"));
  mobility.Install(apNodes);

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
  OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.2"), port)); // to node 1
  onoff.SetAttribute ("PacketSize", UintegerValue (2000));
  onoff.SetConstantRate(DataRate( std::to_string(g_cbr) + "kb/s"));
  ApplicationContainer apps = onoff.Install(staNodes.Get(0)); // from node 0
  apps.Start(Seconds(0.0));
  apps.Stop(Seconds((int)g_seconds));

  Simulator::Stop(Seconds((int)g_seconds + 5));

  NS_LOG_INFO("Sock Sink.");
  PacketSinkHelper sink = PacketSinkHelper("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.2"), port));
  apps.Add( sink.Install(staNodes.Get(1)) );
  apps.Start(Seconds(0.0));
  apps.Stop(Seconds((int)g_seconds));

  /*
  PacketSocketHelper psh;
  psh.Install(staNodes);
  psh.Install(apNodes);

  NS_LOG_INFO("Sock.");
  PacketSocketAddress sock;
  sock.SetSingleDevice(staDevices.Get(0)->GetIfIndex());
  sock.SetPhysicalAddress(staDevices.Get(1)->GetAddress());
  sock.SetProtocol(17);

  OnOffHelper onoff("ns3::UdpSocketFactory", Address(sock));
  onoff.SetAttribute ("PacketSize", UintegerValue (2000));
  onoff.SetConstantRate(DataRate("500kb/s"));
  ApplicationContainer apps = onoff.Install(staNodes.Get(0));
  apps.Start(Seconds(1.0));
  apps.Stop(Seconds(5.0));

  Simulator::Stop(Seconds(10.0));

  NS_LOG_INFO("Sock Sink.");
  //PacketSinkHelper sink = PacketSinkHelper("ns3::UdpSocketFactory", sock);
  //apps = sink.Install(staNodes.Get(1));
  apps.Start(Seconds(0.0));
  apps.Stop(Seconds(10.0));
  */

  Config::Connect("/NodeList/*/ApplicationList/*/$ns3::PacketSink/Rx", MakeCallback(&udp_rx));

  Simulator::Run();
  Simulator::Destroy();

  NS_LOG_INFO("Done!");
}


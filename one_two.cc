#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/athstats-helper.h"

#include "ns3/data-rate.h"

#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("PartOne");

static void udp_rx(std::string path, Ptr<const Packet> p, const Address &addr) {
  std::cout << p->GetSize() << std::endl;
}

int main() {
  Time::SetResolution(Time::NS);

  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

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
  NodeContainer staNodes;
  NodeContainer apNodes;
  staNodes.Create(4);
  apNodes.Create(1);

  YansWifiChannelHelper chan; 
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
  phy.SetChannel(channel.Create());

  WifiHelper wifi = WifiHelper::Default();
  wifi.SetRemoteStationManager("ns3::AarfWifiManager");
  NqosWifiMacHelper mac = NQosWifiMacHelper::Default();

  Ssid ssid = Ssid("tst");
  mac.SetType("ns3::NqstaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install(phy,mac, staNodes);

  // AP
  mac.SetType("ns3::NqapWifiMac", "Ssid", SsidValue(ssid),
      "BeaconGeneration", BooleanValue(true),
      "BeaconInterval", TimeValue(Seconds(2.5));
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
  InternetStackHelper stack;
  stack.Install(staNodes);
  stack.Install(apNodes);

  Ipv4AddressHelper addr;
  addr.setBase("10.1.1.0", "255.255.255.0");
  addr.Assign(staDevices);
  addr.Assign(apDevices);


  /*
  // Apps
  UdpEchoServerHelper echoServer(9);
  ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
  serverApps.Start(Seconds(1.0));
  serverApps.Stop(Seconds(10.0));
  */
  PacketSocketAddress sock;
  sock.SetPhysicalAddress(staDevices.Get(0)->GetAddress());
  sock.SetSingleDevice(staDevices.Get(1)->GetIfIndex());
  sock.setProtocol(17);
  OnOffHelper onoff("ns3::PacketSocketFactory", Address(socket));
  onoff.SetConstantRate(DataRate("500kb/s"));
  ApplicationContainer apps = onoff.Install(staNodes.get(0));
  apps.Start(Seconds(1.0));
  apps.Stop(Seconds(10.0));

  PacketSinkHelper sink = PacketSinkHelper("ns3::PacketSocketFactory", socket);
  apps = sink.Install(nodes.Get(1));
  apps.Start(Seconds(0.0));
  apps.Stop(Seconds(20.0));

  Config::Connect("/NodeList/*/ApplicationList/*/$ns3::PacketSink/Rx", MakeCallback(&udp_rx));


}


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

int main() {
  Time::SetResolution(Time::NS);

  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create(2);

  PointToPointHelper ptp;
  ptp.setDeviceAttribute("DataRate", StringValue("5Mbps"));
  ptp.setChannelAttribute("Delay", StringValue("2ms"));

  NetDeviceContainer devices;
  devices = ptp.Install(nodes);

  // Protocol stack
  InternetStackHelper stack;
  stack.Install(nodes);

  Ipv4AddressHelper addr;
  addr.setBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = addr.Assign(devices);

  // Apps
  UdpEchoServerHelper echoServer(9);
  ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
  serverApps.Start(Seconds(1.0));
  serverApps.Stop(Seconds(10.0));



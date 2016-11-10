/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
// Default Network Topology
//
// Wifi 10.1.1.0
// AP
// * *
// | |
// n1 n0
//
//Consider a Wifi access point (AP) and a base station (STA), which are both static.
//The STA can communicate with the AP only when it is within a certain distance
//from the AP. Beyond that range, the two nodes can't receive each others signals.
// 
// Given below is a code to simulate the said scenario with ns3.
// STA sends a packet to the AP; AP echoes it back to the base station.
// The AP is located at position (x, y) = (0, 0). STA is at (xDistance, 0)
// (all distances in meter). Default value of xDistance is set to 10. [Lines #76, #131]
//  
//  Increase the value of xDistance in the code and find out the maximum distance upto which two way communication is possible. This can be verified from the output of the code, which will show the STA has received reply from the AP (indicated by IP address).
// Node #0 is the AP, #1 is a base station
// #1 sends UDP echo mesg to the AP; AP sends a UDP response back to the node
// Communication is possible only when the station is within a certain distance from the AP

// Mobility model is used for calculating propagation loss and propagation delay.
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("Wifi-2-nodes-fixed");
void PrintLocations (NodeContainer nodes, std::string header) {
    std::cout << header << std::endl;
    for (NodeContainer::Iterator iNode = nodes.Begin (); iNode != nodes.End (); ++iNode) {
        Ptr<Node> object = *iNode;
        Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
        NS_ASSERT (position != 0);
        Vector pos = position->GetPosition ();
        std::cout << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
    }
    std::cout << std::endl;
}

void PrintAddresses(Ipv4InterfaceContainer container, std::string header) {
    std::cout << header << std::endl;
    uint32_t nNodes = container.GetN ();
    for (uint32_t i = 0; i < nNodes; ++i) {
        std::cout << container.GetAddress(i, 0) << std::endl;
    }
    std::cout << std::endl;
}

int main (int argc, char *argv[]) {
    bool verbose = true;
    uint32_t nWifi = 4;
    /** Change this parameter and verify the output */
    double xDistance = 10;
    
    CommandLine cmd;
    cmd.AddValue ("xDistance", "Distance between two nodes along x-axis", xDistance);
	std::cout << "xDistance " << xDistance << std::endl; 
    
    cmd.Parse (argc,argv);
    if (verbose) {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }
    // 1. Create the nodes and hold them in a container
    NodeContainer wifiStaNodes,
    wifiApNode;
    
    wifiStaNodes.Create (nWifi);
    wifiApNode.Create(1);
    // 2. Create channel for communication
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
    phy.SetChannel (channel.Create ());
    WifiHelper wifi;
	wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
    wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
    
    NqosWifiMacHelper mac = NqosWifiMacHelper::Default ();
    // 3a. Set up MAC for base stations
    Ssid ssid = Ssid ("ns-3-ssid");
    mac.SetType ("ns3::StaWifiMac",
                 "Ssid", SsidValue (ssid),
                 "ActiveProbing", BooleanValue (false));
    NetDeviceContainer staDevices;
    staDevices = wifi.Install (phy, mac, wifiStaNodes);
    // 3b. Set up MAC for AP
    mac.SetType ("ns3::ApWifiMac",
                 "Ssid", SsidValue (ssid),
                 "BeaconGeneration", BooleanValue (true),
                 "BeaconInterval", TimeValue (Seconds (5)));
    NetDeviceContainer apDevice;
    apDevice = wifi.Install (phy, mac, wifiApNode);
    // 4. Set mobility of the nodes
    MobilityHelper mobility;
    
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
    positionAlloc->Add (Vector (0.0, 0.0, 0.0));
    positionAlloc->Add (Vector (100.0, 0.0, 0.0));
    positionAlloc->Add (Vector (-100.0, 0.0, 0.0));
    positionAlloc->Add (Vector (0.0, 100.0, 0.0));
    positionAlloc->Add (Vector (0.0, -100.0, 0.0));
    mobility.SetPositionAllocator (positionAlloc);
    
    
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (wifiApNode);
    mobility.Install (wifiStaNodes);

    // 5.Add Internet layers stack
    InternetStackHelper stack;
    stack.Install (wifiStaNodes);
    stack.Install (wifiApNode);
    // 6. Assign IP address to each device
    Ipv4AddressHelper address;
    Ipv4InterfaceContainer wifiInterfaces;
    address.SetBase ("10.1.1.0", "255.255.255.0");
    
    wifiInterfaces = address.Assign(staDevices);
    address.Assign(apDevice);
    // 7a. Create and setup applications (traffic sink)
    UdpEchoServerHelper echoServer (9); // Port # 9
    ApplicationContainer serverApps = echoServer.Install (wifiStaNodes.Get(1));
    serverApps.Start (Seconds (1.0));
    serverApps.Stop (Seconds (4.0));
   
    UdpEchoServerHelper echoServer1 (10); // Port # 10
    ApplicationContainer serverApps1 = echoServer1.Install (wifiStaNodes.Get(3));
    serverApps1.Start (Seconds (6.0));
    serverApps1.Stop (Seconds (9.0));

    // 7b. Create and setup applications (traffic source)
    UdpEchoClientHelper echoClient (wifiInterfaces.GetAddress (1), 9);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
    
    UdpEchoClientHelper echoClient1 (wifiInterfaces.GetAddress (3), 10);
    echoClient1.SetAttribute ("MaxPackets", UintegerValue (1));
    echoClient1.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
    echoClient1.SetAttribute ("PacketSize", UintegerValue (1024));
    
    ApplicationContainer clientApps = echoClient.Install (wifiStaNodes.Get (0)); 
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (3.0));
    
    ApplicationContainer clientApps1 = echoClient1.Install (wifiStaNodes.Get (2)); 
    clientApps1.Start (Seconds (7.0));
    clientApps1.Stop (Seconds (8.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    Simulator::Stop (Seconds (10.0));
    // 8. Enable tracing (optional)
    //phy.EnablePcapAll ("wifi-2-nodes-fixed", true);
    
    PrintAddresses(wifiInterfaces, "IP addresses of base stations");
    PrintLocations(wifiStaNodes, "Location of all nodes");
    
    Simulator::Run ();
    Simulator::Destroy ();
    
    return 0;
}


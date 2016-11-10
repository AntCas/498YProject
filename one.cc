/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006,2007 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/athstats-helper.h"

#include <iostream>

using namespace ns3;

static bool g_verbose = true;

/*
void
DevTxTrace (std::string context, Ptr<const Packet> p)
{
  if (g_verbose)
    {
      std::cout << " TX p: " << *p << std::endl;
    }
}
void
DevRxTrace (std::string context, Ptr<const Packet> p)
{
  if (g_verbose)
    {
      std::cout << " RX p: " << *p << std::endl;
    }
}
void
PhyRxOkTrace (std::string context, Ptr<const Packet> packet, double snr, WifiMode mode, enum WifiPreamble preamble)
{
  if (g_verbose)
    {
      std::cout << "PHYRXOK mode=" << mode << " snr=" << snr << " " << *packet << std::endl;
    }
}
void
PhyRxErrorTrace (std::string context, Ptr<const Packet> packet, double snr)
{
  if (g_verbose)
    {
      std::cout << "PHYRXERROR snr=" << snr << " " << *packet << std::endl;
    }
}
void
PhyTxTrace (std::string context, Ptr<const Packet> packet, WifiMode mode, WifiPreamble preamble, uint8_t txPower)
{
  if (g_verbose)
    {
      std::cout << "PHYTX mode=" << mode << " " << *packet << std::endl;
    }
}
void
PhyStateTrace (std::string context, Time start, Time duration, enum WifiPhy::State state)
{
  if (g_verbose)
    {
      std::cout << " state=" << state << " start=" << start << " duration=" << duration << std::endl;
    }
}

static void
SetPosition (Ptr<Node> node, Vector position)
{
  Ptr<MobilityModel> mobility = node->GetObject<MobilityModel> ();
  mobility->SetPosition (position);
}


static void 
AdvancePosition (Ptr<Node> node) 
{
  Vector pos = GetPosition (node);
  pos.x += 5.0;
  if (pos.x >= 210.0) 
    {
      return;
    }
  SetPosition (node, pos);

  if (g_verbose)
    {
      //std::cout << "x="<<pos.x << std::endl;
    }
  Simulator::Schedule (Seconds (1.0), &AdvancePosition, node);
}
*/

#define PERIOD 1.0

/*
static Vector
GetPosition (Ptr<Node> node) {
  Ptr<MobilityModel> mobility = node->GetObject<MobilityModel> ();
  return mobility->GetPosition ();
}
*/

static void Profile(Ptr<Node> node) {
  //Vector pos = GetPosition(node);
  //pos;

  Simulator::Schedule (Seconds (PERIOD), &Profile, node);

  //std::cout << "TIMING: " << 
}


static int num_pkts_recvd = 0;
void DevRxTrace (std::string context, Ptr<const Packet> p)
{
  Time t = Simulator::Now();
  std::cout << "n:" << num_pkts_recvd << " at " << t.As(Time::MS) << std::endl;
  num_pkts_recvd++;
}

int main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.AddValue ("verbose", "Print trace information if true", g_verbose);

  cmd.Parse (argc, argv);

  Packet::EnablePrinting ();

  // enable rts cts NEVER.
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("9999999"));
  // disable fragmentation
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));

  WifiHelper wifi;
  MobilityHelper mobility;
  NodeContainer stas;
  NodeContainer ap;
  NetDeviceContainer staDevs;
  PacketSocketHelper packetSocket;

  stas.Create (4);
  ap.Create (1);

  // give packet socket powers to nodes.
  packetSocket.Install (stas);
  packetSocket.Install (ap);

  WifiMacHelper wifiMac;
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());
  Ssid ssid = Ssid ("wifi-default");
  wifi.SetRemoteStationManager ("ns3::ArfWifiManager");
  // setup stas.
  wifiMac.SetType ("ns3::StaWifiMac",
                   "Ssid", SsidValue (ssid));
  staDevs = wifi.Install (wifiPhy, wifiMac, stas);
  // setup ap.
  wifiMac.SetType ("ns3::ApWifiMac",
                   "Ssid", SsidValue (ssid));
  wifi.Install (wifiPhy, wifiMac, ap);

  // mobility.
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
      "MinX", DoubleValue (0.0),
      "MinY", DoubleValue (0.0),
      "DeltaX", DoubleValue (10.0),
      "DeltaY", DoubleValue (10.0),
      "GridWidth", UintegerValue (2),
      "LayoutType", StringValue ("RowFirst"));
  mobility.Install (stas);
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
      "MinX", DoubleValue (5.0),
      "MinY", DoubleValue (5.0),
      "DeltaX", DoubleValue (10.0),
      "DeltaY", DoubleValue (10.0),
      "GridWidth", UintegerValue (2),
      "LayoutType", StringValue ("RowFirst"));
  mobility.Install (ap);

  ////Simulator::Schedule (Seconds (1.0), &AdvancePosition, ap.Get (0));
  //Simulator::Schedule (Seconds (PERIOD), &Profile, ap.Get (0));

  PacketSocketAddress socket;
  socket.SetSingleDevice (staDevs.Get (0)->GetIfIndex ());
  socket.SetPhysicalAddress (staDevs.Get (1)->GetAddress ());
  socket.SetProtocol (17);

  OnOffHelper onoff ("ns3::PacketSocketFactory", Address (socket));
  onoff.SetConstantRate (DataRate ("100kb/s"));

  ApplicationContainer apps = onoff.Install (stas.Get (0));
  apps.Start (Seconds (0.1));
  apps.Stop (Seconds (10.0));

  Simulator::Stop (Seconds (4.0));

  //Config::Connect ("/NodeList/*/DeviceList/*/Mac/MacTx", MakeCallback (&DevTxTrace));
  //Config::Connect ("/NodeList/*/DeviceList/*/Mac/MacRx", MakeCallback (&DevRxTrace));
  //Config::Connect ("/NodeList/*/DeviceList/*/Phy/State/RxOk", MakeCallback (&PhyRxOkTrace));
  //Config::Connect ("/NodeList/*/DeviceList/*/Phy/State/RxError", MakeCallback (&PhyRxErrorTrace));
  //Config::Connect ("/NodeList/*/DeviceList/*/Phy/State/Tx", MakeCallback (&PhyTxTrace));
  //Config::Connect ("/NodeList/*/DeviceList/*/Phy/State/State", MakeCallback (&PhyStateTrace));

  AthstatsHelper athstats;
  athstats.EnableAthstats ("athstats-sta", stas);
  athstats.EnableAthstats ("athstats-ap", ap);

  Simulator::Run ();

  Simulator::Destroy ();

  return 0;
}

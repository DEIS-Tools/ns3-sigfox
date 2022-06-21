/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 University of Padova
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
 * Author: Davide Magrin <davide@magr.in>
 */

#include "ns3/sigfox-helper.h"
#include "ns3/log.h"

#include <fstream>

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("SigfoxHelper");

  SigfoxHelper::SigfoxHelper () :
    m_lastPhyPerformanceUpdate (Seconds (0)),
    m_lastGlobalPerformanceUpdate (Seconds (0))
  {
  }

  SigfoxHelper::~SigfoxHelper ()
  {
  }

  NetDeviceContainer
  SigfoxHelper::Install ( const SigfoxPhyHelper &phyHelper,
                        const SigfoxMacHelper &macHelper,
                        NodeContainer c) const
  {
    NS_LOG_FUNCTION_NOARGS ();

    NetDeviceContainer devices;

    // Go over the various nodes in which to install the NetDevice
    for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
      {
        Ptr<Node> node = *i;

        // Create the SigfoxNetDevice
        Ptr<SigfoxNetDevice> device = CreateObject<SigfoxNetDevice> ();

        // Create the PHY
        Ptr<SigfoxPhy> phy = phyHelper.Create (node, device);
        NS_ASSERT (phy != 0);
        device->SetPhy (phy);
        NS_LOG_DEBUG ("Done creating the PHY");

      // Create the MAC
      Ptr<SigfoxMac> mac = macHelper.Create (node, device);
      NS_ASSERT (mac != 0);
      mac->SetPhy (phy);
      NS_LOG_DEBUG ("Done creating the MAC");
      device->SetMac (mac);

      node->AddDevice (device);
      devices.Add (device);
      NS_LOG_DEBUG ("node=" << node << ", mob=" << node->GetObject<MobilityModel> ()->GetPosition ());
    }
  return devices;
}

NetDeviceContainer
SigfoxHelper::Install ( const SigfoxPhyHelper &phy,
                      const SigfoxMacHelper &mac,
                      Ptr<Node> node) const
{
  return Install (phy, mac, NodeContainer (node));
}

void
SigfoxHelper::EnableSimulationTimePrinting (Time interval)
{
  m_oldtime = std::time (0);
  Simulator::Schedule (Seconds (0), &SigfoxHelper::DoPrintSimulationTime, this,
                       interval);
}

void
SigfoxHelper::EnablePeriodicDeviceStatusPrinting (NodeContainer endDevices,
                                                NodeContainer gateways,
                                                std::string filename,
                                                Time interval)
{
  NS_LOG_FUNCTION (this);

  DoPrintDeviceStatus (endDevices, gateways, filename);

  // Schedule periodic printing
  Simulator::Schedule (interval,
                       &SigfoxHelper::EnablePeriodicDeviceStatusPrinting, this,
                       endDevices, gateways, filename, interval);
}

void
SigfoxHelper::DoPrintDeviceStatus (NodeContainer endDevices, NodeContainer gateways,
                                 std::string filename)
{
  const char * c = filename.c_str ();
  std::ofstream outputFile;
  if (Simulator::Now () == Seconds (0))
    {
      // Delete contents of the file as it is opened
      outputFile.open (c, std::ofstream::out | std::ofstream::trunc);
    }
  else
    {
      // Only append to the file
      outputFile.open (c, std::ofstream::out | std::ofstream::app);
    }

  Time currentTime = Simulator::Now();
  for (NodeContainer::Iterator j = endDevices.Begin (); j != endDevices.End (); ++j)
    {
      Ptr<Node> object = *j;
      Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
      NS_ASSERT (position != 0);
      Ptr<NetDevice> netDevice = object->GetDevice (0);
      Ptr<SigfoxNetDevice> sigfoxNetDevice = netDevice->GetObject<SigfoxNetDevice> ();
      NS_ASSERT (sigfoxNetDevice != 0);
      Ptr<EndPointSigfoxMac> mac = sigfoxNetDevice->GetMac ()->GetObject<EndPointSigfoxMac> ();
      double txPower = mac->GetTransmissionPower ();
      Vector pos = position->GetPosition ();
      outputFile << currentTime.GetSeconds () << " "
                 << object->GetId () <<  " "
                 << pos.x << " " << pos.y << " "
                 << unsigned(txPower) << std::endl;
    }
  // for (NodeContainer::Iterator j = gateways.Begin (); j != gateways.End (); ++j)
  //   {
  //     Ptr<Node> object = *j;
  //     Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
  //     Vector pos = position->GetPosition ();
  //     outputFile << currentTime.GetSeconds () << " "
  //                << object->GetId () <<  " "
  //                << pos.x << " " << pos.y << " " << "-1 -1" << std::endl;
  //   }
  outputFile.close ();
}


void
SigfoxHelper::EnablePeriodicPhyPerformancePrinting (NodeContainer gateways,
                                                  std::string filename,
                                                  Time interval)
{
  NS_LOG_FUNCTION (this);

  DoPrintPhyPerformance (gateways, filename);

  Simulator::Schedule (interval,
                       &SigfoxHelper::EnablePeriodicPhyPerformancePrinting,
                       this,
                       gateways, filename, interval);
}

void
SigfoxHelper::DoPrintPhyPerformance (NodeContainer gateways,
                                   std::string filename)
{
  NS_LOG_FUNCTION (this);

  const char * c = filename.c_str ();
  std::ofstream outputFile;
  if (Simulator::Now () == Seconds (0))
    {
      // Delete contents of the file as it is opened
      outputFile.open (c, std::ofstream::out | std::ofstream::trunc);
    }
  else
    {
      // Only append to the file
      outputFile.open (c, std::ofstream::out | std::ofstream::app);
    }

  for (auto it = gateways.Begin (); it != gateways.End (); ++it)
    {
      int systemId = (*it)->GetId ();
      outputFile << Simulator::Now ().GetSeconds () << " " <<
        std::to_string(systemId) << std::endl;
    }

  m_lastPhyPerformanceUpdate = Simulator::Now ();

  outputFile.close();
}

void
SigfoxHelper::EnablePeriodicGlobalPerformancePrinting (std::string filename,
                                                     Time interval)
{
  NS_LOG_FUNCTION (this << filename << interval);

  DoPrintGlobalPerformance (filename);

  Simulator::Schedule (interval,
                       &SigfoxHelper::EnablePeriodicGlobalPerformancePrinting,
                       this,
                       filename, interval);
}

void
SigfoxHelper::DoPrintGlobalPerformance (std::string filename)
{
  NS_LOG_FUNCTION (this);

  const char * c = filename.c_str ();
  std::ofstream outputFile;
  if (Simulator::Now () == Seconds (0))
    {
      // Delete contents of the file as it is opened
      outputFile.open (c, std::ofstream::out | std::ofstream::trunc);
    }
  else
    {
      // Only append to the file
      outputFile.open (c, std::ofstream::out | std::ofstream::app);
    }

  outputFile << Simulator::Now ().GetSeconds () << std::endl;

  m_lastGlobalPerformanceUpdate = Simulator::Now ();

  outputFile.close();
}

void
SigfoxHelper::DoPrintSimulationTime (Time interval)
{
  // NS_LOG_INFO ("Time: " << Simulator::Now().GetHours());
  std::cout << "Simulated time: " << Simulator::Now ().GetHours () << " hours" << std::endl;
  std::cout << "Real time from last call: " << std::time (0) - m_oldtime << " seconds" << std::endl;
  m_oldtime = std::time (0);
  Simulator::Schedule (interval, &SigfoxHelper::DoPrintSimulationTime, this, interval);
}

}
}

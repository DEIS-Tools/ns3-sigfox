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
 *         Muhammad Naeem <mnaeem@cs.aau.dk>
 *
 * Modified by: Michele Albano <mialb@cs.aau.dk>
 */

#ifndef SIGFOX_HELPER_H
#define SIGFOX_HELPER_H

#include "ns3/sigfox-phy-helper.h"
#include "ns3/sigfox-mac-helper.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/net-device.h"
#include "ns3/sigfox-net-device.h"

#include <ctime>

namespace ns3 {
namespace sigfox {

/**
 * Helps to create SigfoxNetDevice objects
 *
 * This class can help create a large set of similar SigfoxNetDevice objects and
 * configure a large set of their attributes during creation.
 */
class SigfoxHelper
{
public:
  virtual ~SigfoxHelper ();

  SigfoxHelper ();

  /**
   * Install SigfoxNetDevices on a list of nodes
   *
   * \param phy the PHY helper to create PHY objects
   * \param mac the MAC helper to create MAC objects
   * \param c the set of nodes on which a sigfox device must be created
   * \returns a device container which contains all the devices created by this
   * method.
   */
  virtual NetDeviceContainer Install (const SigfoxPhyHelper &phyHelper,
                                      const SigfoxMacHelper &macHelper,
                                      NodeContainer c) const;

  /**
   * Install SigfoxNetDevice on a single node
   *
   * \param phy the PHY helper to create PHY objects
   * \param mac the MAC helper to create MAC objects
   * \param node the node on which a sigfox device must be created
   * \returns a device container which contains all the devices created by this
   * method.
   */
  virtual NetDeviceContainer Install (const SigfoxPhyHelper &phyHelper,
                                      const SigfoxMacHelper &macHelper,
                                      Ptr<Node> node) const;

  /**
   * Periodically prints the simulation time to the standard output.
   */
  void EnableSimulationTimePrinting (Time interval);

  /**
   * Periodically prints the status of devices in the network to a file.
   */
  void EnablePeriodicDeviceStatusPrinting (NodeContainer endDevices,
                                           NodeContainer gateways,
                                           std::string filename,
                                           Time interval);

  /**
   * Periodically prints PHY-level performance at every gateway in the container.
   */
  void EnablePeriodicPhyPerformancePrinting (NodeContainer gateways,
                                             std::string filename,
                                             Time interval);

  void DoPrintPhyPerformance (NodeContainer gateways, std::string filename);

  /**
   * Periodically prints global performance.
   */
  void EnablePeriodicGlobalPerformancePrinting (std::string filename,
                                                Time interval);

  void DoPrintGlobalPerformance (std::string filename);

  time_t m_oldtime;

  /**
   * Print a summary of the status of all devices in the network.
   */
  void DoPrintDeviceStatus (NodeContainer endDevices, NodeContainer gateways,
                            std::string filename);

private:
  /**
   * Actually print the simulation time and re-schedule execution of this
   * function.
   */
  void DoPrintSimulationTime (Time interval);

  Time m_lastPhyPerformanceUpdate;
  Time m_lastGlobalPerformanceUpdate;
};

} //namespace ns3

}
#endif /* SIGFOX_HELPER_H */

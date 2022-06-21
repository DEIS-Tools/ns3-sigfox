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

#ifndef SIGFOX_MAC_HELPER_H
#define SIGFOX_MAC_HELPER_H

#include "ns3/net-device.h"
#include "ns3/sigfox-channel.h"
#include "ns3/sigfox-phy.h"
#include "ns3/end-point-sigfox-mac.h"
#include "ns3/sigfox-mac.h"
#include "ns3/gateway-sigfox-mac.h"
#include "ns3/node-container.h"
#include "ns3/random-variable-stream.h"

namespace ns3 {
namespace sigfox {

class SigfoxMacHelper
{
public:
  /**
   * Define the kind of device. Can be either GW (Gateway) or EP (End Point).
   */
  enum DeviceType { GW, EP };

  /**
   * Define the operational region.
   */
  enum Regions { EU };

  /**
   * Create a mac helper without any parameter set. The user must set
   * them all to be able to call Install later.
   */
  SigfoxMacHelper ();

  /**
   * Set an attribute of the underlying MAC object.
   *
   * \param name the name of the attribute to set.
   * \param v the value of the attribute.
   */
  void Set (std::string name, const AttributeValue &v);

  /**
   * Set the kind of MAC this helper will create.
   *
   * \param dt the device type (either gateway or end device).
   */
  void SetDeviceType (enum DeviceType dt);

  /**
   * Set the region in which the device is to operate.
   */
  void SetRegion (enum Regions region);

  /**
   * Create the SigfoxMac instance and connect it to a device
   *
   * \param node the node on which we wish to create a wifi MAC.
   * \param device the device within which this MAC will be created.
   * \returns a newly-created SigfoxMac object.
   */
  Ptr<SigfoxMac> Create (Ptr<Node> node, Ptr<NetDevice> device) const;

private:
  ObjectFactory m_mac;
  enum DeviceType m_deviceType; //!< The kind of device to install
  enum Regions m_region; //!< The region in which the device will operate
};

} // namespace sigfox

} // namespace ns3
#endif /* SIGFOX_PHY_HELPER_H */

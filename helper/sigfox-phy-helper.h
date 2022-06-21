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

#ifndef SIGFOX_PHY_HELPER_H
#define SIGFOX_PHY_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/net-device.h"
#include "ns3/sigfox-channel.h"
#include "ns3/sigfox-phy.h"
#include "ns3/simple-end-point-sigfox-phy.h"
#include "ns3/simple-gateway-sigfox-phy.h"
#include "ns3/sigfox-mac.h"

namespace ns3 {
namespace sigfox {

/**
 * Helper to install SigfoxPhy instances on multiple Nodes.
 */
class SigfoxPhyHelper
{
public:
  /**
   * Enum for the type of device: End Device (ED) or Gateway (GW)
   */
  enum DeviceType
  {
    GW,
    EP
  };

  /**
   * Create a phy helper without any parameter set. The user must set
   * them all to be able to call Install later.
   */
  SigfoxPhyHelper ();

  /**
   * Set the SigfoxChannel to connect the PHYs to.
   *
   * Every PHY created by a call to Install is associated to this channel.
   *
   * \param channel the channel to associate to this helper.
   */
  void SetChannel (Ptr<SigfoxChannel> channel);

  /**
   * Set the kind of PHY this helper will create.
   *
   * \param dt the device type.
   */
  void SetDeviceType (enum DeviceType dt);

  TypeId GetDeviceType (void) const;

  /**
   * Set an attribute of the underlying PHY object.
   *
   * \param name the name of the attribute to set.
   * \param v the value of the attribute.
   */
  void Set (std::string name, const AttributeValue &v);

  /**
   * Crate a SigfoxPhy and connect it to a device on a node.
   *
   * \param node the node on which we wish to create a wifi PHY.
   * \param device the device within which this PHY will be created.
   * \return a newly-created PHY object.
   */
  Ptr<SigfoxPhy> Create (Ptr<Node> node, Ptr<NetDevice> device) const;

private:
  /**
   * The PHY layer factory object.
   */
  ObjectFactory m_phy;

  /**
   * The channel instance the PHYs will be connected to.
   */
  Ptr<SigfoxChannel> m_channel;

};

}   //namespace ns3

}
#endif /* SIGFOX_PHY_HELPER_H */

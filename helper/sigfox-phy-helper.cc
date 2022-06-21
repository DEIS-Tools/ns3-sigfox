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

#include "ns3/sigfox-phy-helper.h"
#include "ns3/log.h"
#include "ns3/sub-band.h"

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("SigfoxPhyHelper");

SigfoxPhyHelper::SigfoxPhyHelper ()
{
  NS_LOG_FUNCTION (this);
}

void
SigfoxPhyHelper::SetChannel (Ptr<SigfoxChannel> channel)
{
  m_channel = channel;
}

void
SigfoxPhyHelper::SetDeviceType (enum DeviceType dt)
{

  NS_LOG_FUNCTION (this << dt);
  switch (dt)
    {
    case GW:
      m_phy.SetTypeId ("ns3::SimpleGatewaySigfoxPhy");
      break;
    case EP:
      m_phy.SetTypeId ("ns3::SimpleEndPointSigfoxPhy");
      break;
    }
}

TypeId
SigfoxPhyHelper::GetDeviceType (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phy.GetTypeId ();
}

void
SigfoxPhyHelper::Set (std::string name, const AttributeValue &v)
{
  m_phy.Set (name, v);
}

Ptr<SigfoxPhy>
SigfoxPhyHelper::Create (Ptr<Node> node, Ptr<NetDevice> device) const
{
  NS_LOG_FUNCTION (this << node->GetId () << device);

  // Create the PHY and set its channel
  Ptr<SigfoxPhy> phy = m_phy.Create<SigfoxPhy> ();
  phy->SetChannel (m_channel);

  // Configuration is different based on the kind of device we have to create
  std::string typeId = m_phy.GetTypeId ().GetName ();
  if (typeId == "ns3::SimpleGatewaySigfoxPhy")
    {
      // Inform the channel of the presence of this PHY
      m_channel->Add (phy);
    }
  else if (typeId == "ns3::SimpleEndPointSigfoxPhy")
    {
      // The line below can be commented to speed up uplink-only simulations.
      // This implies that the SigfoxChannel instance will only know about
      // Gateways, and it will not lose time delivering packets and interference
      // information to devices which will never listen.

      // m_channel->Add (phy);
    }

  // Link the PHY to its net device
  phy->SetDevice (device);

  return phy;
}

} // namespace sigfox
} // namespace ns3

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

#include "ns3/sigfox-mac-helper.h"
#include "ns3/end-point-sigfox-phy.h"
#include "ns3/gateway-sigfox-phy.h"
#include "ns3/end-point-sigfox-phy.h"
#include "ns3/sigfox-net-device.h"
#include "ns3/log.h"
#include "ns3/random-variable-stream.h"

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("SigfoxMacHelper");

SigfoxMacHelper::SigfoxMacHelper () : m_region (SigfoxMacHelper::EU)
{
}

void
SigfoxMacHelper::Set (std::string name, const AttributeValue &v)
{
  m_mac.Set (name, v);
}

void
SigfoxMacHelper::SetDeviceType (enum DeviceType dt)
{
  NS_LOG_FUNCTION (this << dt);
  switch (dt)
    {
    case GW:
      m_mac.SetTypeId ("ns3::GatewaySigfoxMac");
      break;
    case EP:
      m_mac.SetTypeId ("ns3::EndPointSigfoxMac");
      break;
    }
  m_deviceType = dt;
}

void
SigfoxMacHelper::SetRegion (enum SigfoxMacHelper::Regions region)
{
  m_region = region;
}

Ptr<SigfoxMac>
SigfoxMacHelper::Create (Ptr<Node> node, Ptr<NetDevice> device) const
{
  Ptr<SigfoxMac> mac = m_mac.Create<SigfoxMac> ();
  mac->SetDevice (device);

  LogicalSigfoxChannelHelper channelHelper;
  channelHelper.AddSubBand (868, 868.6, 0.01, 14);
  // double margins = 1.406e3 + 17.363e3;
  // channelHelper.SetFrequencyRange (868.034e6 + margins, 868.226e6 - margins);
  channelHelper.SetFrequencyRange (868.034e6, 868.034e6 + 192e3);
  mac->SetLogicalSigfoxChannelHelper (channelHelper);

  return mac;
}

} // namespace sigfox
} // namespace ns3

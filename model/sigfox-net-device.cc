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

#include "ns3/sigfox-net-device.h"
#include "ns3/pointer.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/abort.h"

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("SigfoxNetDevice");

NS_OBJECT_ENSURE_REGISTERED (SigfoxNetDevice);

TypeId
SigfoxNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SigfoxNetDevice")
    .SetParent<NetDevice> ()
    .AddConstructor<SigfoxNetDevice> ()
    .SetGroupName ("sigfox")
    .AddAttribute ("Channel", "The channel attached to this device",
                   PointerValue (),
                   MakePointerAccessor (&SigfoxNetDevice::DoGetChannel),
                   MakePointerChecker<SigfoxChannel> ())
    .AddAttribute ("Phy", "The PHY layer attached to this device.",
                   PointerValue (),
                   MakePointerAccessor (&SigfoxNetDevice::GetPhy,
                                        &SigfoxNetDevice::SetPhy),
                   MakePointerChecker<SigfoxPhy> ())
    .AddAttribute ("Mac", "The MAC layer attached to this device.",
                   PointerValue (),
                   MakePointerAccessor (&SigfoxNetDevice::GetMac,
                                        &SigfoxNetDevice::SetMac),
                   MakePointerChecker<SigfoxMac> ());
  return tid;
}

SigfoxNetDevice::SigfoxNetDevice () :
  m_node (0),
  m_phy (0),
  m_mac (0),
  m_configComplete (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

SigfoxNetDevice::~SigfoxNetDevice ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
SigfoxNetDevice::SetMac (Ptr<SigfoxMac> mac)
{
  m_mac = mac;
}

Ptr<SigfoxMac>
SigfoxNetDevice::GetMac (void) const
{
  return m_mac;
}

void
SigfoxNetDevice::SetPhy (Ptr<SigfoxPhy> phy)
{
  m_phy = phy;
}

Ptr<SigfoxPhy>
SigfoxNetDevice::GetPhy (void) const
{
  return m_phy;
}

void
SigfoxNetDevice::CompleteConfig (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Verify we have all the necessary pieces
  if (m_mac == 0 || m_phy == 0 || m_node == 0 || m_configComplete)
    {
      return;
    }

  m_mac->SetPhy (m_phy);
  m_configComplete = true;
}

void
SigfoxNetDevice::Send (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);

  // Send the packet to the MAC layer, if it exists
  NS_ASSERT (m_mac != 0);
  m_mac->Send (packet);
}

void
SigfoxNetDevice::Receive (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);

  // Fill protocol and address with empty stuff
  NS_LOG_DEBUG ("Calling receiveCallback");
  m_receiveCallback (this, packet, 0, Address ());
}

/******************************************
 *    Methods inherited from NetDevice    *
 ******************************************/

Ptr<Channel>
SigfoxNetDevice::GetChannel (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phy->GetChannel ();
}

Ptr<SigfoxChannel>
SigfoxNetDevice::DoGetChannel (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phy->GetChannel ();
}

void
SigfoxNetDevice::SetIfIndex (const uint32_t index)
{
  NS_LOG_FUNCTION (this << index);
}

uint32_t
SigfoxNetDevice::GetIfIndex (void) const
{
  NS_LOG_FUNCTION (this);

  return 0;
}

void
SigfoxNetDevice::SetAddress (Address address)
{
  NS_LOG_FUNCTION (this);
}

Address
SigfoxNetDevice::GetAddress (void) const
{
  NS_LOG_FUNCTION (this);

  return Address ();
}

bool
SigfoxNetDevice::SetMtu (const uint16_t mtu)
{
  NS_ABORT_MSG ("Unsupported");

  return false;
}

uint16_t
SigfoxNetDevice::GetMtu (void) const
{
  NS_LOG_FUNCTION (this);

  return 0;
}

bool
SigfoxNetDevice::IsLinkUp (void) const
{
  NS_LOG_FUNCTION (this);

  return m_phy != 0;
}

void
SigfoxNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
  NS_LOG_FUNCTION (this);
}

bool
SigfoxNetDevice::IsBroadcast (void) const
{
  NS_LOG_FUNCTION (this);

  return true;
}

Address
SigfoxNetDevice::GetBroadcast (void) const
{
  NS_LOG_FUNCTION (this);

  return Address ();
}

bool
SigfoxNetDevice::IsMulticast (void) const
{
  NS_LOG_FUNCTION (this);

  return true;
}

Address
SigfoxNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  NS_ABORT_MSG ("Unsupported");

  return Address ();
}

Address
SigfoxNetDevice::GetMulticast (Ipv6Address addr) const
{
  NS_LOG_FUNCTION (this);

  return Address ();
}

bool
SigfoxNetDevice::IsBridge (void) const
{
  NS_LOG_FUNCTION (this);

  return false;
}

bool
SigfoxNetDevice::IsPointToPoint (void) const
{
  NS_LOG_FUNCTION (this);

  return false;
}

bool
SigfoxNetDevice::Send (Ptr<Packet> packet, const Address& dest,
                     uint16_t protocolNumber)

{
  NS_LOG_FUNCTION (this << packet << dest << protocolNumber);

  // Fallback to the vanilla Send method
  Send (packet);

  return true;
}

bool
SigfoxNetDevice::SendFrom (Ptr<Packet> packet, const Address& source,
                         const Address& dest, uint16_t protocolNumber)

{
  NS_ABORT_MSG ("Unsupported");

  return false;
}

Ptr<Node>
SigfoxNetDevice::GetNode (void) const
{
  NS_LOG_FUNCTION (this);

  return m_node;
}

void
SigfoxNetDevice::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this);

  m_node = node;
  CompleteConfig ();
}

bool
SigfoxNetDevice::NeedsArp (void) const
{
  NS_LOG_FUNCTION (this);

  return true;
}

void
SigfoxNetDevice::SetReceiveCallback (ReceiveCallback cb)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_receiveCallback = cb;
}

void
SigfoxNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{
  NS_LOG_FUNCTION_NOARGS ();
}

bool
SigfoxNetDevice::SupportsSendFrom (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return false;
}

}
}

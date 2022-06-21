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

#include "ns3/gateway-sigfox-mac.h"
#include "ns3/sigfox-mac-header.h"
#include "ns3/sigfox-net-device.h"
#include "ns3/log.h"

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("GatewaySigfoxMac");

NS_OBJECT_ENSURE_REGISTERED (GatewaySigfoxMac);

TypeId
GatewaySigfoxMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::GatewaySigfoxMac")
    .SetParent<SigfoxMac> ()
    .AddConstructor<GatewaySigfoxMac> ()
    .SetGroupName ("sigfox");
  return tid;
}

GatewaySigfoxMac::GatewaySigfoxMac ()
{
  NS_LOG_FUNCTION (this);
}

GatewaySigfoxMac::~GatewaySigfoxMac ()
{
  NS_LOG_FUNCTION (this);
}

void
GatewaySigfoxMac::Send (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);

  // Get DataRate to send this packet with
  SigfoxTag tag;
  packet->RemovePacketTag (tag);
  double frequency = tag.GetFrequency ();
  NS_LOG_DEBUG ("Freq: " << frequency << " MHz");
  packet->AddPacketTag (tag);

  // XXX Verify we are respecting Duty Cycle

  SigfoxTxParameters params;
  params.bandwidthHz = 600;
  params.nPreamble = 223;

  // Get the duration
  Time duration = m_phy->GetOnAirTime (packet, params);

  NS_LOG_DEBUG ("Duration: " << duration.GetSeconds ());

  // Find the channel with the desired frequency
  double sendingPower = m_channelHelper.GetTxPowerForFrequency (frequency);

  // Add the event to the channelHelper to keep track of duty cycle
  m_channelHelper.AddEvent (duration, frequency);

  // Send the packet to the PHY layer to send it on the channel
  m_phy->Send (packet, params, frequency, sendingPower);

  m_sentNewPacket (packet);
}

bool
GatewaySigfoxMac::IsTransmitting (void)
{
  return m_phy->IsTransmitting ();
}

void
GatewaySigfoxMac::Receive (Ptr<Packet const> packet)
{
  NS_LOG_FUNCTION (this << packet);

  // Make a copy of the packet to work on
  Ptr<Packet> packetCopy = packet->Copy ();

  // Only forward the packet if it's uplink
  SigfoxMacHeader macHdr;
  packetCopy->PeekHeader (macHdr);

  if (true)//macHdr.IsUplink ())
    {
      m_device->GetObject<SigfoxNetDevice> ()->Receive (packetCopy);

      NS_LOG_DEBUG ("Received packet: " << packet);

      m_receivedPacket (packet);
    }
  else
    {
      NS_LOG_DEBUG ("Not forwarding downlink message to NetDevice");
    }
}

void
GatewaySigfoxMac::FailedReception (Ptr<Packet const> packet)
{
  NS_LOG_FUNCTION (this << packet);
}

void
GatewaySigfoxMac::TxFinished (Ptr<const Packet> packet)
{
  NS_LOG_FUNCTION_NOARGS ();
}

Time
GatewaySigfoxMac::GetWaitingTime (double frequency)
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_channelHelper.GetWaitingTime (frequency);
}

void GatewaySigfoxMac::BDPFrequency(int x)
{
}

}
}

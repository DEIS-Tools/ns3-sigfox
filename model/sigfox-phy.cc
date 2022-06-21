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

#include "ns3/sigfox-phy.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include <algorithm>

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("SigfoxPhy");

NS_OBJECT_ENSURE_REGISTERED (SigfoxPhy);

TypeId
SigfoxPhy::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::SigfoxPhy")
          .SetParent<Object> ()
          .SetGroupName ("sigfox")
          .AddTraceSource ("StartSending",
                           "Trace source indicating the PHY layer"
                           "has begun the sending process for a packet",
                           MakeTraceSourceAccessor (&SigfoxPhy::m_startSending),
                           "ns3::Packet::TracedCallback")
          .AddTraceSource ("PhyRxBegin",
                           "Trace source indicating a packet "
                           "is now being received from the channel medium "
                           "by the device",
                           MakeTraceSourceAccessor (&SigfoxPhy::m_phyRxBeginTrace),
                           "ns3::Packet::TracedCallback")
          .AddTraceSource ("PhyRxEnd",
                           "Trace source indicating the PHY has finished "
                           "the reception process for a packet",
                           MakeTraceSourceAccessor (&SigfoxPhy::m_phyRxEndTrace),
                           "ns3::Packet::TracedCallback")
          .AddTraceSource ("ReceivedPacket",
                           "Trace source indicating a packet "
                           "was correctly received",
                           MakeTraceSourceAccessor (&SigfoxPhy::m_successfullyReceivedPacket),
                           "ns3::Packet::TracedCallback")
          .AddTraceSource ("LostPacketBecauseInterference",
                           "Trace source indicating a packet "
                           "could not be correctly decoded because of interfering"
                           "signals",
                           MakeTraceSourceAccessor (&SigfoxPhy::m_interferedPacket),
                           "ns3::Packet::TracedCallback")
          .AddTraceSource ("LostPacketBecauseUnderSensitivity",
                           "Trace source indicating a packet "
                           "could not be correctly received because"
                           "its received power is below the sensitivity of the receiver",
                           MakeTraceSourceAccessor (&SigfoxPhy::m_underSensitivity),
                           "ns3::Packet::TracedCallback");
  return tid;
}

SigfoxPhy::SigfoxPhy ()
{
}

SigfoxPhy::~SigfoxPhy ()
{
}

Ptr<NetDevice>
SigfoxPhy::GetDevice (void) const
{
  return m_device;
}

void
SigfoxPhy::SetDevice (Ptr<NetDevice> device)
{
  NS_LOG_FUNCTION (this << device);

  m_device = device;
}

Ptr<SigfoxChannel>
SigfoxPhy::GetChannel (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_channel;
}

Ptr<MobilityModel>
SigfoxPhy::GetMobility (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  // If there is a mobility model associated to this PHY, take the mobility from
  // there
  if (m_mobility != 0)
    {
      return m_mobility;
    }
  else // Else, take it from the node
    {
      return m_device->GetNode ()->GetObject<MobilityModel> ();
    }
}

void
SigfoxPhy::SetMobility (Ptr<MobilityModel> mobility)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_mobility = mobility;
}

void
SigfoxPhy::SetChannel (Ptr<SigfoxChannel> channel)
{
  NS_LOG_FUNCTION (this << channel);

  m_channel = channel;
}

void
SigfoxPhy::SetReceiveOkCallback (RxOkCallback callback)
{
  m_rxOkCallback = callback;
}

void
SigfoxPhy::SetReceiveFailedCallback (RxFailedCallback callback)
{
  m_rxFailedCallback = callback;
}

void
SigfoxPhy::SetTxFinishedCallback (TxFinishedCallback callback)
{
  m_txFinishedCallback = callback;
}

Time
SigfoxPhy::GetOnAirTime (Ptr<Packet> packet, SigfoxTxParameters txParams)
{

  NS_LOG_FUNCTION (packet << txParams);

  NS_LOG_DEBUG ("Packet size: " << packet->GetSize());

  // We use the following table to compute the duration of a frame based on the
  // payload length, since Sigfox will automatically pad the payload to reach
  // one of several pre-defined possible configurations
  //
  // | Payload length  | RC1/RC3/RC5 | RC2/RC4 |
  // |-----------------+-------------+---------|
  // | < 1 bit         | 1.1 seconds | 190 ms  |
  // | 2 bits - 1 byte | 1.2 seconds | 200 ms  |
  // | 2 - 4 bytes     | 1.45 s      | 250 ms  |
  // | 5 - 8 bytes     | 1.75 s      | 300 ms  |
  // | 9 - 12 bytes    | 2 s         | 350 ms  |

  if (packet->GetSize() == 0)
    return Seconds (1.1);

  if (packet->GetSize() <= 1)
    return Seconds (1.2);

  if (packet->GetSize() <= 4)
    return Seconds (1.45);

  if (packet->GetSize() <= 8)
    return Seconds (1.75);
  
  if (packet->GetSize() <= 12)
    return Seconds (2);

  return Seconds (0);
}

std::ostream &
operator<< (std::ostream &os, const SigfoxTxParameters &params)
{
  return os;
}
} // namespace sigfox
} // namespace ns3

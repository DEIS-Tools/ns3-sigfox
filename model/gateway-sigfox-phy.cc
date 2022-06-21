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

#include "ns3/gateway-sigfox-phy.h"
#include "ns3/sigfox-tag.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("GatewaySigfoxPhy");

NS_OBJECT_ENSURE_REGISTERED (GatewaySigfoxPhy);

/***********************************************************************
 *                 Implementation of Gateway methods                   *
 ***********************************************************************/

TypeId
GatewaySigfoxPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::GatewaySigfoxPhy")
    .SetParent<SigfoxPhy> ()
    .SetGroupName ("sigfox")
    .AddTraceSource ("NoReceptionBecauseTransmitting",
                     "Trace source indicating a packet "
                     "could not be correctly received because"
                     "the GW is in transmission mode",
                     MakeTraceSourceAccessor
                       (&GatewaySigfoxPhy::m_noReceptionBecauseTransmitting),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("LostPacketBecauseNoMoreReceivers",
                     "Trace source indicating a packet "
                     "could not be correctly received because"
                     "there are no more demodulators available",
                     MakeTraceSourceAccessor
                       (&GatewaySigfoxPhy::m_noMoreDemodulators),
                     "ns3::Packet::TracedCallback");
  return tid;
}

GatewaySigfoxPhy::GatewaySigfoxPhy () :
  m_isTransmitting (false)
{
  NS_LOG_FUNCTION_NOARGS ();
}

GatewaySigfoxPhy::~GatewaySigfoxPhy ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

// Uplink sensitivity
// XXX Figure this out
const double GatewaySigfoxPhy::sensitivity = -124;

void
GatewaySigfoxPhy::TxFinished (Ptr<Packet> packet)
{
  m_isTransmitting = false;
}

bool
GatewaySigfoxPhy::IsTransmitting (void)
{
  return m_isTransmitting;
}

bool
GatewaySigfoxPhy::IsOnFrequency (double frequencyMHz)
{
  NS_LOG_FUNCTION (this << frequencyMHz);

  return true;
}
}
}

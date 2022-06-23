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

#include "ns3/sigfox-mac.h"
#include "ns3/log.h"

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("SigfoxMac");

NS_OBJECT_ENSURE_REGISTERED (SigfoxMac);

TypeId
SigfoxMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SigfoxMac")
    .SetParent<Object> ()
    .SetGroupName ("sigfox")
    .AddTraceSource ("SentNewPacket",
                     "Trace source indicating a new packet "
                     "arrived at the MAC layer",
                     MakeTraceSourceAccessor (&SigfoxMac::m_sentNewPacket),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("ReceivedPacket",
                     "Trace source indicating a packet "
                     "was correctly received at the MAC layer",
                     MakeTraceSourceAccessor (&SigfoxMac::m_receivedPacket),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("CannotSendBecauseDutyCycle",
                     "Trace source indicating a packet "
                     "could not be sent immediately because of duty cycle limitations",
                     MakeTraceSourceAccessor (&SigfoxMac::m_cannotSendBecauseDutyCycle),
                     "ns3::Packet::TracedCallback");
  return tid;
}

SigfoxMac::SigfoxMac ()
{
  NS_LOG_FUNCTION (this);
}

SigfoxMac::~SigfoxMac ()
{
  NS_LOG_FUNCTION (this);
}

void
SigfoxMac::SetDevice (Ptr<NetDevice> device)
{
  m_device = device;
}

Ptr<NetDevice>
SigfoxMac::GetDevice (void)
{
  return m_device;
}

Ptr<SigfoxPhy>
SigfoxMac::GetPhy (void)
{
  return m_phy;
}

void
SigfoxMac::SetPhy (Ptr<SigfoxPhy> phy)
{
  // Set the phy
  m_phy = phy;

  // Connect the receive callbacks
  m_phy->SetReceiveOkCallback (MakeCallback (&SigfoxMac::Receive, this));
  m_phy->SetReceiveFailedCallback (MakeCallback (&SigfoxMac::FailedReception, this));
  m_phy->SetTxFinishedCallback (MakeCallback (&SigfoxMac::TxFinished, this));
}

LogicalSigfoxChannelHelper
SigfoxMac::GetLogicalSigfoxChannelHelper (void)
{
  return m_channelHelper;
}

void
SigfoxMac::SetLogicalSigfoxChannelHelper (LogicalSigfoxChannelHelper helper)
{
  m_channelHelper = helper;
}

double
SigfoxMac::GetBandwidthFromDataRate (uint8_t dataRate)
{
  NS_LOG_FUNCTION (this << unsigned(dataRate));
 
  // Check we are in range
  if (dataRate > m_bandwidthForDataRate.size ())
    {
      return 0;
    }

  return m_bandwidthForDataRate.at (dataRate);
}

double
SigfoxMac::GetDbmForTxPower (uint8_t txPower)
{
  NS_LOG_FUNCTION (this << unsigned (txPower));

  if (txPower > m_txDbmForTxPower.size ())
    {
      return 0;
    }
  return m_txDbmForTxPower.at (txPower);
}

void
SigfoxMac::SetBandwidthForDataRate (std::vector<double> bandwidthForDataRate)
{
  m_bandwidthForDataRate = bandwidthForDataRate;
}

void
SigfoxMac::SetMaxAppPayloadForDataRate (std::vector<uint32_t> maxAppPayloadForDataRate)
{
  m_maxAppPayloadForDataRate = maxAppPayloadForDataRate;

}

void
SigfoxMac::SetTxDbmForTxPower (std::vector<double> txDbmForTxPower)
{
  m_txDbmForTxPower = txDbmForTxPower;
}

void
SigfoxMac::SetNPreambleSymbols (int nPreambleSymbols)
{
  m_nPreambleSymbols = nPreambleSymbols;
}

int
SigfoxMac::GetNPreambleSymbols (void)
{
  return m_nPreambleSymbols;
}


void
SigfoxMac::SetReplyDataRateMatrix (ReplyDataRateMatrix replyDataRateMatrix)
{
    
  m_replyDataRateMatrix = replyDataRateMatrix;
}
}
}

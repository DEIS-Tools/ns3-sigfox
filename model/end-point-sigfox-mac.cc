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
 *         Martina Capuzzo <capuzzom@dei.unipd.it>
 *
 * Modified by: Peggy Anderson <peggy.anderson@usask.ca>
 */

#include "ns3/end-point-sigfox-mac.h"
#include "ns3/integer.h"
#include "ns3/sigfox-mac.h"
#include "ns3/end-point-sigfox-phy.h"
#include "ns3/sigfox-tag.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include <algorithm>

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("EndPointSigfoxMac");

NS_OBJECT_ENSURE_REGISTERED (EndPointSigfoxMac);

TypeId
EndPointSigfoxMac::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::EndPointSigfoxMac")
          .SetParent<SigfoxMac> ()
          .SetGroupName ("sigfox")
          .AddTraceSource ("RequiredTransmissions",
                           "Total number of transmissions required to deliver this packet",
                           MakeTraceSourceAccessor (&EndPointSigfoxMac::m_requiredTxCallback),
                           "ns3::TracedValueCallback::uint8_t")
          .AddTraceSource ("TxPower", "Transmission power currently employed by this end device",
                           MakeTraceSourceAccessor (&EndPointSigfoxMac::m_txPower),
                           "ns3::TracedValueCallback::Double")
          .AddAttribute ("MType", "Specify type of message will be sent by this ED.",
                         EnumValue (SigfoxMacHeader::UNCONFIRMED_DATA_UP),
                         MakeEnumAccessor (&EndPointSigfoxMac::m_mType),
                         MakeEnumChecker (SigfoxMacHeader::UNCONFIRMED_DATA_UP, "Unconfirmed",
                                          SigfoxMacHeader::CONFIRMED_DATA_UP, "Confirmed"))
          .AddAttribute ("Repetitions", "Number of times we should transmit a packet",
                         IntegerValue (3),
                         MakeIntegerAccessor (&EndPointSigfoxMac::m_nRepetitions),
                         MakeIntegerChecker<uint32_t> (1, 10))
          .AddConstructor<EndPointSigfoxMac> ();
  return tid;
}

EndPointSigfoxMac::EndPointSigfoxMac ()
    : m_enableDRAdapt (false),
      m_maxNumbTx (8),
      m_txPower (14),
      m_codingRate (1),
      // Sigfox default
      m_headerDisabled (0),
      // Sigfox default
      m_receiveWindowDurationInSymbols (8),
      // Sigfox default
      m_lastKnownLinkMargin (0),
      m_lastKnownGatewayCount (0),
      m_aggregatedDutyCycle (1),
      m_mType (SigfoxMacHeader::CONFIRMED_DATA_UP),
      m_currentFCnt (0)
{
  NS_LOG_FUNCTION (this);

  // Initialize the random variable we'll use to decide which channel to
  // transmit on.
  m_uniformRV = CreateObject<UniformRandomVariable> ();

  // Void the transmission event
  m_nextTx = EventId ();
  m_nextTx.Cancel ();
}

EndPointSigfoxMac::~EndPointSigfoxMac ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

////////////////////////
//  Sending methods   //
////////////////////////

void
EndPointSigfoxMac::Send (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);

  DoSend (packet);
}

void
EndPointSigfoxMac::DoSend (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);
  SendToPhy (packet);
}

void
EndPointSigfoxMac::SendToPhy (Ptr<Packet> packet)
{
  /////////////////////////////////////////////////////////
  // Add headers, prepare TX parameters and send the packet
  /////////////////////////////////////////////////////////

  // Craft LoraTxParameters object
  SigfoxTxParameters params;
  params.bandwidthHz = 100;
  params.nPreamble = 112;

  NS_LOG_DEBUG ("PacketToSend: " << packet);
  SigfoxTag tag;
  packet->RemovePacketTag (tag);
  tag.SetRepetitionNumber (m_sendCount);
  tag.SetPacketNumber (m_appPacketCount);
  tag.SetSenderId (m_device->GetNode()->GetId());
  packet->AddPacketTag (tag);
  m_phy->Send (packet, params, m_channelHelper.GetFrequencyFromChannelSet(), m_txPower);
  // m_phy->Send (packet, params, m_channelHelper.GetRandomFrequency(), m_txPower);
}

//////////////////////////
//  Receiving methods   //
//////////////////////////

void
EndPointSigfoxMac::Receive (Ptr<Packet const> packet)
{
    //NS_LOG_DEBUG ("********************" <<m_packetReceived);
    
}

void
EndPointSigfoxMac::FailedReception (Ptr<Packet const> packet)
{
}

void
EndPointSigfoxMac::ApplyNecessaryOptions (SigfoxMacHeader &macHeader)
{
  NS_LOG_FUNCTION_NOARGS ();

  macHeader.SetMType (m_mType);
}

void
EndPointSigfoxMac::SetMType (SigfoxMacHeader::MType mType)
{
  m_mType = mType;
  NS_LOG_DEBUG ("Message type is set to " << mType);
}

SigfoxMacHeader::MType
EndPointSigfoxMac::GetMType (void)
{
  return m_mType;
}

void
EndPointSigfoxMac::TxFinished (Ptr<const Packet> packet)
{
  NS_LOG_FUNCTION_NOARGS ();

  Ptr<Packet> packetCopy = packet->Copy ();
    //NS_LOG_DEBUG ("hahahhaah" <<m_packetReceived);
  if (m_packetReceived)
    {
      m_phy->GetObject<EndPointSigfoxPhy> ()->SwitchToSleep ();
      m_packetReceived = false;
    }
  else
    {
      m_sendCount = m_sendCount + 1;

      if (m_sendCount == 1)
        {
          msg_cnt += 1;
            if(msg_cnt == updateBDP)
            {
                Simulator::Schedule (Seconds (20), &EndPointSigfoxMac::OpenFirstReceiveWindow, this);
                msg_cnt =0;
            }
        }

      NS_LOG_DEBUG ("Copy number " << m_sendCount << " sent" <<m_nRepetitions);

      if (m_sendCount < m_nRepetitions)
        {
          Simulator::Schedule (Seconds (.525), &EndPointSigfoxMac::SendToPhy, this, packetCopy);
        }

      if (m_sendCount == m_nRepetitions)
        {
          m_sendCount = 0;
          m_appPacketCount += 1;
            if (msg_cnt > 0)
            {
                m_phy->GetObject<EndPointSigfoxPhy> ()->SwitchToSleep ();
            }
        }
    }
}

void
EndPointSigfoxMac::OpenFirstReceiveWindow (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Set Phy in Standby mode
  m_phy->GetObject<EndPointSigfoxPhy> ()->SwitchToRx ();

  Simulator::Schedule (Seconds (25), &EndPointSigfoxMac::CloseFirstReceiveWindow, this);
}

void
EndPointSigfoxMac::CloseFirstReceiveWindow (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_phy->GetObject<EndPointSigfoxPhy> ()->SwitchToStandby ();
   // NS_LOG_DEBUG ("hahahhaah22222" <<m_packetReceived<<m_sendCtrlMsg);
  if (m_sendCtrlMsg )// && m_packetReceived)
    {
      SendCtrlMessage ();
      
    }
  else
  {
      m_phy->GetObject<EndPointSigfoxPhy> ()->SwitchToSleep ();
  }
}

void
EndPointSigfoxMac::SendCtrlMessage (void)
{
   // ctrl_send_cnt+= 1;
    NS_LOG_DEBUG ("Send a control Message" <<ctrl_send_cnt);
   // if (ctrl_send_cnt<=4)
    //{
        m_packetReceived = true;
        Ptr<Packet> ctrlMessage = Create<Packet> (1);
        Simulator::Schedule (Seconds (4), &EndPointSigfoxMac::SendToPhy, this, ctrlMessage);
   // }
   /* else{
        m_phy->GetObject<EndPointSigfoxPhy> ()->SwitchToSleep ();
    }
    if(ctrl_send_cnt>=144)
    {
        ctrl_send_cnt=0;
    }*/
    //Simulator::Schedule (Seconds (86400.0), &ResetCtrlcntr );
}
void
EndPointSigfoxMac::ResetCtrlcntr (void)
{
    ctrl_send_cnt=0;
}
/////////////////////////
// Setters and Getters //
/////////////////////////

uint8_t
EndPointSigfoxMac::GetMaxNumberOfTransmissions (void)
{
  NS_LOG_FUNCTION (this);
  return m_maxNumbTx;
}

void
EndPointSigfoxMac::AddSubBand (double startFrequency, double endFrequency, double dutyCycle,
                               double maxTxPowerDbm)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_channelHelper.AddSubBand (startFrequency, endFrequency, dutyCycle, maxTxPowerDbm);
}

double
EndPointSigfoxMac::GetAggregatedDutyCycle (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_aggregatedDutyCycle;
}

void EndPointSigfoxMac::BDPFrequency(int BDPF)
{
    updateBDP = BDPF;
}

uint8_t
EndPointSigfoxMac::GetTransmissionPower (void)
{
  return m_txPower;
}
} // namespace sigfox
} // namespace ns3

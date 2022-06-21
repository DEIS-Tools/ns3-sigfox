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

#include "ns3/simple-gateway-sigfox-phy.h"
#include "ns3/sigfox-tag.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("SimpleGatewaySigfoxPhy");

NS_OBJECT_ENSURE_REGISTERED (SimpleGatewaySigfoxPhy);

/***********************************************************************
 *                 Implementation of Gateway methods                   *
 ***********************************************************************/

TypeId
SimpleGatewaySigfoxPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SimpleGatewaySigfoxPhy")
    .SetParent<GatewaySigfoxPhy> ()
    .SetGroupName ("sigfox")
    .AddConstructor<SimpleGatewaySigfoxPhy> ();

  return tid;
}

SimpleGatewaySigfoxPhy::SimpleGatewaySigfoxPhy ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

SimpleGatewaySigfoxPhy::~SimpleGatewaySigfoxPhy ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
SimpleGatewaySigfoxPhy::Send (Ptr<Packet> packet, SigfoxTxParameters txParams,
                            double frequencyHz, double txPowerDbm)
{
  NS_LOG_FUNCTION (this << packet << frequencyHz << txPowerDbm);

  // Get the time a packet with these parameters will take to be transmitted
  Time duration = GetOnAirTime (packet, txParams);

  NS_LOG_DEBUG ("Duration of packet: " << duration);

  // TODO
  // if (m_device)
  //   {
  //     m_noReceptionBecauseTransmitting (currentPath->GetEvent ()->GetPacket (),
  //                                       m_device->GetNode ()->GetId ());
  //   }
  // else
  //   {
  //     m_noReceptionBecauseTransmitting (currentPath->GetEvent ()->GetPacket (), 0);
  //   }

  // Send the packet in the channel
  m_channel->Send (this, packet, txPowerDbm, txParams, duration, frequencyHz);

  Simulator::Schedule (duration, &SimpleGatewaySigfoxPhy::TxFinished, this, packet);

  m_isTransmitting = true;

  // Fire the trace source
  if (m_device)
    {
      m_startSending (packet, m_device->GetNode ()->GetId ());
    }
  else
    {
      m_startSending (packet, 0);
    }
}

void
SimpleGatewaySigfoxPhy::StartReceive (Ptr<Packet> packet, double rxPowerDbm,
                                    Time duration, double frequencyHz)
{
  NS_LOG_FUNCTION (this << packet << rxPowerDbm << duration << frequencyHz);

  // Fire the trace source
  m_phyRxBeginTrace (packet);

  if (m_isTransmitting)
    {
      // If we get to this point, there are no demodulators we can use
      NS_LOG_INFO ("Dropping packet reception of packet because we are in TX mode");

      m_phyRxEndTrace (packet);

      // Fire the trace source
      if (m_device)
        {
          m_noReceptionBecauseTransmitting (packet, m_device->GetNode ()->GetId ());
        }
      else
        {
          m_noReceptionBecauseTransmitting (packet, 0);
        }

      return;
    }

  // Add the event to the SigfoxInterferenceHelper
  Ptr<SigfoxInterferenceHelper::Event> event;
  event = m_interference.Add (duration, rxPowerDbm, packet, frequencyHz);

  // See whether the reception power is above or below the sensitivity
  // for that spreading factor
  double sensitivity = SimpleGatewaySigfoxPhy::sensitivity;

  if (rxPowerDbm < sensitivity) // Packet arrived below sensitivity
    {
      NS_LOG_INFO ("Dropping packet reception of packet because under the sensitivity of "
                   << sensitivity << " dBm");

      if (m_device)
        {
          m_underSensitivity (packet, m_device->GetNode ()->GetId ());
        }
      else
        {
          m_underSensitivity (packet, 0);
        }

      // Since the packet is below sensitivity, it makes no sense to
      // search for another ReceivePath
      return;
    }
  else // We have sufficient sensitivity to start receiving
    {
      NS_LOG_INFO ("Scheduling reception of a packet, "
                   << "occupying one demodulator");

      // Schedule the end of the reception of the packet
      EventId endReceiveEventId =
          Simulator::Schedule (duration, &SigfoxPhy::EndReceive, this, packet, event);
    }
}

void
SimpleGatewaySigfoxPhy::EndReceive (Ptr<Packet> packet,
                                  Ptr<SigfoxInterferenceHelper::Event> event)
{
  NS_LOG_FUNCTION (this << packet << *event);

  // Call the trace source
  m_phyRxEndTrace (packet);

  // Call the SigfoxInterferenceHelper to determine whether there was
  // destructive interference. If the packet is correctly received, this
  // method returns a 0.
  bool packetDestroyed = m_interference.IsDestroyedByInterference (event);

  // Check whether the packet was destroyed
  if (packetDestroyed)
    {
      NS_LOG_DEBUG ("packetDestroyed by " << unsigned(packetDestroyed));

      // Update the packet's SigfoxTag
      SigfoxTag tag;
      packet->RemovePacketTag (tag);
      packet->AddPacketTag (tag);

      // Fire the trace source
      if (m_device)
        {
          m_interferedPacket (packet, m_device->GetNode ()->GetId ());
        }
      else
        {
          m_interferedPacket (packet, 0);
        }
    }
  else       // Reception was correct
    {
      NS_LOG_INFO ("Packet received correctly");

      // Fire the trace source
      if (m_device)
        {
          m_successfullyReceivedPacket (packet, m_device->GetNode ()->GetId ());
        }
      else
        {
          m_successfullyReceivedPacket (packet, 0);
        }

      // Forward the packet to the upper layer
      if (!m_rxOkCallback.IsNull ())
        {
          // Make a copy of the packet
          // Ptr<Packet> packetCopy = packet->Copy ();

          // Set the receive power and frequency of this packet in the SigfoxTag: this
          // information can be useful for upper layers trying to control link
          // quality.
          SigfoxTag tag;
          packet->RemovePacketTag (tag);
          tag.SetReceivePower (event->GetRxPowerdBm ());
          tag.SetFrequency (event->GetFrequency ());
          packet->AddPacketTag (tag);

          m_rxOkCallback (packet);
        }

    }
}

}
}

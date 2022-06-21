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

#include <algorithm>
#include "ns3/end-point-sigfox-phy.h"
#include "ns3/simulator.h"
#include "ns3/sigfox-tag.h"
#include "ns3/log.h"

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("EndPointSigfoxPhy");

NS_OBJECT_ENSURE_REGISTERED (EndPointSigfoxPhy);

/**************************
 *  Listener destructor  *
 *************************/

EndPointSigfoxPhyListener::~EndPointSigfoxPhyListener ()
{
}

TypeId
EndPointSigfoxPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EndPointSigfoxPhy")
    .SetParent<SigfoxPhy> ()
    .SetGroupName ("sigfox")
    .AddTraceSource ("LostPacketBecauseWrongFrequency",
                     "Trace source indicating a packet "
                     "could not be correctly decoded because"
                     "the ED was listening on a different frequency",
                     MakeTraceSourceAccessor (&EndPointSigfoxPhy::m_wrongFrequency),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("EndPointState",
                     "The current state of the device",
                     MakeTraceSourceAccessor
                       (&EndPointSigfoxPhy::m_state),
                     "ns3::TracedValueCallback::EndPointSigfoxPhy::State");
  return tid;
}

// Initialize the device with some common settings.
// These will then be changed by helpers.
EndPointSigfoxPhy::EndPointSigfoxPhy () :
  m_state (SLEEP),
  m_frequency (868.1)
{
}

EndPointSigfoxPhy::~EndPointSigfoxPhy ()
{
}

// Downlink sensitivity (from EP-SPECSv1.5 document, February 2020)
const double EndPointSigfoxPhy::sensitivity = -126;   // dBm, 10% packet error rate

bool
EndPointSigfoxPhy::IsTransmitting (void)
{
  return m_state == TX;
}

bool
EndPointSigfoxPhy::IsOnFrequency (double frequencyMHz)
{
  return m_frequency == frequencyMHz;
}

void
EndPointSigfoxPhy::SetFrequency (double frequencyMHz)
{
  m_frequency = frequencyMHz;
}

void
EndPointSigfoxPhy:: SwitchToStandby(void)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_state = STANDBY;

  // Notify listeners of the state change
  for (Listeners::const_iterator i = m_listeners.begin (); i != m_listeners.end (); i++)
    {
      (*i)->NotifyStandby ();
    }
}

void
EndPointSigfoxPhy::SwitchToRx (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  NS_ASSERT (m_state == STANDBY);

  m_state = RX;

  // Notify listeners of the state change
  for (Listeners::const_iterator i = m_listeners.begin (); i != m_listeners.end (); i++)
    {
      (*i)->NotifyRxStart ();
    }
}

void
EndPointSigfoxPhy::SwitchToTx (double txPowerDbm)
{
  NS_LOG_FUNCTION_NOARGS ();

  NS_ASSERT (m_state != RX);

  m_state = TX;

  // Notify listeners of the state change
  for (Listeners::const_iterator i = m_listeners.begin (); i != m_listeners.end (); i++)
    {
      (*i)->NotifyTxStart (txPowerDbm);
    }
}

void
EndPointSigfoxPhy::SwitchToSleep (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  NS_ASSERT (m_state == STANDBY);

  m_state = SLEEP;

  // Notify listeners of the state change
  for (Listeners::const_iterator i = m_listeners.begin (); i != m_listeners.end (); i++)
    {
      (*i)->NotifySleep ();
    }
}

EndPointSigfoxPhy::State
EndPointSigfoxPhy::GetState (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_state;
}

void
EndPointSigfoxPhy::RegisterListener (EndPointSigfoxPhyListener *listener)
{
  m_listeners.push_back (listener);
}

void
EndPointSigfoxPhy::UnregisterListener (EndPointSigfoxPhyListener *listener)
{
  ListenersI i = find (m_listeners.begin (), m_listeners.end (), listener);
  if (i != m_listeners.end ())
    {
      m_listeners.erase (i);
    }
}

}
}

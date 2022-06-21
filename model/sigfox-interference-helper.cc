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

#include "ns3/sigfox-interference-helper.h"
#include "ns3/log-macros-enabled.h"
#include "ns3/log.h"
#include "ns3/enum.h"
#include <limits>

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("SigfoxInterferenceHelper");

/***************************************
 *    SigfoxInterferenceHelper::Event    *
 ***************************************/

// Event Constructor
SigfoxInterferenceHelper::Event::Event (Time duration, double rxPowerdBm, Ptr<Packet> packet, double frequencyMHz)
    : m_startTime (Simulator::Now ()),
      m_endTime (m_startTime + duration),
      m_rxPowerdBm (rxPowerdBm),
      m_packet (packet),
      m_frequencyMHz (frequencyMHz)
{
  // NS_LOG_FUNCTION_NOARGS ();
}

// Event Destructor
SigfoxInterferenceHelper::Event::~Event ()
{
  // NS_LOG_FUNCTION_NOARGS ();
}

// Getters
Time
SigfoxInterferenceHelper::Event::GetStartTime (void) const
{
  return m_startTime;
}

Time
SigfoxInterferenceHelper::Event::GetEndTime (void) const
{
  return m_endTime;
}

Time
SigfoxInterferenceHelper::Event::GetDuration (void) const
{
  return m_endTime - m_startTime;
}

double
SigfoxInterferenceHelper::Event::GetRxPowerdBm (void) const
{
  return m_rxPowerdBm;
}

Ptr<Packet>
SigfoxInterferenceHelper::Event::GetPacket (void) const
{
  return m_packet;
}

double
SigfoxInterferenceHelper::Event::GetFrequency (void) const
{
  return m_frequencyMHz;
}

void
SigfoxInterferenceHelper::Event::Print (std::ostream &stream) const
{
  stream << "(" << m_startTime.GetSeconds () << " s - " << m_endTime.GetSeconds () << "s"
         << ", " << m_rxPowerdBm << " dBm, " << m_frequencyMHz << " MHz";
}

std::ostream &
operator<< (std::ostream &os, const SigfoxInterferenceHelper::Event &event)
{
  event.Print (os);

  return os;
}

NS_OBJECT_ENSURE_REGISTERED (SigfoxInterferenceHelper);

TypeId
SigfoxInterferenceHelper::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::SigfoxInterferenceHelper").SetParent<Object> ().SetGroupName ("sigfox");

  return tid;
}

SigfoxInterferenceHelper::SigfoxInterferenceHelper ()
{
  NS_LOG_FUNCTION (this);
}

SigfoxInterferenceHelper::~SigfoxInterferenceHelper ()
{
  NS_LOG_FUNCTION (this);
}

Time SigfoxInterferenceHelper::oldEventThreshold = Seconds (2);

Ptr<SigfoxInterferenceHelper::Event>
SigfoxInterferenceHelper::Add (Time duration, double rxPower, Ptr<Packet> packet,
                               double frequencyMHz)
{

  NS_LOG_FUNCTION (this << duration.GetSeconds () << rxPower << packet << frequencyMHz);

  // Create an event based on the parameters
  Ptr<SigfoxInterferenceHelper::Event> event =
      Create<SigfoxInterferenceHelper::Event> (duration, rxPower, packet, frequencyMHz);

  // Add the event to the list
  m_events.push_back (event);

  // Clean the event list
  if (m_events.size () > 100)
    {
      CleanOldEvents ();
    }

  return event;
}

void
SigfoxInterferenceHelper::CleanOldEvents (void)
{
  NS_LOG_FUNCTION (this);

  // Cycle the events, and clean up if an event is old.
  for (auto it = m_events.begin (); it != m_events.end ();)
    {
      if ((*it)->GetEndTime () + oldEventThreshold < Simulator::Now ())
        {
          it = m_events.erase (it);
        }
      it++;
    }
}

std::list<Ptr<SigfoxInterferenceHelper::Event>>
SigfoxInterferenceHelper::GetInterferers ()
{
  return m_events;
}

void
SigfoxInterferenceHelper::PrintEvents (std::ostream &stream)
{
  NS_LOG_FUNCTION_NOARGS ();

  stream << "Currently registered events:" << std::endl;

  for (auto it = m_events.begin (); it != m_events.end (); it++)
    {
      (*it)->Print (stream);
      stream << std::endl;
    }
}

bool
SigfoxInterferenceHelper::IsDestroyedByInterference (Ptr<SigfoxInterferenceHelper::Event> event)
{
  NS_LOG_FUNCTION (this << event);

  NS_LOG_INFO ("Current number of events in SigfoxInterferenceHelper: " << m_events.size ());

  // Handy information about the time frame when the packet was received
  Time now = Simulator::Now ();
  Time duration = event->GetDuration ();
  Time packetStartTime = now - duration;
  Time packetEndTime = now;

  // Get the list of interfering events
  std::list<Ptr<SigfoxInterferenceHelper::Event>>::iterator it;

  // double rxPower = std::pow (10, event->GetRxPowerdBm () / 10)/1000;
  // double sumOfInterfererRxPowers = 0;
  // static const double BOLTZMANN = 1.3803e-23;
  // double Nt = BOLTZMANN * 290.0 * 100;
  // double noiseFigureDb = 2;
  // double noiseFigure = std::pow (10, noiseFigureDb / 10);
  // double noisePower = noiseFigure * Nt;

  // Cycle over the events
  for (it = m_events.begin (); it != m_events.end ();)
    {
      // Pointer to the current interferer
      Ptr<SigfoxInterferenceHelper::Event> interferer = *it;

      // Only consider the current event if the channel is the same: we
      // assume there's no interchannel interference. Also skip the current
      // event if it's the same that we want to analyze.
      if (interferer == event)
        {
          NS_LOG_DEBUG ("Skipping the same event");
          it++;
          continue; // Continues from the first line inside the for cycle
        }

      if (GetOverlapTime(*it, event) > Seconds (0) &&
          GetOverlapFrequency(*it, event) > 0.0)
        {
          // TODO Compute this appropriately
          // sumOfInterfererRxPowers += std::pow (10, (*it)->GetRxPowerdBm()/10)/1000;
          return true;
        }
      it++;
    }

  // double sinr = rxPower / (sumOfInterfererRxPowers + noisePower);
  // double z = sqrt (sinr);
  // double ber = 0.5 * erfc (z);
  // NS_LOG_INFO ("bpsk snr=" << sinr << " ber=" << ber);
  // NS_LOG_INFO ("Desired packet rx power: " << rxPower);
  // NS_LOG_INFO ("Noise packet: " << noisePower);
  // NS_LOG_INFO ("Interferer rx power: " << sumOfInterfererRxPowers);
  // NS_LOG_INFO ("SINR: " << sinr << " BER: " << ber);
  // NS_LOG_INFO ("SINR: " << 10 * std::log10(sinr) << " dB");
  // // XXX Arbitrary beta as in do2014benefits
  // return ber > 1e-3;

  return false;
}

void
SigfoxInterferenceHelper::ClearAllEvents (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_events.clear ();
}

double
SigfoxInterferenceHelper::GetOverlapFrequency (Ptr<SigfoxInterferenceHelper::Event> event1,
                                               Ptr<SigfoxInterferenceHelper::Event> event2)
{
  NS_LOG_FUNCTION_NOARGS ();

  double overlap;

  double lowerFrequency = std::min (event1->GetFrequency(),
                                    event2->GetFrequency());
  double higherFrequency = std::max (event1->GetFrequency(),
                                     event2->GetFrequency());

  overlap = std::max(0.0, (lowerFrequency + 50) - (higherFrequency - 50));

  return overlap;
}

Time
SigfoxInterferenceHelper::GetOverlapTime (Ptr<SigfoxInterferenceHelper::Event> event1,
                                          Ptr<SigfoxInterferenceHelper::Event> event2)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Create the value we will return later
  Time overlap;

  // Get handy values
  Time s1 = event1->GetStartTime (); // Start times
  Time s2 = event2->GetStartTime ();
  Time e1 = event1->GetEndTime (); // End times
  Time e2 = event2->GetEndTime ();

  // Non-overlapping events
  if (e1 <= s2 || e2 <= s1)
    {
      overlap = Seconds (0);
    }
  // event1 before event2
  else if (s1 < s2)
    {
      if (e2 < e1)
        {
          overlap = e2 - s2;
        }
      else
        {
          overlap = e1 - s2;
        }
    }
  // event2 before event1 or they start at the same time (s1 = s2)
  else
    {
      if (e1 < e2)
        {
          overlap = e1 - s1;
        }
      else
        {
          overlap = e2 - s1;
        }
    }

  return overlap;
}
} // namespace sigfox
} // namespace ns3

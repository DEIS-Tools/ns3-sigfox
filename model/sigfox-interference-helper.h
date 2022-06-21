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

#ifndef SIGFOX_INTERFERENCE_HELPER_H
#define SIGFOX_INTERFERENCE_HELPER_H

#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/object.h"
#include "ns3/traced-callback.h"
#include "ns3/callback.h"
#include "ns3/packet.h"
#include <list>

namespace ns3 {
namespace sigfox {

/**
 * Helper for SigfoxPhy that manages interference calculations.
 *
 * This class keeps a list of signals that are impinging on the antenna of the
 * device, in order to compute which ones can be correctly received and which
 * ones are lost due to interference.
 */
class SigfoxInterferenceHelper
{
public:
  /**
   * A class representing a signal in time.
   *
   * Used in SigfoxInterferenceHelper to keep track of which signals overlap and
   * cause destructive interference.
   */
  class Event : public SimpleRefCount<SigfoxInterferenceHelper::Event>
  {

  public:
    Event (Time duration, double rxPowerdBm, Ptr<Packet> packet, double frequencyMHz);
    ~Event ();

    /**
     * Get the duration of the event.
     */
    Time GetDuration (void) const;

    /**
     * Get the starting time of the event.
     */
    Time GetStartTime (void) const;

    /**
     * Get the ending time of the event.
     */
    Time GetEndTime (void) const;

    /**
     * Get the power of the event.
     */
    double GetRxPowerdBm (void) const;

    /**
     * Get the packet this event was generated for.
     */
    Ptr<Packet> GetPacket (void) const;

    /**
     * Get the frequency this event was on.
     */
    double GetFrequency (void) const;

    /**
     * Print the current event in a human readable form.
     */
    void Print (std::ostream &stream) const;

  private:
    /**
     * The time this signal begins (at the device).
     */
    Time m_startTime;

    /**
     * The time this signal ends (at the device).
     */
    Time m_endTime;

    /**
     * The power of this event in dBm (at the device).
     */
    double m_rxPowerdBm;

    /**
     * The packet this event was generated for.
     */
    Ptr<Packet> m_packet;

    /**
     * The frequency this event was on.
     */
    double m_frequencyMHz;
  };

  static TypeId GetTypeId (void);

  SigfoxInterferenceHelper ();
  virtual ~SigfoxInterferenceHelper ();

  /**
   * Add an event to the InterferenceHelper
   *
   * \param duration the duration of the packet.
   * \param rxPower the received power in dBm.
   * \param packet The packet carried by this transmission.
   * \param frequencyMHz The frequency this event was sent at.
   *
   * \return the newly created event
   */
  Ptr<SigfoxInterferenceHelper::Event> Add (Time duration, double rxPower, Ptr<Packet> packet, double frequencyMHz);

  /**
   * Get a list of the interferers currently registered at this
   * InterferenceHelper.
   */
  std::list<Ptr<SigfoxInterferenceHelper::Event>> GetInterferers ();

  /**
   * Print the events that are saved in this helper in a human readable format.
   */
  void PrintEvents (std::ostream &stream);

  /**
   * Determine whether the event was destroyed by interference or not. This is
   * the method where the SNIR tables come into play and the computations
   * regarding power are performed.

   * \param event The event for which to check the outcome.
   * \return Whether the packet was lost because of interference.
   */
  bool IsDestroyedByInterference (Ptr<SigfoxInterferenceHelper::Event> event);

  /**
   * Compute the time duration in which two given events are overlapping.
   *
   * \param event1 The first event
   * \param event2 The second event
   *
   * \return The overlap time
   */
  Time GetOverlapTime (Ptr<SigfoxInterferenceHelper::Event> event1,
                       Ptr<SigfoxInterferenceHelper::Event> event2);

  double GetOverlapFrequency (Ptr<SigfoxInterferenceHelper::Event> event1,
                              Ptr<SigfoxInterferenceHelper::Event> event2);

  /**
   * Delete all events in the SigfoxInterferenceHelper.
   */
  void ClearAllEvents (void);

  /**
   * Delete old events in this SigfoxInterferenceHelper.
   */
  void CleanOldEvents (void);

private:
  /**
   * A list of the events this SigfoxInterferenceHelper is keeping track of.
   */
  std::list<Ptr<SigfoxInterferenceHelper::Event>> m_events;

  /**
   * The matrix containing information about how packets survive interference.
   */
  /**
   * The threshold after which an event is considered old and removed from the
   * list.
   */
  static Time oldEventThreshold;
};

/**
 * Allow easy logging of SigfoxInterferenceHelper Events
 */
std::ostream &operator<< (std::ostream &os, const SigfoxInterferenceHelper::Event &event);
} // namespace sigfox
} // namespace ns3
#endif /* SIGFOX_INTERFERENCE_HELPER_H */

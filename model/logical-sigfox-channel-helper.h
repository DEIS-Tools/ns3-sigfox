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

#ifndef LOGICAL_SIGFOX_CHANNEL_HELPER_H
#define LOGICAL_SIGFOX_CHANNEL_HELPER_H

#include "ns3/object.h"
#include "ns3/nstime.h"
#include "ns3/packet.h"
#include "ns3/random-variable-stream.h"
#include "ns3/sub-band.h"
#include <list>
#include <iterator>
#include <vector>

namespace ns3 {
namespace sigfox {

/**
 * This class supports SigfoxMac instances by managing a list of the logical
 * channels that the device is supposed to be using, and establishes their
 * relationship with SubBands.
 *
 * This class also takes into account duty cycle limitations, by updating a list
 * of SubBand objects and providing methods to query whether transmission on a
 * set channel is admissible or not.
 */
class LogicalSigfoxChannelHelper : public Object
{
public:
  static TypeId GetTypeId (void);

  LogicalSigfoxChannelHelper ();
  virtual ~LogicalSigfoxChannelHelper ();

  void SetFrequencyRange (double startFrequencyHz, double endFrequencyHz);

  double GetRandomFrequency (void);

  double GetFrequencyFromChannelSet (void);

  /**
   * Get the time it is necessary to wait before transmitting again, according
   * to the aggregate duty cycle timer.
   *
   * \return The aggregate waiting time.
   */
  Time GetAggregatedWaitingTime (void);

  /**
   * Get the time it is necessary to wait for before transmitting on a given
   * channel.
   *
   * \remark This function does not take into account aggregate waiting time.
   * Check on this should be performed before calling this function.
   *
   * \param channel A pointer to the channel we want to know the waiting time
   * for.
   * \return A Time instance containing the waiting time before transmission is
   * allowed on the channel.
   */
  Time GetWaitingTime (double frequencyHz);

  /**
   * Register the transmission of a packet.
   *
   * \param duration The duration of the transmission event.
   * \param channel The channel the transmission was made on.
   */
  void AddEvent (Time duration, double frequencyHz);

  /**
   * Add a new SubBand to this helper.
   *
   * \param firstFrequency The first frequency of the subband, in MHz.
   * \param lastFrequency The last frequency of the subband, in MHz.
   * \param dutyCycle The duty cycle that needs to be enforced on this subband.
   * \param maxTxPowerDbm The maximum transmission power [dBm] that can be used
   * on this SubBand.
   */
  void AddSubBand (double firstFrequency, double lastFrequency,
                   double dutyCycle, double maxTxPowerDbm);

  /**
   * Add a new SubBand.
   *
   * \param subBand A pointer to the SubBand that needs to be added.
   */
  void AddSubBand (Ptr<SubBand> subBand);

  /**
   * Returns the maximum transmission power [dBm] that is allowed on a channel.
   *
   * \param logicalChannel The power for which to check the maximum allowed
   * transmission power.
   * \return The power in dBm.
   */
  double GetTxPowerForFrequency (double frequencyHz);

  /**
   * Get the SubBand a frequency belongs to.
   *
   * \param frequency The frequency we want to check.
   * \return The SubBand the frequency belongs to.
   */
  Ptr<SubBand> GetSubBandFromFrequency (double frequency);

private:
  /**
   * A list of the SubBands that are currently registered within this helper.
   */
  std::list<Ptr <SubBand> > m_subBandList;

  Time m_nextAggregatedTransmissionTime; //!< The next time at which
  //!transmission will be possible
  //!according to the aggregated
  //!transmission timer

  double m_aggregatedDutyCycle; //!< The next time at which
  //!transmission will be possible
  //!according to the aggregated
  //!transmission timer

  Ptr<UniformRandomVariable> m_channelSelector;
};
}

}
#endif /* LOGICAL_SIGFOX_CHANNEL_HELPER_H */

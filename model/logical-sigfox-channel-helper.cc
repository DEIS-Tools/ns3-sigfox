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

#include "ns3/logical-sigfox-channel-helper.h"
#include "ns3/double.h"
#include "ns3/random-variable-stream.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("LogicalSigfoxChannelHelper");

NS_OBJECT_ENSURE_REGISTERED (LogicalSigfoxChannelHelper);

TypeId
LogicalSigfoxChannelHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LogicalSigfoxChannelHelper")
    .SetParent<Object> ()
    .SetGroupName ("sigfox");
  return tid;
}

LogicalSigfoxChannelHelper::LogicalSigfoxChannelHelper () :
  m_nextAggregatedTransmissionTime (Seconds (0)),
  m_aggregatedDutyCycle (1),
  m_channelSelector (CreateObject<UniformRandomVariable> ())
{
  NS_LOG_FUNCTION (this);
}

LogicalSigfoxChannelHelper::~LogicalSigfoxChannelHelper ()
{
  NS_LOG_FUNCTION (this);
}

void
LogicalSigfoxChannelHelper::SetFrequencyRange (double startFrequencyHz, double endFrequencyHz)
{
  m_channelSelector->SetAttribute ("Min", DoubleValue (startFrequencyHz));
  m_channelSelector->SetAttribute ("Max", DoubleValue (endFrequencyHz));
}

double
LogicalSigfoxChannelHelper::GetRandomFrequency (void)
{
  return m_channelSelector->GetValue();
}

double
LogicalSigfoxChannelHelper::GetFrequencyFromChannelSet (void)
{
  int channelNumber = m_channelSelector->GetInteger (0, 359);
  // Channels have a fixed 100 Hz bandwidth, starting at 868.180 MHz for channel 0, ending at 868.198
  // MHz for channel 180, restarting at 868.202 MHz for channel 220 and ending at 868.220 MHz for channel 400.
  if (channelNumber < 180)
    {
      return 868.180e6 + 50 + channelNumber * 100;
    }
  else
    {
      return 868.202e6 + 50 + (channelNumber - 180) * 100;
    }
}

Ptr<SubBand>
LogicalSigfoxChannelHelper::GetSubBandFromFrequency (double frequency)
{
  // Get the SubBand this frequency belongs to
  std::list< Ptr< SubBand > >::iterator it;
  for (it = m_subBandList.begin (); it != m_subBandList.end (); it++)
    {
      if ((*it)->BelongsToSubBand (frequency))
        {
          return *it;
        }
    }

  NS_LOG_ERROR ("Requested frequency: " << frequency);
  NS_ABORT_MSG ("Warning: frequency is outside any known SubBand.");

  return 0;     // If no SubBand is found, return 0
}

void
LogicalSigfoxChannelHelper::AddSubBand (double firstFrequency,
                                      double lastFrequency, double dutyCycle,
                                      double maxTxPowerDbm)
{
  NS_LOG_FUNCTION (this << firstFrequency << lastFrequency);

  Ptr<SubBand> subBand = Create<SubBand> (firstFrequency, lastFrequency,
                                          dutyCycle, maxTxPowerDbm);

  m_subBandList.push_back (subBand);
}

void
LogicalSigfoxChannelHelper::AddSubBand (Ptr<SubBand> subBand)
{
  NS_LOG_FUNCTION (this << subBand);

  m_subBandList.push_back (subBand);
}

Time
LogicalSigfoxChannelHelper::GetAggregatedWaitingTime (void)
{
  // Aggregate waiting time
  Time aggregatedWaitingTime = m_nextAggregatedTransmissionTime - Simulator::Now ();

  // Handle case in which waiting time is negative
  aggregatedWaitingTime = Seconds (std::max (aggregatedWaitingTime.GetSeconds (),
                                             double(0)));

  NS_LOG_DEBUG ("Aggregated waiting time: " << aggregatedWaitingTime.GetSeconds ());

  return aggregatedWaitingTime;
}

Time
LogicalSigfoxChannelHelper::GetWaitingTime (double frequencyHz)
{
  NS_LOG_FUNCTION (this << frequencyHz);

  // SubBand waiting time
  Time subBandWaitingTime = GetSubBandFromFrequency (frequencyHz)->
    GetNextTransmissionTime () -
    Simulator::Now ();

  // Handle case in which waiting time is negative
  subBandWaitingTime = Seconds (std::max (subBandWaitingTime.GetSeconds (),
                                          double(0)));

  NS_LOG_DEBUG ("Waiting time: " << subBandWaitingTime.GetSeconds ());

  return subBandWaitingTime;
}

void
LogicalSigfoxChannelHelper::AddEvent (Time duration, double frequencyHz)
{
  NS_LOG_FUNCTION (this << duration << frequencyHz);

  Ptr<SubBand> subBand = GetSubBandFromFrequency (frequencyHz);

  // TODO Fix this mess
  double dutyCycle = 0.0;
  double timeOnAir = duration.GetSeconds ();

  // Computation of necessary waiting time on this sub-band
  subBand->SetNextTransmissionTime (Simulator::Now () + Seconds (timeOnAir + dutyCycle));
  // Computation of necessary aggregate waiting time
  m_nextAggregatedTransmissionTime = Simulator::Now () + Seconds
      (timeOnAir / m_aggregatedDutyCycle - timeOnAir);

  NS_LOG_DEBUG ("Time on air: " << timeOnAir);
  NS_LOG_DEBUG ("m_aggregatedDutyCycle: " << m_aggregatedDutyCycle);
  NS_LOG_DEBUG ("Current time: " << Simulator::Now ().GetSeconds ());
  NS_LOG_DEBUG ("Next transmission on this sub-band allowed at time: " <<
                (subBand->GetNextTransmissionTime ()).GetSeconds ());
  NS_LOG_DEBUG ("Next aggregated transmission allowed at time " <<
                m_nextAggregatedTransmissionTime.GetSeconds ());
}

double
LogicalSigfoxChannelHelper::GetTxPowerForFrequency (double frequencyHz)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Get the maxTxPowerDbm from the SubBand this channel is in
  std::list< Ptr< SubBand > >::iterator it;
  for (it = m_subBandList.begin (); it != m_subBandList.end (); it++)
    {
      // Check whether this channel is in this SubBand
      if ((*it)->BelongsToSubBand (frequencyHz))
        {
          return (*it)->GetMaxTxPowerDbm ();
        }
    }
  NS_ABORT_MSG ("Frequency doesn't belong to a known SubBand");

  return 0;
}
}
}

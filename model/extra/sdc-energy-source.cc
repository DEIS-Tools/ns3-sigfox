/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 Network Security Lab, University of Washington, Seattle.
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
 * Authors: Michele Albano <mialb@cs.aau.dk>
 */

#include "sdc-energy-source.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/double.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/simulator.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SdcEnergySource");

NS_OBJECT_ENSURE_REGISTERED (SdcEnergySource);

TypeId
SdcEnergySource::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SdcEnergySource")
    .SetParent<EnergySource> ()
    .SetGroupName ("Energy")
    .AddConstructor<SdcEnergySource> ()
    .AddAttribute ("SdcEnergySourceInitialEnergyJ",
                   "Initial energy stored in basic energy source.",
                   DoubleValue (10),  // in Joules
                   MakeDoubleAccessor (&SdcEnergySource::SetInitialEnergy,
                                       &SdcEnergySource::GetInitialEnergy),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SdcEnergySupplyVoltageV",
                   "Initial supply voltage for basic energy source.",
                   DoubleValue (3.0), // in Volts
                   MakeDoubleAccessor (&SdcEnergySource::SetSupplyVoltage,
                                       &SdcEnergySource::GetSupplyVoltage),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SdcEnergyLowBatteryThreshold",
                   "Low battery threshold for basic energy source.",
                   DoubleValue (0.10), // as a fraction of the initial energy
                   MakeDoubleAccessor (&SdcEnergySource::m_lowBatteryTh),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SdcEnergyHighBatteryThreshold",
                   "High battery threshold for basic energy source.",
                   DoubleValue (0.15), // as a fraction of the initial energy
                   MakeDoubleAccessor (&SdcEnergySource::m_highBatteryTh),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SdcSelfDischargeParameter",
                   "Self discharge parameter for the SDC energy source.",
                   DoubleValue (0.97), // which percentage you lose in a month. Default is 3% -> 97% of the energy survives
                   MakeDoubleAccessor (&SdcEnergySource::m_sdcParameter),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SdcPeriodicEnergyUpdateInterval",
                   "Time between two consecutive periodic energy updates.",
                   TimeValue (Seconds (3600.0)),
                   MakeTimeAccessor (&SdcEnergySource::SetEnergyUpdateInterval,
                                     &SdcEnergySource::GetEnergyUpdateInterval),
                   MakeTimeChecker ())
    .AddTraceSource ("SdcRemainingEnergy",
                     "SdcRemaining energy at SdcEnergySource.",
                     MakeTraceSourceAccessor (&SdcEnergySource::m_remainingEnergyJ),
                     "ns3::TracedValueCallback::Double")
  ;
  return tid;
}

SdcEnergySource::SdcEnergySource ()
{
  NS_LOG_FUNCTION (this);
  m_lastUpdateTime = Seconds (0.0);
  m_depleted = false;
}

SdcEnergySource::~SdcEnergySource ()
{
  NS_LOG_FUNCTION (this);
}

void
SdcEnergySource::UpdateEnergySource (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("SdcEnergySource:Updating remaining energy.");

  double remainingEnergy = m_remainingEnergyJ;
  SdcCalculateRemainingEnergy ();

  m_lastUpdateTime = Simulator::Now ();

  if (!m_depleted && m_remainingEnergyJ <= m_lowBatteryTh * m_initialEnergyJ)
    {
      m_depleted = true;
      HandleEnergyDrainedEvent ();
    }
  else if (m_depleted && m_remainingEnergyJ > m_highBatteryTh * m_initialEnergyJ)
    {
      m_depleted = false;
      HandleEnergyRechargedEvent ();
    }
  else if (m_remainingEnergyJ != remainingEnergy)
    {
      NotifyEnergyChanged ();
    }

  if (m_energyUpdateEvent.IsExpired ())
    {
      m_energyUpdateEvent = Simulator::Schedule (m_energyUpdateInterval,
                                                 &SdcEnergySource::UpdateEnergySource,
                                                 this);
    }
}

/*
 * Private functions start here.
 */

void
SdcEnergySource::SdcCalculateRemainingEnergy (void)
{
  NS_LOG_FUNCTION (this);
  double totalCurrentA = CalculateTotalCurrent ();
  Time duration = Simulator::Now () - m_lastUpdateTime;
  NS_ASSERT (duration.IsPositive ());
  // energy = current * voltage * time
  double remainingEnergyJ = 0;
  if (m_sdcParameter > 0) {
	  double param = -log(m_sdcParameter) / 2592000;
	  double tmp = totalCurrentA * m_supplyVoltageV / param;
	  remainingEnergyJ = exp(-param*duration.GetSeconds()) * (m_remainingEnergyJ + tmp) - tmp;
//	  fprintf(stderr, "I am actually %E %E %E.\n", param, tmp, Simulator::Now().GetSeconds());
  } else {
	  remainingEnergyJ -= (totalCurrentA * m_supplyVoltageV * duration).GetSeconds();
  }
//  fprintf(stderr, "I am finally in SdcCalculateRemainingEnergy.\n\tvoltage %lf current %f time elapsed %f energy from %f to %f\n", m_supplyVoltageV, totalCurrentA, duration.GetSeconds(), (double)m_remainingEnergyJ, remainingEnergyJ);

  NS_ASSERT (remainingEnergyJ >= 0);
  m_remainingEnergyJ = remainingEnergyJ;
  NS_LOG_DEBUG ("SdcEnergySource:Remaining energy = " << m_remainingEnergyJ);
}

} // namespace ns3

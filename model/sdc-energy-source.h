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

#ifndef SDC_ENERGY_SOURCE_H
#define SDC_ENERGY_SOURCE_H

#include "ns3/traced-value.h"
#include "ns3/nstime.h"
#include "ns3/event-id.h"
#include "ns3/energy-source.h"

namespace ns3 {

/**
 * \ingroup energy
 * SdcEnergySource is a BasicEnergySource with Self-Discharge Current.
 *
 */
class SdcEnergySource : public EnergySource
{
public:
  static TypeId GetTypeId (void);
  SdcEnergySource();
  virtual ~SdcEnergySource();

  /**
   * \return Initial energy stored in energy source, in Joules.
   *
   * Implements GetInitialEnergy.
   */
  virtual double GetInitialEnergy (void) const;

  /**
   * \returns Supply voltage at the energy source.
   *
   * Implements GetSupplyVoltage.
   */
  virtual double GetSupplyVoltage (void) const;

  /**
   * \return Remaining energy in energy source, in Joules
   *
   * Implements GetRemainingEnergy.
   */
  virtual double GetRemainingEnergy (void);

  /**
   * \returns Energy fraction.
   *
   * Implements GetEnergyFraction.
   */
  virtual double GetEnergyFraction (void);

  /**
   * Implements UpdateEnergySource.
   */
  virtual void UpdateEnergySource (void);

  /**
   * \param initialEnergyJ Initial energy, in Joules
   *
   * Sets initial energy stored in the energy source. Note that initial energy
   * is assumed to be set before simulation starts and is set only once per
   * simulation.
   */
  void SetInitialEnergy (double initialEnergyJ);

  /**
   * \param supplyVoltageV Supply voltage at the energy source, in Volts.
   *
   * Sets supply voltage of the energy source.
   */
  void SetSupplyVoltage (double supplyVoltageV);

  /**
   * \param interval Energy update interval.
   *
   * This function sets the interval between each energy update.
   */
  void SetEnergyUpdateInterval (Time interval);

  /**
   * \returns The interval between each energy update.
   */
  Time GetEnergyUpdateInterval (void) const;



private:

  /// Defined in ns3::Object
  void DoInitialize (void);

  /// Defined in ns3::Object
  void DoDispose (void);

  /**
   * Handles the remaining energy going to zero event. This function notifies
   * all the energy models aggregated to the node about the energy being
   * depleted. Each energy model is then responsible for its own handler.
   */
  void HandleEnergyDrainedEvent (void);

  /**
   * Handles the remaining energy exceeding the high threshold after it went
   * below the low threshold. This function notifies all the energy models
   * aggregated to the node about the energy being recharged. Each energy model
   * is then responsible for its own handler.
   */
  void HandleEnergyRechargedEvent (void);

  /**
   * Calculates remaining energy. This function uses the total current from all
   * device models to calculate the amount of energy to decrease. 
   * This function considers also the self-discharge current.
   */
  void SdcCalculateRemainingEnergy(void);

private:
  double m_initialEnergyJ;                //!< initial energy, in Joules
  double m_supplyVoltageV;                //!< supply voltage, in Volts
  double m_lowBatteryTh;                  //!< low battery threshold, as a fraction of the initial energy
  double m_highBatteryTh;                 //!< high battery threshold, as a fraction of the initial energy
  /**
   * set to true when the remaining energy goes below the low threshold,
   * set to false again when the remaining energy exceeds the high threshold
   */
  bool m_depleted;
  TracedValue<double> m_remainingEnergyJ; //!< remaining energy, in Joules
  EventId m_energyUpdateEvent;            //!< energy update event
  Time m_lastUpdateTime;                  //!< last update time
  Time m_energyUpdateInterval;            //!< energy update interval
  double m_sdcParameter;                  // how much energy will be in your battery after one month
};

} // namespace ns3

#endif /* SDC_ENERGY_SOURCE_H */

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

#include "ns3/basic-energy-source.h"

namespace ns3 {

/**
 * \ingroup energy
 * SdcEnergySource is a BasicEnergySource with Self-Discharge Current.
 *
 */
class SdcEnergySource : public BasicEnergySource
{
public:
  static TypeId GetTypeId (void);
  SdcEnergySource();
  virtual ~SdcEnergySource();

  /**
   * Implements UpdateEnergySource.
   */
  virtual void UpdateEnergySource (void);


private:

  /**
   * Calculates remaining energy. This function uses the total current from all
   * device models to calculate the amount of energy to decrease. 
   * This function considers also the self-discharge current.
   */
  void SdcCalculateRemainingEnergy(void);

private:
  double m_sdcParameter;                  // how much energy will be in your battery after one month
};

} // namespace ns3

#endif /* SDC_ENERGY_SOURCE_H */

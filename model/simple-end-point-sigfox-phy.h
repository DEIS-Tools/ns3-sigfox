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
 *
 */

#ifndef SIMPLE_END_DEVICE_SIGFOX_PHY_H
#define SIMPLE_END_DEVICE_SIGFOX_PHY_H

#include "ns3/object.h"
#include "ns3/traced-value.h"
#include "ns3/net-device.h"
#include "ns3/nstime.h"
#include "ns3/mobility-model.h"
#include "ns3/node.h"
#include "ns3/end-point-sigfox-phy.h"

namespace ns3 {
namespace sigfox {

class SigfoxChannel;

/**
 * Class representing a simple Sigfox transceiver, with an error model based
 * on receiver sensitivity and a SIR table.
 */
class SimpleEndPointSigfoxPhy : public EndPointSigfoxPhy
{
public:
  static TypeId GetTypeId (void);

  // Constructor and destructor
  SimpleEndPointSigfoxPhy ();
  virtual ~SimpleEndPointSigfoxPhy ();

  // Implementation of EndPointSigfoxPhy's pure virtual functions
  virtual void StartReceive (Ptr<Packet> packet, double rxPowerDbm,
                             Time duration, double frequencyMHz);

  // Implementation of SigfoxPhy's pure virtual functions
  virtual void EndReceive (Ptr<Packet> packet,
                           Ptr<SigfoxInterferenceHelper::Event> event);

  // Implementation of SigfoxPhy's pure virtual functions
  virtual void Send (Ptr<Packet> packet, SigfoxTxParameters txParams,
                     double frequencyHz, double txPowerDbm);

private:
};

} /* namespace ns3 */

}
#endif /* SIMPLE_END_DEVICE_SIGFOX_PHY_H */

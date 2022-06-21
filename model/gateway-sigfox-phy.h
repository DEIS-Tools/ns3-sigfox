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

#ifndef GATEWAY_SIGFOX_PHY_H
#define GATEWAY_SIGFOX_PHY_H

#include "ns3/object.h"
#include "ns3/net-device.h"
#include "ns3/nstime.h"
#include "ns3/mobility-model.h"
#include "ns3/node.h"
#include "ns3/sigfox-phy.h"
#include "ns3/traced-value.h"
#include <list>

namespace ns3 {
namespace sigfox {

class SigfoxChannel;

/**
 * Class modeling a Sigfox SX1301 chip.
 *
 * This class models the behaviour of the chip employed in Sigfox gateways. These
 * chips are characterized by the presence of 8 receive paths, or parallel
 * receivers, which can be employed to listen to different channels
 * simultaneously. This characteristic of the chip is modeled using the
 * ReceivePath class, which describes a single parallel receiver. GatewaySigfoxPhy
 * essentially holds and manages a collection of these objects.
 */
class GatewaySigfoxPhy : public SigfoxPhy
{
public:
  static TypeId GetTypeId (void);

  GatewaySigfoxPhy ();
  virtual ~GatewaySigfoxPhy ();

  virtual void StartReceive (Ptr<Packet> packet, double rxPowerDbm, Time duration, double frequencyMHz) = 0;

  virtual void EndReceive (Ptr<Packet> packet,
                           Ptr<SigfoxInterferenceHelper::Event> event) = 0;

  virtual void Send (Ptr<Packet> packet, SigfoxTxParameters txParams,
                     double frequencyMHz, double txPowerDbm) = 0;

  virtual void TxFinished (Ptr<Packet> packet);

  bool IsTransmitting (void);

  virtual bool IsOnFrequency (double frequencyMHz);

  /**
   * A vector containing the sensitivities required to correctly decode
   * different spreading factors.
   */
  static const double sensitivity;

protected:
  /**
   * This class represents a configurable reception path.
   *
   * ReceptionPaths are configured to listen on a certain frequency. Differently
   * from EndPointSigfoxPhys, these do not need to be configured to listen for a
   * certain SF. ReceptionPaths be either locked on an event or free.
   */
  class ReceptionPath : public SimpleRefCount<GatewaySigfoxPhy::ReceptionPath>
  {

public:
    /**
     * Constructor.
     *
     * \param frequencyMHz The frequency this path is set to listen on.
     */
    ReceptionPath (double frequencyMHz);

    ~ReceptionPath ();

    /**
     * Getter for the operating frequency.
     *
     * \return The frequency this ReceivePath is configured to listen on.
     */
    double GetFrequency (void);

    /**
     * Setter for the frequency.
     *
     * \param frequencyMHz The frequency [MHz] this ReceptionPath will listen on.
     */
    void SetFrequency (double frequencyMHz);

    /**
     * Query whether this reception path is available to lock on a signal.
     *
     * \return True if its current state is free, false if it's currently locked.
     */
    bool IsAvailable (void);

    /**
     * Set this reception path as available.
     *
     * This function sets the m_available variable as true, and deletes the
     * SigfoxInterferenceHelper Event this ReceivePath was previously locked on.
     */
    void Free (void);

    /**
     * Set this reception path as not available and lock it on the
     * provided event.
     *
     * \param event The SigfoxInterferenceHelper Event to lock on.
     */
    void LockOnEvent (Ptr<SigfoxInterferenceHelper::Event> event);

    /**
     * Set the event this reception path is currently on.
     *
     * \param event the event to lock this ReceptionPath on.
     */
    void SetEvent (Ptr<SigfoxInterferenceHelper::Event> event);

    /**
     * Get the event this reception path is currently on.
     *
     * \returns 0 if no event is currently being received, a pointer to
     * the event otherwise.
     */
    Ptr<SigfoxInterferenceHelper::Event> GetEvent (void);

    /**
     * Get the EventId of the EndReceive call associated to this ReceptionPath's
     * packet.
     */
    EventId GetEndReceive (void);

    /**
     * Set the EventId of the EndReceive call associated to this ReceptionPath's
     * packet.
     */
    void SetEndReceive (EventId endReceiveEventId);

private:
    /**
     * The frequency this path is currently listening on, in MHz.
     */
    double m_frequencyMHz;

    /**
     * Whether this reception path is available to lock on a signal or not.
     */
    bool m_available;

    /**
     * The event this reception path is currently locked on.
     */
    Ptr< SigfoxInterferenceHelper::Event > m_event;

    /**
     * The EventId associated of the call to EndReceive that is scheduled to
     * happen when the packet this ReceivePath is locked on finishes reception.
     */
    EventId m_endReceiveEventId;
  };

  /**
   * The number of occupied reception paths.
   */
  TracedValue<int> m_occupiedReceptionPaths;

  /**
   * Trace source that is fired when a packet cannot be received because all
   * available ReceivePath instances are busy.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet>, uint32_t> m_noMoreDemodulators;

  /**
   * Trace source that is fired when a packet cannot be received because
   * the Gateway is in transmission state.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet>, uint32_t> m_noReceptionBecauseTransmitting;

  bool m_isTransmitting; //!< Flag indicating whether a transmission is going on
};

} /* namespace ns3 */

}
#endif /* GATEWAY_SIGFOX_PHY_H */

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
 *         Muhammad Naeem <mnaeem@cs.aau.dk>
 *
 * Modified by: Michele Albano <mialb@cs.aau.dk>
 */

#ifndef SIGFOX_NET_DEVICE_H
#define SIGFOX_NET_DEVICE_H

#include "ns3/net-device.h"
#include "ns3/sigfox-channel.h"
#include "ns3/sigfox-phy.h"
#include "ns3/sigfox-mac.h"

namespace ns3 {
namespace sigfox {

class SigfoxChannel;
class SigfoxPhy;
class SigfoxMac;

/**
 * Hold together all Sigfox related objects.
 *
 * This class holds together pointers to SigfoxChannel, SigfoxPhy and SigfoxMac,
 * exposing methods through which Application instances can send packets. The
 * Application only needs to craft its packets, the NetDevice will take care of
 * calling the SigfoxMac's Send method with the appropriate parameters.
 */
class SigfoxNetDevice : public NetDevice
{
public:
  static TypeId GetTypeId (void);

  // Constructor and destructor
  SigfoxNetDevice ();
  virtual ~SigfoxNetDevice ();

  /**
   * Set which SigfoxMac instance is linked to this device.
   *
   * \param mac the mac layer to use.
   */
  void SetMac (Ptr<SigfoxMac> mac);

  /**
   * Set which SigfoxPhy instance is linked to this device.
   *
   * \param phy the phy layer to use.
   */
  void SetPhy (Ptr<SigfoxPhy> phy);

  /**
   * Get the SigfoxMac instance that is linked to this NetDevice.
   *
   * \return the mac we are currently using.
   */
  Ptr<SigfoxMac> GetMac (void) const;

  /**
   * Get the SigfoxPhy instance that is linked to this NetDevice.
   *
   * \return the phy we are currently using.
   */
  Ptr<SigfoxPhy> GetPhy (void) const;

  /**
   * Send a packet through the Sigfox stack.
   *
   * \param packet The packet to send.
   */
  void Send (Ptr<Packet> packet);

  /**
   * This function is implemented to achieve compliance with the NetDevice
   * interface. Note that the dest and protocolNumber args are ignored.
   */
  bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);

  /**
   * Callback the Mac layer calls whenever a packet arrives and needs to be
   * forwarded up the stack.
   *
   * \param packet The packet that was received.
   */
  void Receive (Ptr<Packet> packet);

  // From class NetDevice. Some of these have little meaning for a Sigfox
  // network device (since, for instance, IP is not used in the standard)
  virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb);
  virtual Ptr<Channel> GetChannel (void) const;
  virtual void SetNode (Ptr<Node> node);
  virtual Ptr<Node> GetNode (void) const;

  virtual void SetIfIndex (const uint32_t index);
  virtual uint32_t GetIfIndex (void) const;
  virtual void SetAddress (Address address);
  virtual Address GetAddress (void) const;
  virtual bool SetMtu (const uint16_t mtu);
  virtual uint16_t GetMtu (void) const;
  virtual bool IsLinkUp (void) const;
  virtual void AddLinkChangeCallback (Callback<void> callback);
  virtual bool IsBroadcast (void) const;
  virtual Address GetBroadcast (void) const;
  virtual bool IsMulticast (void) const;
  virtual Address GetMulticast (Ipv4Address multicastGroup) const;
  virtual Address GetMulticast (Ipv6Address addr) const;
  virtual bool IsBridge (void) const;
  virtual bool IsPointToPoint (void) const;
  virtual bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);
  virtual bool NeedsArp (void) const;
  virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb);
  virtual bool SupportsSendFrom (void) const;

protected:
  /**
   * Receive a packet from the lower layer and pass the
   * packet up the stack.
   *
   * \param packet The packet we need to forward.
   * \param from The from address.
   * \param to The to address.
   */
  void ForwardUp (Ptr<Packet> packet, Mac48Address from, Mac48Address to);

private:
  /**
   * Return the SigfoxChannel this device is connected to.
   */
  Ptr<SigfoxChannel> DoGetChannel (void) const;

  /**
   * Complete the configuration of this Sigfox device by connecting all lower
   * components (PHY, MAC, Channel) together.
   */
  void CompleteConfig (void);

  // Member variables
  Ptr<Node> m_node; //!< The Node this NetDevice is connected to.
  Ptr<SigfoxPhy> m_phy; //!< The SigfoxPhy this NetDevice is connected to.
  Ptr<SigfoxMac> m_mac; //!< The SigfoxMac this NetDevice is connected to.
  bool m_configComplete; //!< Whether the configuration was already completed.

  /**
   * Upper layer callback used for notification of new data packet arrivals.
   */
  NetDevice::ReceiveCallback m_receiveCallback;
};

} //namespace ns3

}
#endif /* SIGFOX_NET_DEVICE_H */

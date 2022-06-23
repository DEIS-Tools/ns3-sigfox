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

#ifndef END_POINT_SIGFOX_MAC_H
#define END_POINT_SIGFOX_MAC_H

#include "ns3/sigfox-mac.h"
#include "ns3/sigfox-mac-header.h"
#include "ns3/random-variable-stream.h"
#include "ns3/traced-value.h"

namespace ns3 {
namespace sigfox {

/**
 * Class representing the MAC layer of a Sigfox device.
 */
class EndPointSigfoxMac : public SigfoxMac
{
public:
  static TypeId GetTypeId (void);

  EndPointSigfoxMac ();
  virtual ~EndPointSigfoxMac ();

  /////////////////////
  // Sending methods //
  /////////////////////

  /**
   * Send a packet.
   *
   * The MAC layer of the ED will take care of using the right parameters.
   *
   * \param packet the packet to send
   */
  virtual void Send (Ptr<Packet> packet);

  /**
   * Checking if we are performing the transmission of a new packet or a retransmission, and call SendToPhy function.
   *
   * \param packet the packet to send
   */
  virtual void DoSend (Ptr<Packet> packet);

  /**
  * Add headers and send a packet with the sending function of the physical layer.
  *
  * \param packet the packet to send
  */
  virtual void SendToPhy (Ptr<Packet> packet);

  ///////////////////////
  // Receiving methods //
  ///////////////////////

  /**
   * Receive a packet.
   *
   * This method is typically registered as a callback in the underlying PHY
   * layer so that it's called when a packet is going up the stack.
   *
   * \param packet the received packet.
   */
  virtual void Receive (Ptr<Packet const> packet);

  virtual void FailedReception (Ptr<Packet const> packet);

  /**
   * Perform the actions that are required after a packet send.
   *
   * This function handles opening of the first receive window.
   */
  virtual void TxFinished (Ptr<const Packet> packet);

  /////////////////////////
  // Getters and Setters //
  /////////////////////////

  /**
   * Set the maximum number of transmissions allowed.
   *
   * \param maxNumbTx The maximum number of transmissions allowed
   */
  void SetMaxNumberOfTransmissions (uint8_t maxNumbTx);

  /**
   * Set the maximum number of transmissions allowed.
   */
  uint8_t GetMaxNumberOfTransmissions (void);

  /**
   * Get the transmission power this end device is set to use.
   *
   * \return The transmission power this device uses when transmitting.
   */
  virtual uint8_t GetTransmissionPower (void);

  /**
   * Get the aggregated duty cycle.
   *
   * \return A time instance containing the aggregated duty cycle in fractional
   * form.
   */
  double GetAggregatedDutyCycle (void);

  /////////////////////////
  // MAC command methods //
  /////////////////////////

  /**
   * Add the necessary options and MAC commands to the SigfoxMacHeader.
   *
   * \param macHeader The mac header on which to apply the options.
   */
  void ApplyNecessaryOptions (SigfoxMacHeader &macHeader);

  /**
   * Set the message type to send when the Send method is called.
   */
  void SetMType (SigfoxMacHeader::MType mType);

  /**
   * Get the message type to send when the Send method is called.
   */
  SigfoxMacHeader::MType GetMType (void);

  // Logical channel administration //
  ////////////////////////////////////

  /**
   * Add a logical channel to the helper.
   *
   * \param frequency The channel's center frequency.
   */
  void AddLogicalChannel (double frequency);

  /**
   * Set a new logical channel in the helper.
   *
   * \param chIndex The channel's new index.
   * \param frequency The channel's center frequency.
   */
  void SetLogicalChannel (uint8_t chIndex, double frequency);

  /**
   * Add a logical channel to the helper.
   *
   * \param frequency The channel's center frequency.
   */
  void AddLogicalChannel (Ptr<LogicalSigfoxChannel> logicalChannel);

  /**
   * Add a subband to the logical channel helper.
   *
   * \param startFrequency The SubBand's lowest frequency.
   * \param endFrequency The SubBand's highest frequency.
   * \param dutyCycle The SubBand's duty cycle, in fraction form.
   * \param maxTxPowerDbm The maximum transmission power allowed on the SubBand.
   */
  void AddSubBand (double startFrequency, double endFrequency, double dutyCycle,
                   double maxTxPowerDbm);

  void OpenFirstReceiveWindow (void);

  void CloseFirstReceiveWindow (void);

  void SendCtrlMessage (void);
    void ResetCtrlcntr (void);
    void BDPFrequency(int BDPF);
protected:

  bool m_sendCtrlMsg = true;
  uint32_t m_sendCount = 0;
  uint8_t m_appPacketCount = 0;
  bool m_packetReceived = false;
    int ctrl_send_cnt=0;
    int msg_cnt=0;
    int updateBDP; //BiDirectionalProcedureFrequency
  /**
   * Enable Data Rate adaptation during the retransmission procedure.
   */
  bool m_enableDRAdapt;

  /**
   * Maximum number of transmission allowed.
   */
  uint8_t m_maxNumbTx;

  /**
   * The transmission power this device is using to transmit.
   */
  TracedValue<double> m_txPower;

  /**
   * The coding rate used by this device.
   */
  uint8_t m_codingRate;

  /**
   * Whether or not the header is disabled for communications by this device.
   */
  bool m_headerDisabled;

  /**
   * The duration of a receive window in number of symbols. This should be
   * converted to time based or the reception parameter used.
   *
   * The downlink preamble transmitted by the gateways contains 8 symbols.
   * The receiver requires 5 symbols to detect the preamble and synchronize.
   * Therefore there must be a 5 symbols overlap between the receive window
   * and the transmitted preamble.
   * (Ref: Recommended SX1272/76 Settings for EU868 Sigfox Network Operation )
   */
  uint8_t m_receiveWindowDurationInSymbols;

  /**
   * An uniform random variable, used by the Shuffle method to randomly reorder
   * the channel list.
   */
  Ptr<UniformRandomVariable> m_uniformRV;

  /////////////////
  //  Callbacks  //
  /////////////////

  /**
   * The trace source fired when the transmission procedure is finished.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<uint8_t, bool, Time, Ptr<Packet> > m_requiredTxCallback;

private:
  /**
   * Find the minimum waiting time before the next possible transmission.
   */
  Time GetNextTransmissionDelay (void);

  /**
   * The event of retransmitting a packet in a consecutive moment if an ACK is not received.
   *
   * This Event is used to cancel the retransmission if the ACK is found in ParseCommand function and
   * if a newer packet is delivered from the application to be sent.
   */
  EventId m_nextTx;

  /**
   * The event of transmitting a packet in a consecutive moment, when the duty cycle let us transmit.
   *
   * This Event is used to cancel the transmission of this packet if a newer packet is delivered from the application to be sent.
   */
  EventId m_nextRetx;

  /**
   * The last known link margin.
   *
   * This value is obtained (and updated) when a LinkCheckAns Mac command is
   * received.
   */
  TracedValue<double> m_lastKnownLinkMargin;

  /**
   * The last known gateway count (i.e., gateways that are in communication
   * range with this end device)
   *
   * This value is obtained (and updated) when a LinkCheckAns Mac command is
   * received.
   */
  TracedValue<int> m_lastKnownGatewayCount;

  /**
   * The aggregated duty cycle this device needs to respect across all sub-bands.
   */
  TracedValue<double> m_aggregatedDutyCycle;

  /**
   * The message type to apply to packets sent with the Send method.
   */
  SigfoxMacHeader::MType m_mType;

  uint8_t m_currentFCnt;

  uint32_t m_nRepetitions;
};


} /* namespace ns3 */

}
#endif /* END_POINT_SIGFOX_MAC_H */

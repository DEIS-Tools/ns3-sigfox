/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 *
 * Author: Davide Magrin <davide@magr.in>
 *
 * Modified by: Michele Albano <mialb@cs.aau.dk>
 */

#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/pointer.h"
#include "ns3/energy-source.h"
#include "sigfox-radio-energy-model.h"

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("SigfoxRadioEnergyModel");

NS_OBJECT_ENSURE_REGISTERED (SigfoxRadioEnergyModel);

TypeId
SigfoxRadioEnergyModel::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::SigfoxRadioEnergyModel")
          .SetParent<DeviceEnergyModel> ()
          .SetGroupName ("Energy")
          .AddConstructor<SigfoxRadioEnergyModel> ()
          .AddAttribute ("StandbyCurrentA", "The default radio Standby current in Ampere.",
                         DoubleValue (4.3), // idle mode = 1.4mA
                         MakeDoubleAccessor (&SigfoxRadioEnergyModel::SetStandbyCurrentA,
                                             &SigfoxRadioEnergyModel::GetStandbyCurrentA),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("TxCurrentA", "The radio Tx current in Ampere.",
                         DoubleValue (47), // transmit at 0dBm = 28mA
                         MakeDoubleAccessor (&SigfoxRadioEnergyModel::SetTxCurrentA,
                                             &SigfoxRadioEnergyModel::GetTxCurrentA),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("RxCurrentA", "The radio Rx current in Ampere.",
                         DoubleValue (19), // receive mode = 11.2mA
                         MakeDoubleAccessor (&SigfoxRadioEnergyModel::SetRxCurrentA,
                                             &SigfoxRadioEnergyModel::GetRxCurrentA),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("SleepCurrentA", "The radio Sleep current in Ampere.",
                         DoubleValue (0.027), // sleep mode = 1.5microA
                         MakeDoubleAccessor (&SigfoxRadioEnergyModel::SetSleepCurrentA,
                                             &SigfoxRadioEnergyModel::GetSleepCurrentA),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("TxCurrentModel", "A pointer to the attached tx current model.",
                         PointerValue (),
                         MakePointerAccessor (&SigfoxRadioEnergyModel::m_txCurrentModel),
                         MakePointerChecker<SigfoxTxCurrentModel> ())
          .AddTraceSource (
              "TotalEnergyConsumption", "Total energy consumption of the radio device.",
              MakeTraceSourceAccessor (&SigfoxRadioEnergyModel::m_totalEnergyConsumption),
              "ns3::TracedValueCallback::Double")
          .AddTraceSource ("SystemCurrent", "System Current of the radio device.",
                           MakeTraceSourceAccessor (&SigfoxRadioEnergyModel::m_systemcurrent),
                           "ns3::TracedValueCallback::Double");
  return tid;
}

SigfoxRadioEnergyModel::SigfoxRadioEnergyModel ()
{
  NS_LOG_FUNCTION (this);
  m_currentState = EndPointSigfoxPhy::SLEEP; // initially STANDBY
  m_lastUpdateTime = Seconds (0.0);
  m_nPendingChangeState = 0;
  m_isSupersededChangeState = false;
  m_energyDepletionCallback.Nullify ();
  m_source = NULL;
  // set callback for EndPointSigfoxPhy listener
  m_listener = new SigfoxRadioEnergyModelPhyListener;
  m_listener->SetChangeStateCallback (MakeCallback (&DeviceEnergyModel::ChangeState, this));
  // set callback for updating the tx current
  m_listener->SetUpdateTxCurrentCallback (
      MakeCallback (&SigfoxRadioEnergyModel::SetTxCurrentFromModel, this));
}

SigfoxRadioEnergyModel::~SigfoxRadioEnergyModel ()
{
  NS_LOG_FUNCTION (this);
  delete m_listener;
}

void
SigfoxRadioEnergyModel::SetEnergySource (Ptr<EnergySource> source)
{
  NS_LOG_FUNCTION (this << source);
  NS_ASSERT (source != NULL);
  m_source = source;
}

double
SigfoxRadioEnergyModel::GetTotalEnergyConsumption (void) const
{
  NS_LOG_FUNCTION (this);
  return m_totalEnergyConsumption;
}

double
SigfoxRadioEnergyModel::GetStandbyCurrentA (void) const
{
  NS_LOG_FUNCTION (this);
  return m_idleCurrentA;
}

void
SigfoxRadioEnergyModel::SetStandbyCurrentA (double idleCurrentA)
{
  NS_LOG_FUNCTION (this << idleCurrentA);
  m_idleCurrentA = idleCurrentA;
}

double
SigfoxRadioEnergyModel::GetTxCurrentA (void) const
{
  NS_LOG_FUNCTION (this);
  return m_txCurrentA;
}

void
SigfoxRadioEnergyModel::SetTxCurrentA (double txCurrentA)
{
  NS_LOG_FUNCTION (this << txCurrentA);
  m_txCurrentA = txCurrentA;
}

double
SigfoxRadioEnergyModel::GetRxCurrentA (void) const
{
  NS_LOG_FUNCTION (this);
  return m_rxCurrentA;
}

void
SigfoxRadioEnergyModel::SetRxCurrentA (double rxCurrentA)
{
  NS_LOG_FUNCTION (this << rxCurrentA);
  m_rxCurrentA = rxCurrentA;
}

double
SigfoxRadioEnergyModel::GetSleepCurrentA (void) const
{
  NS_LOG_FUNCTION (this);
  return m_sleepCurrentA;
}

void
SigfoxRadioEnergyModel::SetSleepCurrentA (double sleepCurrentA)
{
  NS_LOG_FUNCTION (this << sleepCurrentA);
  m_sleepCurrentA = sleepCurrentA;
}

EndPointSigfoxPhy::State
SigfoxRadioEnergyModel::GetCurrentState (void) const
{
  NS_LOG_FUNCTION (this);
  return m_currentState;
}

void
SigfoxRadioEnergyModel::SetEnergyDepletionCallback (SigfoxRadioEnergyDepletionCallback callback)
{
  NS_LOG_FUNCTION (this);
  if (callback.IsNull ())
    {
      NS_LOG_DEBUG ("SigfoxRadioEnergyModel:Setting NULL energy depletion callback!");
    }
  m_energyDepletionCallback = callback;
}

void
SigfoxRadioEnergyModel::SetEnergyRechargedCallback (SigfoxRadioEnergyRechargedCallback callback)
{
  NS_LOG_FUNCTION (this);
  if (callback.IsNull ())
    {
      NS_LOG_DEBUG ("SigfoxRadioEnergyModel:Setting NULL energy recharged callback!");
    }
  m_energyRechargedCallback = callback;
}

void
SigfoxRadioEnergyModel::SetTxCurrentModel (Ptr<SigfoxTxCurrentModel> model)
{
  m_txCurrentModel = model;
}

void
SigfoxRadioEnergyModel::SetTxCurrentFromModel (double txPowerDbm)
{
  if (m_txCurrentModel)
    {
      m_txCurrentA = m_txCurrentModel->CalcTxCurrent (txPowerDbm);
    }
}

void
SigfoxRadioEnergyModel::ChangeState (int newState)
{
  NS_LOG_FUNCTION (this << newState);

  Time duration = Simulator::Now () - m_lastUpdateTime;
  NS_ASSERT (duration.GetNanoSeconds () >= 0); // check if duration is valid

  // energy to decrease = current * voltage * time
  double energyToDecrease = 0.0;
  double xx = 0.0;
  double dd = 0.0;
  double supplyVoltage = m_source->GetSupplyVoltage ();
  NS_LOG_DEBUG ("m_currentState " << m_currentState);
  switch (m_currentState)
    {
    case EndPointSigfoxPhy::STANDBY:
      energyToDecrease = duration.GetSeconds () * m_idleCurrentA; // * supplyVoltage;
      xx = m_idleCurrentA;
      dd = duration.GetSeconds ();
      break;
    case EndPointSigfoxPhy::TX:
      energyToDecrease = duration.GetSeconds () * m_txCurrentA; // * supplyVoltage;
      xx = m_txCurrentA;
      dd = duration.GetSeconds ();
      break;
    case EndPointSigfoxPhy::RX:
      energyToDecrease = duration.GetSeconds () * m_rxCurrentA; // * supplyVoltage;
      xx = m_rxCurrentA;
      dd = duration.GetSeconds ();
      break;
    case EndPointSigfoxPhy::SLEEP:
      energyToDecrease = duration.GetSeconds () * m_sleepCurrentA; // * supplyVoltage;
      xx = m_sleepCurrentA;
      dd = duration.GetSeconds ();
      break;
    default:
      NS_FATAL_ERROR ("SigfoxRadioEnergyModel:Undefined radio state: " << m_currentState);
    }

  // new sate current
  switch (newState)
    {
    case EndPointSigfoxPhy::STANDBY:
      xx = m_idleCurrentA;
      break;
    case EndPointSigfoxPhy::TX:
      xx = m_txCurrentA;
      break;
    case EndPointSigfoxPhy::RX:
      xx = m_rxCurrentA;
      break;
    case EndPointSigfoxPhy::SLEEP:
      xx = m_sleepCurrentA;
      break;
    default:
      NS_FATAL_ERROR ("SigfoxRadioEnergyModel:Undefined radio state: " << m_currentState);
    }
  // update total energy consumption
  m_totalEnergyConsumption += energyToDecrease;

  // update last update time stamp
  m_lastUpdateTime = Simulator::Now ();

  m_nPendingChangeState++;

  // notify energy source
  m_source->UpdateEnergySource ();

  // in case the energy source is found to be depleted during the last update, a callback might be
  // invoked that might cause a change in the Sigfox PHY state (e.g., the PHY is put into SLEEP mode).
  // This in turn causes a new call to this member function, with the consequence that the previous
  // instance is resumed after the termination of the new instance. In particular, the state set
  // by the previous instance is erroneously the final state stored in m_currentState. The check below
  // ensures that previous instances do not change m_currentState.

  if (!m_isSupersededChangeState)
    {
      // update current state & last update time stamp
      SetSigfoxRadioState ((EndPointSigfoxPhy::State) newState);
      m_systemcurrent = xx;
      // some debug message
      NS_LOG_DEBUG ("SigfoxRadioEnergyModel:Total energy consumption is "
                    << m_totalEnergyConsumption << "J");
      NS_LOG_DEBUG (" newState" << newState << " energyToDecrease" << energyToDecrease << " Current"
                                << xx << " Duration" << dd << " supplyVoltage" << supplyVoltage
                                << "m_currentState" << m_currentState);
      //NS_LOG_DEBUG ("SigfoxRadioEnergyModel:Total energy consumption is " << m_totalEnergyConsumption << "J");
    }

  m_isSupersededChangeState = (m_nPendingChangeState > 1);

  m_nPendingChangeState--;
}

void
SigfoxRadioEnergyModel::HandleEnergyDepletion (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("SigfoxRadioEnergyModel:Energy is depleted!");
  // invoke energy depletion callback, if set.
  if (!m_energyDepletionCallback.IsNull ())
    {
      m_energyDepletionCallback ();
    }
}

void
SigfoxRadioEnergyModel::HandleEnergyChanged (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("SigfoxRadioEnergyModel:Energy changed!");
}

void
SigfoxRadioEnergyModel::HandleEnergyRecharged (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("SigfoxRadioEnergyModel:Energy is recharged!");
  // invoke energy recharged callback, if set.
  if (!m_energyRechargedCallback.IsNull ())
    {
      m_energyRechargedCallback ();
    }
}

SigfoxRadioEnergyModelPhyListener *
SigfoxRadioEnergyModel::GetPhyListener (void)
{
  NS_LOG_FUNCTION (this);
  return m_listener;
}

/*
 * Private functions start here.
 */

void
SigfoxRadioEnergyModel::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_source = NULL;
  m_energyDepletionCallback.Nullify ();
}

double
SigfoxRadioEnergyModel::DoGetCurrentA (void) const
{
  NS_LOG_FUNCTION (this);
  switch (m_currentState)
    {
    case EndPointSigfoxPhy::STANDBY:
      return m_idleCurrentA;
    case EndPointSigfoxPhy::TX:
      return m_txCurrentA;
    case EndPointSigfoxPhy::RX:
      return m_rxCurrentA;
    case EndPointSigfoxPhy::SLEEP:
      return m_sleepCurrentA;
    default:
      NS_FATAL_ERROR ("SigfoxRadioEnergyModel:Undefined radio state:" << m_currentState);
    }
}

void
SigfoxRadioEnergyModel::SetSigfoxRadioState (const EndPointSigfoxPhy::State state)
{
  NS_LOG_FUNCTION (this << state);
  m_currentState = state;
  std::string stateName;
  switch (state)
    {
    case EndPointSigfoxPhy::STANDBY:
      stateName = "STANDBY";
      break;
    case EndPointSigfoxPhy::TX:
      stateName = "TX";
      break;
    case EndPointSigfoxPhy::RX:
      stateName = "RX";
      break;
    case EndPointSigfoxPhy::SLEEP:
      stateName = "SLEEP";
      break;
    }
  NS_LOG_DEBUG ("SigfoxRadioEnergyModel:Switching to state: "
                << stateName << " at time = " << Simulator::Now ().GetSeconds () << " s");
}

// -------------------------------------------------------------------------- //

SigfoxRadioEnergyModelPhyListener::SigfoxRadioEnergyModelPhyListener ()
{
  NS_LOG_FUNCTION (this);
  m_changeStateCallback.Nullify ();
  m_updateTxCurrentCallback.Nullify ();
}

SigfoxRadioEnergyModelPhyListener::~SigfoxRadioEnergyModelPhyListener ()
{
  NS_LOG_FUNCTION (this);
}

void
SigfoxRadioEnergyModelPhyListener::SetChangeStateCallback (
    DeviceEnergyModel::ChangeStateCallback callback)
{
  NS_LOG_FUNCTION (this << &callback);
  NS_ASSERT (!callback.IsNull ());
  m_changeStateCallback = callback;
}

void
SigfoxRadioEnergyModelPhyListener::SetUpdateTxCurrentCallback (UpdateTxCurrentCallback callback)
{
  NS_LOG_FUNCTION (this << &callback);
  NS_ASSERT (!callback.IsNull ());
  m_updateTxCurrentCallback = callback;
}

void
SigfoxRadioEnergyModelPhyListener::NotifyRxStart ()
{
  NS_LOG_FUNCTION (this);
  if (m_changeStateCallback.IsNull ())
    {
      NS_FATAL_ERROR ("SigfoxRadioEnergyModelPhyListener:Change state callback not set!");
    }
  m_changeStateCallback (EndPointSigfoxPhy::RX);
}

void
SigfoxRadioEnergyModelPhyListener::NotifyTxStart (double txPowerDbm)
{
  NS_LOG_FUNCTION (this << txPowerDbm);
  if (m_updateTxCurrentCallback.IsNull ())
    {
      NS_FATAL_ERROR ("SigfoxRadioEnergyModelPhyListener:Update tx current callback not set!");
    }
  m_updateTxCurrentCallback (txPowerDbm);
  if (m_changeStateCallback.IsNull ())
    {
      NS_FATAL_ERROR ("SigfoxRadioEnergyModelPhyListener:Change state callback not set!");
    }
  m_changeStateCallback (EndPointSigfoxPhy::TX);
}

void
SigfoxRadioEnergyModelPhyListener::NotifySleep (void)
{
  NS_LOG_FUNCTION (this);
  if (m_changeStateCallback.IsNull ())
    {
      NS_FATAL_ERROR ("SigfoxRadioEnergyModelPhyListener:Change state callback not set!");
    }
  m_changeStateCallback (EndPointSigfoxPhy::SLEEP);
}

void
SigfoxRadioEnergyModelPhyListener::NotifyStandby (void)
{
  NS_LOG_FUNCTION (this);
  if (m_changeStateCallback.IsNull ())
    {
      NS_FATAL_ERROR ("SigfoxRadioEnergyModelPhyListener:Change state callback not set!");
    }
  m_changeStateCallback (EndPointSigfoxPhy::STANDBY);
}

/*
 * Private function state here.
 */

void
SigfoxRadioEnergyModelPhyListener::SwitchToStandby (void)
{
  NS_LOG_FUNCTION (this);
  if (m_changeStateCallback.IsNull ())
    {
      NS_FATAL_ERROR ("SigfoxRadioEnergyModelPhyListener:Change state callback not set!");
    }
  m_changeStateCallback (EndPointSigfoxPhy::STANDBY);
}

} // namespace sigfox
} // namespace ns3

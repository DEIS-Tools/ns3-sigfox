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
 * Author: Romagnolo Stefano <romagnolostefano93@gmail.com>
 */

#include "sigfox-tx-current-model.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "sigfox-utils.h"

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("SigfoxTxCurrentModel");

NS_OBJECT_ENSURE_REGISTERED (SigfoxTxCurrentModel);

TypeId
SigfoxTxCurrentModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SigfoxTxCurrentModel")
    .SetParent<Object> ()
    .SetGroupName ("Sigfox")
  ;
  return tid;
}

SigfoxTxCurrentModel::SigfoxTxCurrentModel ()
{
}

SigfoxTxCurrentModel::~SigfoxTxCurrentModel ()
{
}

// Similarly to the wifi case
NS_OBJECT_ENSURE_REGISTERED (LinearSigfoxTxCurrentModel);

TypeId
LinearSigfoxTxCurrentModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LinearSigfoxTxCurrentModel")
    .SetParent<SigfoxTxCurrentModel> ()
    .SetGroupName ("Sigfox")
    .AddConstructor<LinearSigfoxTxCurrentModel> ()
    .AddAttribute ("Eta", "The efficiency of the power amplifier.",
                   DoubleValue (0.10),
                   MakeDoubleAccessor (&LinearSigfoxTxCurrentModel::SetEta,
                                       &LinearSigfoxTxCurrentModel::GetEta),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Voltage", "The supply voltage (in Volts).",
                   DoubleValue (3.3),
                   MakeDoubleAccessor (&LinearSigfoxTxCurrentModel::SetVoltage,
                                       &LinearSigfoxTxCurrentModel::GetVoltage),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("StandbyCurrent", "The current in the STANDBY state (in Watts).",
                   DoubleValue (0.0014),      // idle mode = 1.4mA
                   MakeDoubleAccessor (&LinearSigfoxTxCurrentModel::SetStandbyCurrent,
                                       &LinearSigfoxTxCurrentModel::GetStandbyCurrent),
                   MakeDoubleChecker<double> ())
  ;
  return tid;
}

LinearSigfoxTxCurrentModel::LinearSigfoxTxCurrentModel ()
{
  NS_LOG_FUNCTION (this);
}

LinearSigfoxTxCurrentModel::~LinearSigfoxTxCurrentModel ()
{
  NS_LOG_FUNCTION (this);
}

void
LinearSigfoxTxCurrentModel::SetEta (double eta)
{
  NS_LOG_FUNCTION (this << eta);
  m_eta = eta;
}

void
LinearSigfoxTxCurrentModel::SetVoltage (double voltage)
{
  NS_LOG_FUNCTION (this << voltage);
  m_voltage = voltage;
}

void
LinearSigfoxTxCurrentModel::SetStandbyCurrent (double idleCurrent)
{
  NS_LOG_FUNCTION (this << idleCurrent);
  m_idleCurrent = idleCurrent;
}

double
LinearSigfoxTxCurrentModel::GetEta (void) const
{
  return m_eta;
}

double
LinearSigfoxTxCurrentModel::GetVoltage (void) const
{
  return m_voltage;
}

double
LinearSigfoxTxCurrentModel::GetStandbyCurrent (void) const
{
  return m_idleCurrent;
}

double
LinearSigfoxTxCurrentModel::CalcTxCurrent (double txPowerDbm) const
{
  NS_LOG_FUNCTION (this << txPowerDbm);
  return DbmToW (txPowerDbm) / (m_voltage * m_eta) + m_idleCurrent;
}


NS_OBJECT_ENSURE_REGISTERED (ConstantSigfoxTxCurrentModel);

TypeId
ConstantSigfoxTxCurrentModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ConstantSigfoxTxCurrentModel")
    .SetParent<SigfoxTxCurrentModel> ()
    .SetGroupName ("Sigfox")
    .AddConstructor<ConstantSigfoxTxCurrentModel> ()
    .AddAttribute ("TxCurrent",
                   "The radio Tx current in Ampere.",
                   DoubleValue (0.028),        // transmit at 0dBm = 28mA
                   MakeDoubleAccessor (&ConstantSigfoxTxCurrentModel::SetTxCurrent,
                                       &ConstantSigfoxTxCurrentModel::GetTxCurrent),
                   MakeDoubleChecker<double> ())
  ;
  return tid;
}

ConstantSigfoxTxCurrentModel::ConstantSigfoxTxCurrentModel ()
{
  NS_LOG_FUNCTION (this);
}

ConstantSigfoxTxCurrentModel::~ConstantSigfoxTxCurrentModel ()
{
  NS_LOG_FUNCTION (this);
}

void
ConstantSigfoxTxCurrentModel::SetTxCurrent (double txCurrent)
{
  NS_LOG_FUNCTION (this << txCurrent);
  m_txCurrent = txCurrent;
}

double
ConstantSigfoxTxCurrentModel::GetTxCurrent (void) const
{
  return m_txCurrent;
}

double
ConstantSigfoxTxCurrentModel::CalcTxCurrent (double txPowerDbm) const
{
  NS_LOG_FUNCTION (this << txPowerDbm);
  return m_txCurrent;
}

}
} // namespace ns3

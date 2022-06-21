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

#include "ns3/sigfox-mac-header.h"
#include "ns3/log.h"
#include <bitset>

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("SigfoxMacHeader");

SigfoxMacHeader::SigfoxMacHeader ()
{
}

SigfoxMacHeader::~SigfoxMacHeader ()
{
}

TypeId
SigfoxMacHeader::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("SigfoxMacHeader").SetParent<Header> ().AddConstructor<SigfoxMacHeader> ();
  return tid;
}

TypeId
SigfoxMacHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
SigfoxMacHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return 1; // This header only consists in 8 bits
}

void
SigfoxMacHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION_NOARGS ();
  // TODO

  // The header we need to fill
  uint8_t header = 0;

  // The MType
  header |= m_mtype << 5;

  // Write the byte
  start.WriteU8 (header);

  NS_LOG_DEBUG ("Serialization of MAC header: " << std::bitset<8> (header));
}

uint32_t
SigfoxMacHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Save the byte on a temporary variable
  uint8_t byte;
  byte = start.ReadU8 ();

  // Move the three most significant bits to the least significant positions
  // to get the MType
  m_mtype = byte >> 5;

  return 1; // the number of bytes consumed.
}

void
SigfoxMacHeader::Print (std::ostream &os) const
{
  os << "MessageType=" << unsigned (m_mtype) << std::endl;
}

void
SigfoxMacHeader::SetMType (enum MType mtype)
{
  NS_LOG_FUNCTION (this << mtype);

  m_mtype = mtype;
}

uint8_t
SigfoxMacHeader::GetMType (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_mtype;
}

bool
SigfoxMacHeader::IsUplink (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return (m_mtype == UNCONFIRMED_DATA_UP) || (m_mtype == CONFIRMED_DATA_UP);
}

bool
SigfoxMacHeader::IsConfirmed (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return (m_mtype == CONFIRMED_DATA_DOWN) || (m_mtype == CONFIRMED_DATA_UP);
}
} // namespace sigfox
} // namespace ns3

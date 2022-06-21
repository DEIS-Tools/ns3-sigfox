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

#include "ns3/sigfox-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {
namespace sigfox {

NS_OBJECT_ENSURE_REGISTERED (SigfoxTag);

TypeId
SigfoxTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SigfoxTag")
    .SetParent<Tag> ()
    .SetGroupName ("sigfox")
    .AddConstructor<SigfoxTag> ()
  ;
  return tid;
}

TypeId
SigfoxTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

SigfoxTag::SigfoxTag () :
  m_receivePower (0),
  m_frequency (0),
  m_durationSeconds (0),
  m_repNumber (0),
  m_packetNumber (0),
  m_senderId (0)
{
}

SigfoxTag::~SigfoxTag ()
{
}

uint32_t
SigfoxTag::GetSerializedSize (void) const
{
  // Each datum about a SF is 1 byte + receivePower (the size of a double) +
  // frequency (the size of a double)
  return 2 * 8 + 1 * 32 + 3 * sizeof(double);
}

void
SigfoxTag::Serialize (TagBuffer i) const
{
  i.WriteDouble (m_receivePower);
  i.WriteDouble (m_frequency);
  i.WriteDouble (m_durationSeconds);
  i.WriteU8(m_repNumber);
  i.WriteU8(m_packetNumber);
  i.WriteU32(m_senderId);
}

void
SigfoxTag::Deserialize (TagBuffer i)
{
  m_receivePower = i.ReadDouble ();
  m_frequency = i.ReadDouble ();
  m_durationSeconds = i.ReadDouble ();
  m_repNumber = i.ReadU8();
  m_packetNumber = i.ReadU8();
  m_senderId = i.ReadU32();
}

void
SigfoxTag::Print (std::ostream &os) const
{
  os << m_receivePower << " " << m_frequency << " " << m_durationSeconds;
}

double
SigfoxTag::GetReceivePower () const
{
  return m_receivePower;
}

void
SigfoxTag::SetReceivePower (double receivePower)
{
  m_receivePower = receivePower;
}

void
SigfoxTag::SetFrequency (double frequency)
{
  m_frequency = frequency;
}

double
SigfoxTag::GetFrequency (void)
{
  return m_frequency;
}


void
SigfoxTag::SetRepetitionNumber (uint8_t rep)
{
  m_repNumber = rep;
}

uint8_t
SigfoxTag::GetRepetitionNumber (void)
{
  return m_repNumber;
}

void
SigfoxTag::SetPacketNumber (uint8_t pn)
{
  m_packetNumber = pn;
}

uint8_t
SigfoxTag::GetPacketNumber (void)
{
  return m_packetNumber;
}

void
SigfoxTag::SetSenderId (uint32_t senderId)
{
  m_senderId = senderId;
}

uint32_t
SigfoxTag::GetSenderId (void)
{
  return m_senderId;
}

double
SigfoxTag::GetDurationSeconds (void)
{
  return m_durationSeconds;
}

void
SigfoxTag::SetDurationSeconds (double durationSeconds)
{
  m_durationSeconds = durationSeconds;
}

}
} // namespace ns3

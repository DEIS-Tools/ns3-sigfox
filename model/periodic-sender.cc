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

#include "ns3/periodic-sender.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/sigfox-net-device.h"
#include "ns3/random-variable-stream.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/uinteger.h"

namespace ns3 {
namespace sigfox {

NS_LOG_COMPONENT_DEFINE ("PeriodicSender");

NS_OBJECT_ENSURE_REGISTERED (PeriodicSender);

TypeId
PeriodicSender::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PeriodicSender")
    .SetParent<Application> ()
    .AddConstructor<PeriodicSender> ()
    .SetGroupName ("sigfox")
    .AddAttribute ("Interval", "The interval between packet sends of this app",
                   TimeValue (Seconds (0)),
                   MakeTimeAccessor (&PeriodicSender::GetInterval,
                                     &PeriodicSender::SetInterval),
                   MakeTimeChecker ());
  // .AddAttribute ("PacketSizeRandomVariable", "The random variable that determines the shape of the packet size, in bytes",
  //                StringValue ("ns3::UniformRandomVariable[Min=0,Max=10]"),
  //                MakePointerAccessor (&PeriodicSender::m_pktSizeRV),
  //                MakePointerChecker <RandomVariableStream>());
  return tid;
}

PeriodicSender::PeriodicSender ()
  : m_interval (Seconds (10)),
  m_initialDelay (Seconds (1)),
  m_basePktSize (10),
  m_pktSizeRV (0)

{
  NS_LOG_FUNCTION_NOARGS ();
}

PeriodicSender::~PeriodicSender ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
PeriodicSender::SetInterval (Time interval)
{
  NS_LOG_FUNCTION (this << interval);
  m_interval = interval;
}

Time
PeriodicSender::GetInterval (void) const
{
  NS_LOG_FUNCTION (this);
  return m_interval;
}

void
PeriodicSender::SetInitialDelay (Time delay)
{
  NS_LOG_FUNCTION (this << delay);
  m_initialDelay = delay;
}


void
PeriodicSender::SetPacketSizeRandomVariable (Ptr <RandomVariableStream> rv)
{
  m_pktSizeRV = rv;
}


void
PeriodicSender::SetPacketSize (uint8_t size)
{
  m_basePktSize = size;
}

// ***********for Data compression ******************
void PeriodicSender::EnableSendingData(void)
{
    new_msr= data[i];
    i += 1;
    NS_LOG_DEBUG ("Measured value is============== "<<new_msr );
    if(i==10789)
    {
        i =0;
    }
    if(abs(old_msr - new_msr) >30)
    {
        sending = true;
        old_msr = new_msr;
    }
    
    Simulator::Schedule (Seconds (600.0), &PeriodicSender::EnableSendingData, this);
}
// ***********  for Weather Driven Strategy    ******************
void PeriodicSender::EnableSendingWeather(void)
{
    ns3::RngSeedManager::SetSeed (3);  // Changes seed from default of 1 to 3
    ns3::RngSeedManager::SetRun (5);   // Changes run number from default of 1 to 7
    
    // ************   For Rainy model  *************************
    // ************   using rand command  *************************
    //double random_value = (double)rand()/RAND_MAX;
    
    // ************   UniformRandomVariable  *************************
    //Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
    
    // ************   ExponentialRandomVariable  *************************
    //Ptr<ExponentialRandomVariable> y = CreateObject<ExponentialRandomVarlable> ();
    
    // ************   ExponentialRandomVariable  *************************
    double mean = 0.5;
    double bound = 1.0;
     
    Ptr<ExponentialRandomVariable> x = CreateObject<ExponentialRandomVariable> ();
    x->SetAttribute ("Mean", DoubleValue (mean));
    x->SetAttribute ("Bound", DoubleValue (bound));
    
    double random_value = x->GetValue();
    NS_LOG_DEBUG ("============== " <<random_value);
    
    if (random_value < 0.48)
    {
        m_interval= Seconds (600);
        NS_LOG_DEBUG ("It is rainy============== " <<m_interval<<"============== " <<random_value);
    }else{
        m_interval= Seconds (1800);
        NS_LOG_DEBUG ("It is Not rainy============== "<<m_interval );
    }
    // ********************************************************
}

void
PeriodicSender::SendPacket (void)
{
    if(EnableWeatherModel == true)
    {
        EnableSendingWeather();
    }
    NS_LOG_FUNCTION (this);
    
  // Create and send a new packet
  Ptr<Packet> packet;
  if (m_pktSizeRV)
    {
      packet = Create<Packet> (m_basePktSize);
    }
  else
    {
      packet = Create<Packet> (m_basePktSize);
    }
    if(EnableDataModel==true)
    {
        if(sending==false)
        {
            m_mac->Send (packet);
            sending = false;
        }
    }
    else
    {
        m_mac->Send (packet);
        m_mac->BDPFrequency(UpdateBDPF);
    }
        
        

  // Schedule the next SendPacket event
  m_sendEvent = Simulator::Schedule (m_interval, &PeriodicSender::SendPacket,
                                     this);

  //NS_LOG_DEBUG ("Sent a packet of size " << packet->GetSize ());
  //    int x= 0; x = Sendfrequency ();
  sendcnt += 1;
  //NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "Node Sends a packet");
  //NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "Number of sent packet " <<x );
}
int
PeriodicSender::Sendfrequency (void)
{
  return sendcnt;
}

// SelectTransmissionStrategy
void PeriodicSender::SelectTransmissionStrategy (uint8_t  SelectStrategy)
{
    if(SelectStrategy==1)
    {
        m_interval= Seconds (600);
    }
    if(SelectStrategy==2)
    {
        m_interval= Seconds (600);
    }
    if(SelectStrategy==3)
    {
        EnableWeatherModel = true;
    }
    if(SelectStrategy==4)
    {
        EnableDataModel = true;
        EnableSendingData();
    }
}

void PeriodicSender::SelectBiDirectionalProcedureFrequency(uint8_t BDPF)
{
    UpdateBDPF = BDPF;
}

void
PeriodicSender::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  // Make sure we have a MAC layer
  if (m_mac == 0)
    {
      // Assumes there's only one device
      Ptr<SigfoxNetDevice> sigfoxNetDevice = m_node->GetDevice (0)->GetObject<SigfoxNetDevice> ();

      m_mac = sigfoxNetDevice->GetMac ();
      NS_ASSERT (m_mac != 0);
    }

  // Schedule the next SendPacket event
  Simulator::Cancel (m_sendEvent);
  NS_LOG_DEBUG ("Starting up application with a first event with a " <<
                m_initialDelay.GetSeconds () << " seconds delay");

  m_sendEvent = Simulator::Schedule (m_initialDelay,
                                     &PeriodicSender::SendPacket, this);
  NS_LOG_DEBUG ("Event Id: " << m_sendEvent.GetUid ());
}

void
PeriodicSender::StopApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Simulator::Cancel (m_sendEvent);
}

}
}

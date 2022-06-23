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
 * Author: Muhammad Naeem <mnaeem@cs.aau.dk>
 *
 * Modified by: Michele Albano <mialb@cs.aau.dk>
 */


#include "ns3/end-point-sigfox-phy.h"
#include "ns3/gateway-sigfox-phy.h"
#include "ns3/gateway-sigfox-mac.h"
#include "ns3/sigfox-tag.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/sigfox-helper.h"
#include "ns3/node-container.h"
#include "ns3/mobility-helper.h"
#include "ns3/position-allocator.h"
#include "ns3/double.h"
#include "ns3/random-variable-stream.h"
#include "ns3/periodic-sender-helper.h"
#include "ns3/command-line.h"
#include "ns3/forwarder-helper.h"
#include "ns3/energy-module.h"
#include "ns3/sigfox-net-device.h"
#include "ns3/sigfox-radio-energy-model-helper.h"
#include "ns3/config.h"
#include "ns3/names.h"
#include <algorithm>
#include <ctime>
#include "ns3/periodic-sender.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>

using namespace ns3;
using namespace sigfox;

NS_LOG_COMPONENT_DEFINE ("EnergyModelExample");
int nDevices = 1;
int nGateways = 1;

const int day = 86400;
// const int TotalTime = 2000; //4*day;
const int TotalTime = 62; //4*day;
double simulationTime = TotalTime;
uint32_t receivedpackets = 0;
uint32_t sendpackets = 0;
uint32_t numberofmeasurments = 0;

int appPeriodSeconds = TotalTime;
double battery = 100 * 60 * 60;
double EnergyConsumptionMeasurment = 0;
double EnergyConsumptionNode = 0;
double TotalRemainingEnergy = 0;


void
Print (void)
{
  if (TotalRemainingEnergy >= 0)
    TotalRemainingEnergy = battery - EnergyConsumptionNode - EnergyConsumptionMeasurment;
  if (TotalRemainingEnergy <= 0)
    TotalRemainingEnergy = 0;
  // NS_LOG_UNCOND ( TotalRemainingEnergy << "   " <<EnergyConsumptionNode <<  "   " << EnergyConsumptionMeasurment);
  std::ofstream out ("BatteryLevel.txt", std::ios::app);
  out << (Simulator::Now ()).GetSeconds () << " , " << TotalRemainingEnergy << std::endl;
  out.close ();
  Simulator::Schedule (Seconds (60.0), &Print);
}

void
syscurrent (double y, double x)
{
  std::ofstream out ("CurrentGraph.txt", std::ios::app);
  out << (Simulator::Now ()).GetSeconds () - 0.001 << " " << y << std::endl;
  //out.close ();

  //std::ofstream out ("CurrentGraph.txt", std::ios::app);
  out << (Simulator::Now ()).GetSeconds () << " " << x << std::endl;
  out.close ();
}

void
RemainingEnergy (double oldValue, double remainingEnergy)
{
  /*NS_LOG_UNCOND (Simulator::Now ().GetSeconds ()
                 << "s Current remaining energy = " << remainingEnergy << "J");*/
}

void
TotalEnergy (double oldValue, double totalEnergy)
{
  //NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "s Total energy consumed by radio = " << totalEnergy << "J");
  EnergyConsumptionNode = totalEnergy;
}

void
Measure ()
{
  //NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "Node 0 Measures a Value");
  //NS_LOG_UNCOND ("Old value"<<EnergyConsumptionMeasurment<<"Simulation time"<<Simulator::Now ().GetSeconds () );
  EnergyConsumptionMeasurment += 32.2;
  //numberofmeasurments += 1;
  Simulator::Schedule (Seconds (60.0), &Measure);
  //NS_LOG_UNCOND ("new value"<<EnergyConsumptionMeasurment<<"Simulation time"<<Simulator::Now ().GetSeconds () );
}

void
SelfDischarge ()
{
  NS_LOG_UNCOND ("Old value" << TotalRemainingEnergy << "Simulation time"
                             << Simulator::Now ().GetSeconds ());
  TotalRemainingEnergy =
      TotalRemainingEnergy -
      0.02 * (TotalRemainingEnergy / (30 * day)) *
          day; //EnergyConsumptionSelfDischarge += 5.4;     // battery[id]=battery[id]−sdc_rate[id]∗(battery[id]/sdc_time[id])∗1∗day
  //numberofmeasurments += 1;
  //NS_LOG_UNCOND ("new value"<<TotalRemainingEnergy<<"Simulation tvoidime"<<Simulator::Now ().GetSeconds () );
  Simulator::Schedule (Seconds (86400.0), &SelfDischarge);
}

void
PacketSentOnChannel (Ptr<const Packet> packet)
{
  NS_LOG_FUNCTION (packet);
  SigfoxTag tag;
  packet->PeekPacketTag (tag);

  NS_LOG_DEBUG ("Packet was sent at frequency " << tag.GetFrequency () << " with a duration of "
                                                << tag.GetDurationSeconds () << " s.");

  NS_LOG_DEBUG ("This was APP packet number " << unsigned(tag.GetPacketNumber ()) << ", repetition number "
                << unsigned(tag.GetRepetitionNumber ()) << ", from device "
                << unsigned(tag.GetSenderId()));

  std::ofstream out ("Packets.txt", std::ios::app);
  out << std::fixed;
  out << (Simulator::Now ()).GetSeconds () << " " << tag.GetFrequency () << " "
      << tag.GetDurationSeconds () << std::endl;
  out.close ();
}

std::vector<std::vector<std::vector<bool>>> packetOutcomes;

void
SetPacketOutcome (uint32_t senderId, uint8_t appPacket, uint8_t repetitionNumber, bool outcome)
{
  // Only count the packets if they are sent outside the transients
  NS_LOG_DEBUG ("packetOutcomes size: " << packetOutcomes.size ());
  std::vector<std::vector<bool>> &senderOutcomes = packetOutcomes.at (senderId);
  NS_LOG_DEBUG ("senderOutcomes size: " << senderOutcomes.size ());
  if (appPacket >= senderOutcomes.size ())
    {
      if (Simulator::Now () > Seconds (10) && Simulator::Now () < Seconds (62 - 10))
        {
          NS_LOG_DEBUG ("Adding new outcome");
          senderOutcomes.push_back (std::vector<bool> ({outcome}));
        }
    }
  else
    {
      NS_LOG_DEBUG ("Appending to existing outcome");
      senderOutcomes.at (appPacket).push_back (outcome);
    }
}

std::pair<int, int>
CountSuccessesAndFailures ()
{
  int failures = 0;
  int successes = 0;
  for (auto &senderOutcomes : packetOutcomes)
    {
      for (auto &outcomes : senderOutcomes)
        {
          if (std::none_of (outcomes.begin (), outcomes.end (), [] (bool v) {return v;}))
            {
              failures++;
            }
          else
            {
              successes++;
            }
        }
    }
  return std::pair<int, int> (successes, failures);
}

void
ReceivedPacketAtGateway (Ptr<const Packet> packet, uint32_t id)
{
  NS_LOG_FUNCTION (packet);

  SigfoxTag tag;
  packet->PeekPacketTag (tag);

  NS_LOG_DEBUG ("This was APP packet number " << unsigned(tag.GetPacketNumber ()) << ", repetition number "
                << unsigned(tag.GetRepetitionNumber ()) << ", from device "
                << unsigned(tag.GetSenderId()));

  SetPacketOutcome (tag.GetSenderId(), tag.GetPacketNumber(), tag.GetRepetitionNumber(), true);
}

void
LostPacketAtGateway (Ptr<const Packet> packet, uint32_t id)
{
  NS_LOG_FUNCTION (packet);

  SigfoxTag tag;
  packet->PeekPacketTag (tag);

  NS_LOG_DEBUG ("This was APP packet number " << unsigned(tag.GetPacketNumber ()) << ", repetition number "
                << unsigned(tag.GetRepetitionNumber ()) << ", from device "
                << unsigned(tag.GetSenderId()));

  SetPacketOutcome (tag.GetSenderId(), tag.GetPacketNumber(), tag.GetRepetitionNumber(), false);
}

int
main (int argc, char *argv[])
{
  uint32_t nRepetitions = 1;
  // LogComponentEnable ("EnergyModelExample", LOG_LEVEL_ALL);
  // LogComponentEnable ("SigfoxChannel", LOG_LEVEL_INFO);
  // LogComponentEnable ("SigfoxPhy", LOG_LEVEL_ALL);
  // LogComponentEnable ("EndPointSigfoxPhy", LOG_LEVEL_ALL);
  // LogComponentEnable ("GatewaySigfoxPhy", LOG_LEVEL_ALL);
  // LogComponentEnable ("SigfoxInterferenceHelper", LOG_LEVEL_ALL);
  // LogComponentEnable ("SigfoxMac", LOG_LEVEL_ALL);
  // LogComponentEnable ("EndPointSigfoxMac", LOG_LEVEL_ALL);
  // LogComponentEnable ("GatewaySigfoxMac", LOG_LEVEL_ALL);
  // LogComponentEnable ("LogicalSigfoxChannelHelper", LOG_LEVEL_ALL);
  // LogComponentEnable ("SigfoxHelper", LOG_LEVEL_ALL);
  // LogComponentEnable ("SigfoxPhyHelper", LOG_LEVEL_ALL);
  // LogComponentEnable ("SigfoxMacHelper", LOG_LEVEL_ALL);
  // LogComponentEnable ("PeriodicSenderHelper", LOG_LEVEL_ALL);
  // LogComponentEnable ("PeriodicSender", LOG_LEVEL_ALL);
  // LogComponentEnable ("SigfoxMacHeader", LOG_LEVEL_ALL);
  // LogComponentEnable ("SimpleEndPointSigfoxPhy", LOG_LEVEL_ALL);
  // LogComponentEnable ("SigfoxRadioEnergyModel", LOG_LEVEL_ALL);
  // LogComponentEnable ("BasicEnergySource", LOG_LEVEL_ALL);
  // LogComponentEnable ("Forwarder", LOG_LEVEL_ALL);
  LogComponentEnableAll (LOG_PREFIX_FUNC);
  LogComponentEnableAll (LOG_PREFIX_NODE);
  LogComponentEnableAll (LOG_PREFIX_TIME);

  CommandLine cmd;
  cmd.AddValue ("nDevices", "Number of Sigfox End Points to simulate", nDevices);
  cmd.AddValue ("nRepetitions", "Number of repetitions", nRepetitions);
  cmd.Parse (argc, argv);

  Config::SetDefault("ns3::EndPointSigfoxMac::Repetitions", IntegerValue(nRepetitions));

  /************************
  *  Create the channel  *
  ************************/

  NS_LOG_INFO ("Creating the channel...");

  // Create the sigfox channel object
  Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel> ();
  loss->SetPathLossExponent (3.76);
  loss->SetReference (1, 7.7);

  Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel> ();

  Ptr<SigfoxChannel> channel = CreateObject<SigfoxChannel> (loss, delay);

  // channel->TraceConnectWithoutContext ("PacketSent", MakeCallback (&PacketSentOnChannel));

  /************************
  *  Create the helpers  *
  ************************/

  NS_LOG_INFO ("Setting up helpers...");

  MobilityHelper mobility;
  Ptr<ListPositionAllocator> allocator = CreateObject<ListPositionAllocator> ();
  allocator->Add (Vector (0, 0, 0));
  allocator->Add (Vector (0, 0, 0));
  mobility.SetPositionAllocator (allocator);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  // Create the SigfoxPhyHelper
  SigfoxPhyHelper phyHelper = SigfoxPhyHelper ();
  phyHelper.SetChannel (channel);

  // Create the SigfoxMacHelper
  SigfoxMacHelper macHelper = SigfoxMacHelper ();

  // Create the SigfoxHelper
  SigfoxHelper helper = SigfoxHelper ();

  //Create the ForwarderHelper
  ForwarderHelper forHelper = ForwarderHelper ();
  /************************
  *  Create End Devices  *
  ************************/

  NS_LOG_INFO ("Creating the end device...");

  // Create a set of nodes
  NodeContainer endDevices;
  endDevices.Create (nDevices);

  // Assign a mobility model to the node
  mobility.Install (endDevices);

  // Create the SigfoxNetDevices of the end devices
  phyHelper.SetDeviceType (SigfoxPhyHelper::EP);
  macHelper.SetDeviceType (SigfoxMacHelper::EP);
  NetDeviceContainer endDevicesNetDevices = helper.Install (phyHelper, macHelper, endDevices);

  // Initialize the data structure to keep track of packet successes and failures
  packetOutcomes.resize(nDevices);

  /*********************
   *  Create Gateways  *
   *********************/

  NS_LOG_INFO ("Creating the gateway...");
  NodeContainer gateways;
  gateways.Create (nGateways);

  mobility.SetPositionAllocator (allocator);
  mobility.Install (gateways);

  // Create a netdevice for each gateway
  phyHelper.SetDeviceType (SigfoxPhyHelper::GW);
  macHelper.SetDeviceType (SigfoxMacHelper::GW);
  helper.Install (phyHelper, macHelper, gateways);

  // Connect trace sources to keep track of packets
  gateways.Get (0)->GetDevice (0)->GetObject<SigfoxNetDevice> ()->GetPhy ()->TraceConnectWithoutContext("ReceivedPacket", MakeCallback(&ReceivedPacketAtGateway));
  gateways.Get (0)->GetDevice (0)->GetObject<SigfoxNetDevice> ()->GetPhy ()->TraceConnectWithoutContext("LostPacketBecauseInterference", MakeCallback(&LostPacketAtGateway));

  /************************
   * Install Energy Model *
   ************************/

  BasicEnergySourceHelper basicSourceHelper;
  SigfoxRadioEnergyModelHelper radioEnergyHelper;

  // configure energy source
  basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (3600)); // Energy in J
  basicSourceHelper.Set ("BasicEnergySupplyVoltageV", DoubleValue (3.3));

  radioEnergyHelper.Set ("StandbyCurrentA", DoubleValue (0.0043));
  //radioEnergyHelper.Set ("TxCurrentA", DoubleValue (28000));
  radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0.047));
  radioEnergyHelper.Set ("SleepCurrentA", DoubleValue (0.02784));
  radioEnergyHelper.Set ("RxCurrentA", DoubleValue (0.019));

  radioEnergyHelper.SetTxCurrentModel ("ns3::ConstantSigfoxTxCurrentModel", "TxCurrent",
                                       DoubleValue (0.050));

  // install source on EDs' nodes
  EnergySourceContainer sources = basicSourceHelper.Install (endDevices);
  Names::Add ("/Names/EnergySource", sources.Get (0));

  // install device model
  DeviceEnergyModelContainer deviceModels =
      radioEnergyHelper.Install (endDevicesNetDevices, sources);

  /*********************************************************************************/

  Time appStopTime = Seconds (simulationTime);
  PeriodicSenderHelper appHelper = PeriodicSenderHelper ();
  appHelper.SetPeriod (Seconds (60)); //appPeriodSeconds));
  appHelper.SetPacketSize (12);
  //Ptr<RandomVariableStream> rv = CreateObjectWithAttributes<UniformRandomVariable> ( "Min", DoubleValue (10), "Max", DoubleValue (10));
  ApplicationContainer appContainer = appHelper.Install (endDevices);

  appContainer.Start (Seconds (0));
  appContainer.Stop (appStopTime);

  /**************
     * Get output *
     **************/

  Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (sources.Get (0));
  //Simulator::Schedule (Seconds (60.0), &GetRemainingEnergy);
  basicSourcePtr->TraceConnectWithoutContext ("RemainingEnergy", MakeCallback (&RemainingEnergy));

  Ptr<DeviceEnergyModel> basicRadioModelPtr =
      basicSourcePtr->FindDeviceEnergyModels ("ns3::SigfoxRadioEnergyModel").Get (0);
  NS_ASSERT (basicRadioModelPtr != NULL);
  basicRadioModelPtr->TraceConnectWithoutContext ("TotalEnergyConsumption",
                                                  MakeCallback (&TotalEnergy));

  basicRadioModelPtr->TraceConnectWithoutContext ("SystemCurrent", MakeCallback (&syscurrent));

  /**************
   * Get output File *
   **************/
  std::string CSVfileName ("BatteryLevel.txt", "CurrentGraph.txt");
  std::ofstream out (CSVfileName.c_str ());
  out.close ();

  /****************
  *  Simulation  *
  ****************/

  Simulator::Stop (Seconds (simulationTime));

  NS_LOG_INFO ("Running simulation...");
  Print ();

  Simulator::Schedule (Seconds (60.0), &Measure);
  Simulator::Schedule (Seconds (86400.0), &SelfDischarge);
  Simulator::Run ();

  Simulator::Destroy ();

  std::pair<int, int> successesAndFailures = CountSuccessesAndFailures();
  std::cout << successesAndFailures.first << " " << successesAndFailures.second;

  return 0;
}

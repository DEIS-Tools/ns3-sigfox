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

#include "ns3/names.h"
#include "ns3/end-point-sigfox-phy.h"
#include "ns3/gateway-sigfox-phy.h"
#include "ns3/end-point-sigfox-mac.h"
#include "ns3/gateway-sigfox-mac.h"
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
#include "ns3/sdc-energy-source-helper.h"
#include "ns3/sdc-energy-source.h"
#include <algorithm>
#include <ctime>
#include "ns3/periodic-sender.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>

using namespace ns3;
using namespace sigfox;

NS_LOG_COMPONENT_DEFINE ("SigfoxEnergyModelExample");

int nDevices = 1;
int nGateways = 1;

const int day = 86400;
const int TotalTime = 600*day;
double simulationTime = TotalTime;
uint32_t receivedpackets = 0;
uint32_t sendpackets = 0;
uint32_t numberofmeasurments = 0;

int appPeriodSeconds = TotalTime;
double battery = 10000 * 60 * 60;  // 10000mAh   ..... converted into mAs
double EnergyConsumptionMeasurment = 0;
//double EnergyConsumptionSelfDischarge =0;
double EnergyConsumptionNode = 0;
double TotalRemainingEnergy = 0;

//______________________Print Data________________________________
void
Print (void)
{
  if (TotalRemainingEnergy >= 0)
    TotalRemainingEnergy = battery - EnergyConsumptionNode - EnergyConsumptionMeasurment;
  if (TotalRemainingEnergy <= 0)
    TotalRemainingEnergy = 0;
 // NS_LOG_UNCOND ( battery<<"    "<<TotalRemainingEnergy << "   " <<EnergyConsumptionNode <<  "   " << EnergyConsumptionMeasurment);
  std::ofstream out ("BatteryLevel.txt", std::ios::app);
  out << (Simulator::Now ()).GetSeconds () << " , " << TotalRemainingEnergy << "" << std::endl;
  out.close ();
  Simulator::Schedule (Seconds (60.0), &Print);
}

void
syscurrent (double y, double x)
{
    //NS_LOG_UNCOND ("  hahahahahaha " );
  std::ofstream out ("CurrentGraph.txt", std::ios::app);
  out << (Simulator::Now ()).GetSeconds () << " " << y << std::endl;
  out << (Simulator::Now ()).GetSeconds () << " " << x << std::endl;
  out.close ();
}

//________________________________________________________________
/// Trace function for remaining energy at node.
void
RemainingEnergy (double oldValue, double remainingEnergy)
{
  /*NS_LOG_UNCOND (Simulator::Now ().GetSeconds ()
                 << "s Current remaining energy = " << remainingEnergy << "J");*/
}

/// Trace function for total energy consumption at node.
void
TotalEnergy (double oldValue, double totalEnergy)
{
  //NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "s Total energy consumed by radio = " << totalEnergy << "J");
  EnergyConsumptionNode = totalEnergy;
}
//______________________Measuring value___________________________
void
Measure ()
{
  //NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "Node 0 Measures a Value");
  //NS_LOG_UNCOND ("Old value"<<EnergyConsumptionMeasurment<<"Simulation time"<<Simulator::Now ().GetSeconds () );
  EnergyConsumptionMeasurment += 5.8*4.9;// Measure current * measure time
  //numberofmeasurments += 1;
  Simulator::Schedule (Seconds (60.0), &Measure);
  //NS_LOG_UNCOND ("new value"<<EnergyConsumptionMeasurment<<"Simulation time"<<Simulator::Now ().GetSeconds () );
}
//______________________Measuring value___________________________
void
SelfDischarge ()
{
  //NS_LOG_UNCOND ("Old value" << TotalRemainingEnergy << "Simulation time"
                             //<< Simulator::Now ().GetSeconds ());
  TotalRemainingEnergy =
      TotalRemainingEnergy -
      0.02 * (TotalRemainingEnergy / (30 * day)) *
          day; //EnergyConsumptionSelfDischarge += 5.4;     // battery[id]=battery[id]−sdc_rate[id]∗(battery[id]/sdc_time[id])∗1∗day
  Simulator::Schedule (Seconds (86400.0), &SelfDischarge);
}

int
main (int argc, char *argv[])
{
    
    /*LogComponentEnable ("SigfoxEnergyModelExample", LOG_LEVEL_ALL);
    //LogComponentEnable("LoraChannel", LOG_LEVEL_INFO);
     LogComponentEnable("SigfoxPhy", LOG_LEVEL_ALL);
     LogComponentEnable("EndPointSigfoxPhy", LOG_LEVEL_ALL);
     LogComponentEnable("GatewaySigfoxPhy", LOG_LEVEL_ALL);
    // LogComponentEnable("LoraInterferenceHelper", LOG_LEVEL_ALL);
     LogComponentEnable("SigfoxMac", LOG_LEVEL_ALL);
     LogComponentEnable("EndPointSigfoxMac", LOG_LEVEL_ALL);
    // LogComponentEnable("ClassAEndDeviceLorawanMac", LOG_LEVEL_ALL);
     LogComponentEnable("GatewaySigfoxMac", LOG_LEVEL_ALL);
     //LogComponentEnable("LogicalLoraChannelHelper", LOG_LEVEL_ALL);
    // LogComponentEnable("LogicalLoraChannel", LOG_LEVEL_ALL);
     LogComponentEnable("SigfoxHelper", LOG_LEVEL_ALL);
     LogComponentEnable("SigfoxPhyHelper", LOG_LEVEL_ALL);
    // LogComponentEnable("LorawanMacHelper", LOG_LEVEL_ALL);
    LogComponentEnable("PeriodicSenderHelper", LOG_LEVEL_ALL);
     LogComponentEnable("PeriodicSender", LOG_LEVEL_ALL);
    // LogComponentEnable("LorawanMacHeader", LOG_LEVEL_ALL);
     //LogComponentEnable("LoraFrameHeader", LOG_LEVEL_ALL);
    // LogComponentEnable("NetworkScheduler", LOG_LEVEL_ALL);
    // LogComponentEnable("NetworkServer", LOG_LEVEL_ALL);
    // LogComponentEnable("NetworkStatus", LOG_LEVEL_ALL);
    // LogComponentEnable("NetworkController", LOG_LEVEL_ALL);
      LogComponentEnable ("SimpleEndPointSigfoxPhy", LOG_LEVEL_ALL);
     LogComponentEnable ("SigfoxNetDevice", LOG_LEVEL_ALL);
      //LogComponentEnable ("LoraEnergyModelExample", LOG_LEVEL_ALL);
     LogComponentEnable ("SigfoxRadioEnergyModel", LOG_LEVEL_ALL);
      //LogComponentEnable ("BasicEnergySource", LOG_LEVEL_ALL);
      LogComponentEnable ("Forwarder", LOG_LEVEL_ALL);*/
    LogComponentEnableAll (LOG_PREFIX_FUNC);
    LogComponentEnableAll (LOG_PREFIX_NODE);
    LogComponentEnableAll (LOG_PREFIX_TIME);
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

  /************************
   * Install Energy Model *
   ************************/

  SdcEnergySourceHelper basicSourceHelper;
  SigfoxRadioEnergyModelHelper radioEnergyHelper;

  // configure energy source
  basicSourceHelper.Set ("SdcEnergySourceInitialEnergyJ", DoubleValue (36000000)); // Energy in J
  basicSourceHelper.Set ("SdcEnergySupplyVoltageV", DoubleValue (3.3));

  radioEnergyHelper.Set ("StandbyCurrentA", DoubleValue (0.0108));
  //radioEnergyHelper.Set ("TxCurrentA", DoubleValue (28000));
  radioEnergyHelper.Set ("TxCurrentA", DoubleValue (50));
  radioEnergyHelper.Set ("SleepCurrentA", DoubleValue (0.00235));
  radioEnergyHelper.Set ("RxCurrentA", DoubleValue (13));

  radioEnergyHelper.SetTxCurrentModel ("ns3::ConstantSigfoxTxCurrentModel", "TxCurrent",
                                       DoubleValue (50));

  // install source on EDs' nodes
  EnergySourceContainer sources = basicSourceHelper.Install (endDevices);
  Names::Add ("/Names/EnergySource", sources.Get (0));

  // install device model
  DeviceEnergyModelContainer deviceModels =
      radioEnergyHelper.Install (endDevicesNetDevices, sources);

  /*********************************************************************************/

  Time appStopTime = Seconds (simulationTime);
  PeriodicSenderHelper appHelper = PeriodicSenderHelper ();
  appHelper.SetPeriod (Seconds (600)); //appPeriodSeconds));
  appHelper.SetPacketSize (12); //payload size in bytes
  //Ptr<RandomVariableStream> rv = CreateObjectWithAttributes<UniformRandomVariable> ( "Min", DoubleValue (10), "Max", DoubleValue (10));
  ApplicationContainer appContainer = appHelper.Install (endDevices);

  appContainer.Start (Seconds (0));
  appContainer.Stop (appStopTime);

  /**************
   * Get output *
   **************/

  Ptr<SdcEnergySource> basicSourcePtr = DynamicCast<SdcEnergySource> (sources.Get (0));
  //Simulator::Schedule (Seconds (60.0), &GetRemainingEnergy);
  basicSourcePtr->TraceConnectWithoutContext ("RemainingEnergy", MakeCallback (&RemainingEnergy));

  Ptr<DeviceEnergyModel> basicRadioModelPtr =
      basicSourcePtr->FindDeviceEnergyModels ("ns3::SigfoxRadioEnergyModel").Get (0);
  NS_ASSERT (basicRadioModelPtr != NULL);
  basicRadioModelPtr->TraceConnectWithoutContext ("TotalEnergyConsumption",
                                                  MakeCallback (&TotalEnergy));

  basicRadioModelPtr->TraceConnectWithoutContext ("SystemCurrent", MakeCallback (&syscurrent));

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

  return 0;
}

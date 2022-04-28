# ns3-sigfox
This repository contains the code for the Sigfox module for network simulator 3

**Contents:**
Overview
Authors
Documentation
Code repository
Current implementation
Features
Limitations
Examples
Installing the Sigfox module
Future Work

**Overview**


This is an ns-3 module that can be used to perform simulations of a Sigfox network. Aside from models representing the various components of the network and helpers to set them up, the module comes with a packet tracker that monitors network performance, and with facilities that print the network topology to file for easy debugging and simulation monitoring. 

**Authors**
Muhammad Naeem
Michele Albano
Davide Magrin

**Documentation**
This module has been documented using UML diagrams. The UML diagrams for the current release are shown below. This documentation will be available in the ns-3 models documentation once the module is released.

**Code repository**
The code can be found here: (add a link) This includes a ready-to-use version of ns-3.33 with my module.
Additionally, the user can also download a (add a link of folder) To use it, the user must then copy the folder to the src/ folder of ns-3.


**Features**
•	Capable of generating the current consumption behaviour of the Sigfox sensor node.
•	The module can simulate the overall battery life of the sensor node.
•	Capable of simulating large network, having several thousands of devices.
Limitations
•	Network Server is used in this simulation since performance metrics are collected through the GW trace sources, and packets don't require an acknowledgement.

**Examples**
The module contains two following example models to simulate the model:
•	sigfox-energy-model-example.cc
This example shows Sigfox energy model. This example keeps track of the current consumption values and battery level and computes some statistics at the end of the simulation.

•	large-scale-network-example.cc
This example shows configuring a large network using the ns-3 Sigfox module. A big network features several thousand devices and tens of gateways. Each device is equipped with a ‘PeriodicSender’ application that periodically sends a packet to the network server through the Gateways. The example keeps track of the sent, received, and lost packets, and computes some statistics at the end of the simulation. No Network Server is used in this simulation since performance metrics are collected through the GW trace sources, and packets don't require an acknowledgement.

**Installing the Sigfox module**

We need a GitHub link to explain how to download module

At last, it's a matter of configure/build/run an example

./waf configure --enable-examples 
./waf build
./waf --run src/sigfox/examples/sigfox-energy-model-example 
./waf --run src/sigfox/examples/large-scale-network-example   


**Future Work**
•	implementation of a complete Sigfox IoT solution including network server

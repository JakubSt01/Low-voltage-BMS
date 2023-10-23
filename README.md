# Low-voltage-BMS

Authors: Jakub Stelmach, Paweł Oleszczak

Project Assumptions:

The main goal of the project is to create a comprehensive battery management and protection system for a lithium-ion battery pack. 
The battery voltage should remain within the range of 9 to 13 volts, and the battery itself is composed of 3 sets of 18350 cells connected in series.
Our application is designed to provide protection against over-discharge, monitor the temperature of 4/8 points to prevent cell overheating,
safeguard against excessive output current (up to 150A), and measure it in real-time. Additionally, it will measure the input current for the electronics
and detect charging, while also ensuring cell balancing. The entire system is intended to be maintenance-free, with additional features configurable via jumpers on the PCB.

The purpose of this system is not only to protect and manage batteries used in various household devices but also to function as a power management system for the combustion engine race car being developed by AGH Racing.

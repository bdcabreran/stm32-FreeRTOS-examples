# FreeRTOS Examples for Nucleo F103RB using J-Link with Ozone and System View Software

This repository contains FreeRTOS examples for the Nucleo F103RB development board. These examples are designed to be used with J-Link with Ozone and System View software for debugging and tracing.

## Prerequisites
To use these examples, you will need the following:

* Nucleo F103RB development board
* J-Link hardware debugger
* Ozone software for debugging
* System View software for tracing
* STM32CubeMX software for generating code
* FreeRTOS kernel

## Getting Started
1. Clone this repository to your local machine.
2. Open STM32CubeMX software and import the project from the appropriate example folder in the repository.
3. Generate the code and open the project in your preferred IDE (such as Eclipse or STM32CubeIDE).
4. Configure the project for your specific hardware (such as updating the clock configuration).
Build the project and load it onto your Nucleo F103RB board using J-Link.
5. Open Ozone software and connect to the J-Link debugger.
6. Start debugging the project using Ozone software.
7. Open System View software and connect to the trace output of the project.
8. Start tracing the project using System View software.

## Examples
This repository contains the following FreeRTOS examples for the Nucleo F103RB board:

* STM32 Project with SWO enabled using J-link to print out debug messages. 

### Acknowledgments
This repository is based on the FreeRTOS example projects for STM32 Nucleo-64 boards provided by STMicroelectronics.
Introduction
=

The main goal of this project was to build a clock that uses low-power features of LPC812 microcontroller.

A clock built using 

* NXP LPC812M101JD20 ARM Cortex-M0+ microcontroller (SO20 package).
* NXP PCA2129T Real Time Clock (SO16 package).
* LCD from Ericsson GA628 mobile phone.
* CSI 24C32WI 32kb EEPROM (SO8 package).
* 3V Microchip MCP1702 Low Quiescent Current LDO Regulator ( SOT-23A package).
* 5V Microchip MCP1702 Low Quiescent Current LDO Regulator ( SOT-23A package).
* Resistors and capacitors (1206 package).
* 3 push buttons and 2 LEDs.
* Photo-etched double-sided PCBs.

Notes
=

* The LCD is described in my othe post: Ericsson GA628 LCD with Bus Pirate
* Microcontroller, RTC and LCD are powered from 3V voltage regulator.
* LCD is also powered from 5V voltage regulator.
* 9V battery is chosen because it is easy to get 3V and 5V with voltage regulators.
* Communication between micro-controller and peripherals is done using I2C bus.
* Clock is updated every second or minute.
* Between updates, microcontroller is in Power-down sleep mode.
* The board is powered from 9V alkaline battery which has capacity 565mAh (according to wikipedia).
* Average current consumption is 65µA, that means clock will run about 1 year.


Project uses external library LPCOpen Software Development Platform 2.01
that is available freely from NXP.
The library was built with the following options:
"Optimization: Level 3(-O3)", "One ELF Section per Function".

Links
=
More info at http://kbiva.wordpress.com/2014/05/28/low-power-clock-with-lpc812/

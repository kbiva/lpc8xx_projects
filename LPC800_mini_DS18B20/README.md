Introduction
=
LPC800 Mini-Kit, Nokia 6020 LCD and DS18B20 thermometers.

Shows temperatures of DS18B20 digital thermometers on LCD.

Hardware
=
* DS18B20 digital thermometers.
* Custom made Nokia 6020 LCD backpack.
* NXP LPC800-Mini-Kit: MCU clock is 12Mhz from internal oscillator.

Software
=
Used peripherals: 

* GPIO.
* SPI0 (9-bit, speed is 12Mhz).
* Multi-Rate Timer (MRT) for microseconds delay.
* Self wake-up timer (WKT) for waking up from power-down.
* Power Management Unit (PMU).

Used Reduced power modes: 
* Power-down.

In order to fit compiled code in 4Kb following options we used:
"Use MicroLIB", "Optimization: Level 3(-O3)", "One ELF Section per Function"

Project uses external libraries:

* LPCOpen Software Development Platform 2.01
* 1-Wire library.
* DS18B20 library.

The libraries were built with the following options:
"Optimization: Level 3(-O3)", "One ELF Section per Function".

Links
=
More info at http://kbiva.wordpress.com/2014/11/04/lpc800-mini-kit-nokia-6020-lcd-and-ds18b20-thermometers/

Video: http://youtu.be/gxZD8Vbii2c

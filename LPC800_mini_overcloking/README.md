=
WARNING
Do not try this at home. Your LPC810 may be damaged if operating at more than 30Mhz.
The source code is purely for educational purposes on how to set MCU clock speed and
so a simple benchmark.

Introduction
=
LPC800 Mini-Kit and Nokia 6020 LCD.
Shows how to dinamically set MCU speed using PLL.
Overclocks MCU up to 72Mhz. Maximal safe speed is 30Mhz according to datasheet.
Sets MCU speed from 2 Mhz to 72 Mhz in small increments, diplays 100 filled rectangles
on LCD, and shows taken time in milliseconds.
Communication between board and LCD is 9-bit SPI at MCU speed.

Hardware
=
* Custom made Nokia 6020 LCD backpack.
* NXP LPC800-Mini-Kit: MCU clock is 12Mhz from internal oscillator.

Software
=
Used peripherals: 

* GPIO.
* SPI0 (9-bit, speed is the same as MCU clock (2Mhz -72Mhz).
* Multi-Rate Timer (MRT) for millisesonds delay. 

In order to fit compiled code in 4Kb following options we used:
"Use MicroLIB", "Optimization: Level 2(-O2)", "One ELF Section per Function"

Project uses external libraries:

* LPCOpen Software Development Platform 2.01
* xprintf library

The libraries were built with the following options:
"Optimization: Level 3(-O3)", "One ELF Section per Function".

Links
=
More info at :

Video: 

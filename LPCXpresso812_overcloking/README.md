=
WARNING
Do not try this at home. Your LPC812 may be damaged if operating at more than 30Mhz.
The source code is purely for educational purposes on how to set MCU clock speed and
so a simple benchmark.

Introduction
=
LPCXpresso LPC812 and Nokia 6100 LCD.
Shows how to dinamically set MCU speed using PLL.
Overclocks MCU up to 72Mhz. Maximal safe speed is 30Mhz according to datasheet.
Sets MCU speed from 2 Mhz to 72 Mhz in small increments, diplays 100 filled rectangles
on LCD, and shows taken time in milliseconds.
Communication between board and LCD is 9-bit SPI at MCU speed.
At 72Mhz I had to slow down SPI speed a little bit because Nokia 6100 LCD couldn't handle it.

Hardware
=
* Custom made Nokia 6100 LCD backpack.
* NXP LPCXpresso LPC812: MCU clock is 12Mhz from internal oscillator.

Software
=
Used peripherals: 

* GPIO.
* SPI0 (9-bit, speed is the same as MCU clock (2Mhz -72Mhz).
* Multi-Rate Timer (MRT) for millisesonds delay. 

Following options we used:
"Use MicroLIB", "Optimization: Level 2(-O2)", "One ELF Section per Function"

Project uses external libraries:

* LPCOpen Software Development Platform 2.01
* xprintf library

The libraries were built with the following options:
"Optimization: Level 3(-O3)", "One ELF Section per Function".

Links
=
More info at: 

Video: 

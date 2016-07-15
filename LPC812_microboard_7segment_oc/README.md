Introduction
=
LPC812 microboard and 2.5 digits indicator.
Overclocks MCU up to 72Mhz.
Displays current frequency on a 2.5 digits indicator.
LED blinks at constant speed (period is 1s at any MCU frequency)

Hardware
=
* 2.5 digits indicator board.
* LPC812 microboard.
* One LED.

Software
=
Used peripherals: 

* GPIO.
* SPI0 (16-bit, speed is the same as MCU clock (2Mhz - 72Mhz).
* Multi-Rate Timer (MRT) for millisesonds delay. 

Following options we used:
"Use MicroLIB", "Optimization: Level 2(-O2)", "One ELF Section per Function"

Project uses external libraries:

* LPCOpen 2.19 library for LPC8xx
* 2.5 digits indicator library

The libraries were built with the following options:
"Optimization: Level 3(-O3)", "One ELF Section per Function".

Links
=
More info at: http://wp.me/p31f2I-oH

Video: http://youtu.be/0Xp2Hgbzp0Y

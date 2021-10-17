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

Project uses external libraries:

* LPCOpen 3.02 library for LPC8xx
* 2.5 digits indicator library

Links
=
More info at: http://wp.me/p31f2I-oH

Video: http://youtu.be/0Xp2Hgbzp0Y

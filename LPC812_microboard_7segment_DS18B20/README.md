Introduction
=
LPC812 microboard, DS18B20 thermometer and 2.5 digits indicator.
First it displays 64 bit serial number of the connected thermometer 
and then displays current temperature on a 2.5 digits indicator.
LED blinks at constant speed (period is 1s).

Hardware
=
* 2.5 digits indicator board.
* LPC812 microboard (MCU clock is 12Mhz from internal oscillator).
* One LED.
* DS18B20 termomether.

Software
=
Used peripherals: 

* GPIO.
* SPI0 (16-bit, speed is the same as MCU clock.
* Multi-Rate Timer (MRT) for millisesonds delay. 

Project uses external libraries:

* LPCOpen 3.02 library for LPC8xx
* 2.5 digits indicator library
* 1-Wire library
* DS18B20 library

Links
=
More info at: http://wp.me/p31f2I-oH

Video: http://youtu.be/0Xp2Hgbzp0Y

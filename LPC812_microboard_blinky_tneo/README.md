Introduction
=
LPC812 microboard and eight LEDs.
One LED is blinking every second.
The other LEDs are blinking with following periods:
50ms,100ms,200ms,400ms,800ms,1600ms and 3200ms.
Using real-time kernel TNeo. Each LED is controlled by separate task (total 8 tasks).

Hardware
=
* LPC812 microboard. (MCU clock is 12Mhz from internal oscillator).
* Eight LEDs.

Software
=
Used peripherals: 

* GPIO.

Following options we used:
"Use MicroLIB", "Optimization: Level 2(-O2)", "One ELF Section per Function"

Project uses external libraries:

* LPCOpen 2.19 library for LPC8xx
* TNeo library

The libraries were built with the following options:
"Optimization: Level 3(-O3)", "One ELF Section per Function".

Links
=
More info at: http://wp.me/p31f2I-oH

Video: http://youtu.be/0Xp2Hgbzp0Y

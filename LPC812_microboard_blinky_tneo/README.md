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

Connections
=
* P0_17 -> resistor -> LED -> 3.3V
* P0_14 -> resistor -> LED -> 3.3V
* P0_0 -> resistor -> LED -> 3.3V
* P0_6 -> resistor -> LED -> 3.3V
* P0_7 -> resistor -> LED -> 3.3V
* P0_8 -> resistor -> LED -> 3.3V
* P0_9 -> resistor -> LED -> 3.3V
* P0_1 -> resistor -> LED -> 3.3V

Software
=
Used peripherals: 

* GPIO.

Project uses external libraries:

* LPCOpen 3.02 library for LPC8xx
* TNeo library

Introduction
=
LPC800 Mini-Kit, 2.5 digits indicator and IR Receiver.

* Shows received RC-5 codes from IR remote on indicator.
* Reduced power mode switching from remote.

Hardware
=
* IR remote used for testing is from Terratec M3PO player.
This remote uses RC-5 extended protocol (second bit is 0).
As I don't have remote that uses normal RC-5 protocol,
decoding normal RC-5 frames is untested and commented out.
* IR Receiver module is Vishay TSOP34836.
* 2.5 digits indicator on SPI bus.
* NXP LPC800-Mini-Kit: MCU clock is 12Mhz from internal oscillator.

Connections
=
IR receiver:

* VCC to 3.3V
* GND to GND
* Out to PIO0_3

2.5 digits indicator:

* VCC to 3.3V
* GND to GND
* CLK to PIO0_4
* DAT to PIO0_0

Software
=
Used peripherals: 

* GPIO,
* SPI0 (16-bit, speed is 12Mhz),
* Multi-Rate Timer (MRT),
* State Configurable Timer (SCT),
* Power Management Unit (PMU).

Used Reduced power modes:

* Sleep, 
* Deep-sleep, 
* Power-down.

SCT diagram and code were generated in LPCExpresso.

Project uses external library LPCOpen Software Development Platform 3.02
that is available freely from NXP.

Links
=
For RC-5 decoding I used "An Efficient Algorithm for Decoding RC5 Remote Control Signals"
from http://www.clearwater.com.au/code/rc5

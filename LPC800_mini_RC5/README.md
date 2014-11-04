Introduction
=
LPC800 Mini-Kit, Nokia 6100 LCD backpack and IR Receiver.

* Shows received RC-5 codes from IR remote on LCD.
* Shows current reduced power mode.
* Turns on/off LCD from remote.
* Reduced power mode switching from remote.

Hardware
=
* IR remote used for testing is from Terratec M3PO player.
This remote uses RC-5 extended protocol (second bit is 0).
As I don't have remote that uses normal RC-5 protocol,
decoding normal RC-5 frames is untested and commented out.
* IR Receiver module is Vishay TSOP34836 or TSOP34836.
* Custom made Nokia 6100 LCD backpack.
* NXP LPC800-Mini-Kit: MCU clock is 12Mhz from internal oscillator.

Software
=
Used peripherals: 

* GPIO,
* SPI0 (9-bit, speed is 12Mhz),
* Multi-Rate Timer (MRT),
* State Configurable Timer (SCT),
* Power Management Unit (PMU).

Used Reduced power modes: 
* Sleep, 
* Deep-sleep, 
* Power-down.

SCT diagram and code were generated in LPCExpresso.

In order to fit compiled code in 4Kb following options we used:
"Use MicroLIB", "Optimization: Level 3(-O3)", "One ELF Section per Function"

Project uses external library LPCOpen Software Development Platform 2.01
that is available freely from NXP.
The library was built with the following options:
"Optimization: Level 3(-O3)", "One ELF Section per Function".

Links
=
More info at http://kbiva.wordpress.com/2014/01/30/lpc800-mini-kit-nokia-6100-lcd-rc-5-protocol-decoder/

Video: http://youtu.be/cr8GFKLw17s

For RC-5 decoding I used "An Efficient Algorithm for Decoding RC5 Remote Control Signals"
from http://www.clearwater.com.au/code/rc5

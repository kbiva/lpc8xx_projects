Introduction
=

Birthday clock - second iteration of the clock, that I have build.
The main goal of this project was to build a clock that uses low-power features of LPC812 microcontroller.
Changes form the first version:

* Updated and optimised schematics.
* Smaller housing.
* Rotary encoder instead of buttons.
* Powered by much bigger capacity Li-Ion batteries.
* Touch sensor to turn backlight.
* Birthday notification

A clock built using 

* NXP LPC812M101JD20 ARM Cortex-M0+ microcontroller (SO20 package).
* NXP PCA2129T Real Time Clock (SO16 package).
* LCD from Ericsson GA628 mobile phone (controller chip: Philips PCF2103)
* CSI 24C32WI 32kbit (4Kbytes) EEPROM (SO8 package).
* 3V Microchip MCP1702 Low Quiescent Current LDO Regulator (SOT-23A package).
* 5V Microchip MCP1702 Low Quiescent Current LDO Regulator (SOT-23A package).
* Resistors and capacitors (1206 package).
* Rotary encoder and 2 LEDs.
* Touch sensor TTP223 (SOT-23-6L package).

Birthdays

* Capacity: 100 entries.
* Stored in EEPROM.
* When there is a birthday, yellow LED blinks, in the menu you can see who has it.
* Multiple birthdays on the same day are supported.

Notes
=

* The LCD is described in my othe post: Ericsson GA628 LCD with Bus Pirate
* Microcontroller, RTC and LCD are powered from 3V voltage regulator.
* LCD is also powered from 5V voltage regulator.
* Using 2 Li-Ion rechargable batteries connected in series.
* Communication between micro-controller and peripherals is done using I2C bus.
* Clock is updated every second or minute.
* Between updates, microcontroller is in Power-down sleep mode.
* The board is powered from 2 Li-Ion rechargable batteries (capacity 2400mAh)
* Average current consumption is 65µA (optional backlight adds 10µA)


Project uses external library LPCOpen Software Development Platform 3.02
that is available freely from NXP.

Links
=

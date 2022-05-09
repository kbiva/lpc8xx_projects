/*
 * pins.h
 *
 *      Author: Kestutis Bivainis
 */

#ifndef PINS_H_
#define PINS_H_

#include <stdint.h>
#include <stdbool.h>

#define PIN_PCA2129T_INTERRUPT  4

#define PIN_BUTTON1  1
#define PIN_BUTTON2  9
#define PIN_BUTTON3  15

#define IOCON_PIOBUTTON1 IOCON_PIO1
#define IOCON_PIOBUTTON2 IOCON_PIO9
#define IOCON_PIOBUTTON3 IOCON_PIO15

#define PIN_LED1  13
#define PIN_LED2  17

#define IOCON_PIOLED1 IOCON_PIO13
#define IOCON_PIOLED2 IOCON_PIO17

#define PIN_EEPROM_WP  16
#define IOCON_PIOEEPROM_WP IOCON_PIO16

#define IRQ_BUTTON1  1
#define IRQ_BUTTON2  2
#define IRQ_BUTTON3  3
#define IRQ_TIME 4

void init_pins(void);
void configure_SWD(bool value);

#endif

/*
 * PCF2103.h
 *
 *      Author: Kestutis Bivainis
 */

#ifndef PCF2103_H_
#define PCF2103_H_

#define PCF2103_I2C_ADDR_7BIT 0x74

#define PCF2103_LCD_WIDTH 12
#define PCF2103_LCD_SEND_WIDTH (PCF2103_LCD_WIDTH + 2)

#define TENS_PLACE_DIGIT(x) ((x)/10+0xb0)
#define ONES_PLACE_DIGIT(x) ((x)%10+0xb0)

#endif

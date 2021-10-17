/*
 * i2c.h
 *
 *      Author: Kestutis Bivainis
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

#define I2C_BITRATE 400000

void setup_I2C_master(void);

void read_clock(uint8_t AddressI2C, uint8_t reg, uint8_t cnt, uint8_t* val);
void write_clock(uint8_t AddressI2C, uint8_t reg, uint8_t cnt, uint8_t* val);

void write_LCD(uint8_t AddressI2C, uint8_t reg, uint8_t cnt, uint8_t* val);

void read_EEPROM(uint8_t AddressI2C, uint16_t reg, uint8_t cnt, uint8_t* val);
void write_EEPROM(uint8_t AddressI2C, uint16_t reg, uint8_t cnt, uint8_t* val);

#endif

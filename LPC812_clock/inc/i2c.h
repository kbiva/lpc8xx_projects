/*
 * i2c.h
 *
 *      Author: Kestutis Bivainis
 */

#ifndef I2C_H_
#define I2C_H_

#define I2C_BITRATE 400000

void setupI2CMaster(void);
void setValues(uint8_t AddressI2C, uint8_t reg, uint8_t cnt, uint8_t* val);
void readValues(uint8_t AddressI2C, uint8_t reg, uint8_t cnt, uint8_t* val);
void readEEPROM(uint8_t AddressI2C, uint16_t reg, uint8_t cnt, uint8_t* val);

#endif

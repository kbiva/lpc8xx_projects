/*
 * i2c.c
 *
 *      Author: Kestutis Bivainis
 */

#include "chip.h"
#include "i2c.h"
#include "24C32WI.h"
#include "PCF2103.h"

/* I2C master handle and memory for ROM API */
static I2C_HANDLE_T *i2cHandleMaster;

/* Use a buffer size larger than the expected return value of
   i2c_get_mem_size() for the static I2C handle type */
static uint32_t i2cMasterHandleMEM[0x20];

static void errorI2C(void) {
  while (1) {}
}

/* Setup I2C handle and parameters */
void setup_I2C_master(void) {
  /* Enable I2C clock and reset I2C peripheral - the boot ROM does not do this */
  Chip_I2C_Init(LPC_I2C);

  /* Perform a sanity check on the storage allocation */
  if (LPC_I2CD_API->i2c_get_mem_size() > sizeof(i2cMasterHandleMEM)) {
    /* Example only: this should never happen and probably isn't needed for most I2C code. */
    errorI2C();
  }

  /* Setup the I2C handle */
  i2cHandleMaster = LPC_I2CD_API->i2c_setup(LPC_I2C_BASE, i2cMasterHandleMEM);
  if (i2cHandleMaster == NULL) {
    errorI2C();
  }

  /* Set I2C bitrate */
  if (LPC_I2CD_API->i2c_set_bitrate(i2cHandleMaster, Chip_Clock_GetSystemClockRate(),I2C_BITRATE) != LPC_OK) {
    errorI2C();
  }
}

/* Master transmit in polling mode */
void write_clock(uint8_t AddressI2C, uint8_t reg, uint8_t cnt, uint8_t* val) {

  uint8_t SendData[16];
  I2C_PARAM_T param;
  I2C_RESULT_T result;
  ErrorCode_t error_code;
  uint32_t index = 2;

  /* 7-bit address */
  SendData[0] = AddressI2C;
  SendData[1] = reg;
  while (cnt--) {
    SendData[index++] = *val++;
  }

  /* Setup I2C parameters for number of bytes with stop - appears as follows on bus:
     Start - address7 or address10upper - ack
     (10 bits addressing only) address10lower - ack
     value 1 - ack
     value 2 - ack - stop */
  param.num_bytes_send  = index;
  param.buffer_ptr_send = &SendData[0];
  param.num_bytes_rec   = 0;
  param.stop_flag       = 1;

  /* Set timeout (much) greater than the transfer length */
  error_code=LPC_I2CD_API->i2c_set_timeout(i2cHandleMaster, 100000);

  /* Do master write transfer */
  error_code = LPC_I2CD_API->i2c_master_transmit_poll(i2cHandleMaster, &param, &result);

  /* Completed without erors? */
  if (error_code != LPC_OK) {
    /* Likely cause is NAK */
    errorI2C();
  }
  /* Note results are only valid when there are no errors */
}

void read_clock(uint8_t AddressI2C, uint8_t reg, uint8_t cnt, uint8_t* val) {

  I2C_PARAM_T param;
  I2C_RESULT_T result;
  ErrorCode_t error_code;

  write_clock(AddressI2C,reg,0,0);

  val[0] = AddressI2C | 0x01;

  /* Setup parameters for transfer */
  param.num_bytes_rec  = cnt+1;
  param.buffer_ptr_rec = &val[0];
  param.stop_flag      = 1;

  /* Set timeout (much) greater than the transfer length */
  LPC_I2CD_API->i2c_set_timeout(i2cHandleMaster, 100000);

  /* Do master read transfer */
  error_code = LPC_I2CD_API->i2c_master_receive_poll(i2cHandleMaster, &param, &result);

  /* Completed without erors? */
  if (error_code != LPC_OK) {
    /* Likely cause is NAK */
    errorI2C();
  }
}

void write_LCD(uint8_t AddressI2C, uint8_t reg, uint8_t cnt, uint8_t* val) {

  uint8_t SendData[PCF2103_LCD_SEND_WIDTH + 2];
  I2C_PARAM_T param;
  I2C_RESULT_T result;
  ErrorCode_t error_code;
  uint32_t index = 2;

  /* 7-bit address */
  SendData[0] = AddressI2C;
  SendData[1] = reg;
  while (cnt--) {
    SendData[index++] = *val++;
  }

  /* Setup I2C parameters for number of bytes with stop - appears as follows on bus:
     Start - address7 or address10upper - ack
     (10 bits addressing only) address10lower - ack
     value 1 - ack
     value 2 - ack - stop */
  param.num_bytes_send  = index;
  param.buffer_ptr_send = &SendData[0];
  param.num_bytes_rec   = 0;
  param.stop_flag       = 1;

  /* Set timeout (much) greater than the transfer length */
  error_code=LPC_I2CD_API->i2c_set_timeout(i2cHandleMaster, 100000);

  /* Do master write transfer */
  error_code = LPC_I2CD_API->i2c_master_transmit_poll(i2cHandleMaster, &param, &result);

  /* Completed without erors? */
  if (error_code != LPC_OK) {
    /* Likely cause is NAK */
    errorI2C();
  }
  /* Note results are only valid when there are no errors */
}

void read_EEPROM(uint8_t AddressI2C, uint16_t reg, uint8_t cnt, uint8_t* val) {

  I2C_PARAM_T param;
  I2C_RESULT_T result;
  ErrorCode_t error_code;
  uint8_t sendData[3];
  uint8_t readData[_24C32WI_PAGE_SIZE + 1];
  uint32_t index = 1;

  sendData[0] = AddressI2C | 0x01;
  sendData[1] = reg >> 8;
  sendData[2] = (uint8_t)reg;

  readData[0] = AddressI2C | 0x01;

  /* Setup parameters for transfer */
  param.num_bytes_send  = 3;
  param.buffer_ptr_send = &sendData[0];
  param.num_bytes_rec   = cnt+1;
  param.buffer_ptr_rec  = &readData[0];
  param.stop_flag       = 1;

  /* Set timeout (much) greater than the transfer length */
  LPC_I2CD_API->i2c_set_timeout(i2cHandleMaster, 100000);

  /* Do master read transfer */
  error_code = LPC_I2CD_API->i2c_master_tx_rx_poll(i2cHandleMaster, &param, &result);

  /* Completed without erors? */
  if (error_code != LPC_OK) {
    /* Likely cause is NAK */
    errorI2C();
  }
  while (cnt--) {
    *val++ = readData[index++];
  }
}

void write_EEPROM(uint8_t AddressI2C, uint16_t reg, uint8_t cnt, uint8_t* val) {

  uint8_t SendData[_24C32WI_PAGE_SIZE + 3];
  I2C_PARAM_T param;
  I2C_RESULT_T result;
  ErrorCode_t error_code;
  uint32_t index = 3;

  /* 7-bit address */
  SendData[0] = AddressI2C;
  SendData[1] = reg >> 8;
  SendData[2] = (uint8_t)reg;

  while (cnt--) {
    SendData[index++] = *val++;
  }

  /* Setup I2C parameters for number of bytes with stop - appears as follows on bus:
     Start - address7 or address10upper - ack
     (10 bits addressing only) address10lower - ack
     value 1 - ack
     value 2 - ack - stop */
  param.num_bytes_send  = index;
  param.buffer_ptr_send = &SendData[0];
  param.num_bytes_rec   = 0;
  param.stop_flag       = 1;

  /* Set timeout (much) greater than the transfer length */
  error_code=LPC_I2CD_API->i2c_set_timeout(i2cHandleMaster, 100000);

  /* Do master write transfer */
  error_code = LPC_I2CD_API->i2c_master_transmit_poll(i2cHandleMaster, &param, &result);

  /* Completed without erors? */
  if (error_code != LPC_OK) {
    /* Likely cause is NAK */
    errorI2C();
  }
  /* Note results are only valid when there are no errors */
}

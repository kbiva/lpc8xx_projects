/*
 * 24C32WI.h
 *
 *      Author: Kestutis Bivainis
 */

#ifndef _24C32WI_H_
#define _24C32WI_H_

#define _24C32WI_I2C_ADDR_7BIT 0xA0
#define _24C32WI_SETTINGS_ADDRESS 0x00

#define _24C32WI_SETTINGS_LENGTH 7

#define ENTER 1
#define NEXT 2
#define UPDOWN 3
  
#define UPDATE_1_SEC 1
#define UPDATE_1_MIN 2

#define FORMAT_HHMMSS 0
#define FORMAT_HHMM 1
#define FORMAT_HHMM_MIDD 2
#define FORMAT_MIDD_HHMM 3
#define FORMAT_HHMMSSYYMIDD 4

#define ICONSMODE_POWER 0
#define ICONSMODE_WEEKDAYS 1
#define ICONSMODE_NONE 2

#endif

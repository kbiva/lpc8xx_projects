#ifndef CLOCK_H_
#define CLOCK_H_

#include <stdint.h>
#include "PCF2103.h"

/* System oscillator rate and clock rate on the CLKIN pin */
const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

typedef enum _ICONSMODE {
  ICONSMODE_POWER = 0x00,
  ICONSMODE_WEEKDAYS = 0x01,
  ICONSMODE_NONE = 0x02
} ICONSMODE;

typedef enum _BLINK {
  BLINK_OFF = 0x00,
  BLINK_ON = 0x01,
  BLINK_1MIN = 0x02
} BLINK;

typedef enum _DISPLAYFORMAT {
  DISPLAYFORMAT_HHMMSS_SEP = 0x01,
  DISPLAYFORMAT_HHMM_SEP = 0x02,
  DISPLAYFORMAT_HHMMSS = 0x03,
  DISPLAYFORMAT_HHMM = 0x04,
  DISPLAYFORMAT_HHMM_MIDD = 0x05,
  DISPLAYFORMAT_MIDD_HHMM = 0x06,
  DISPLAYFORMAT_HHMMSS_MIDD = 0x07,
  DISPLAYFORMAT_HHMMSSYYMIDD = 0x08,
  DISPLAYFORMAT_YYMIDDHHMMSS = 0x09
} DISPLAYFORMAT;

typedef enum _UPDATEFREQ {
  UPDATEFREQ_DISABLED = 0x00,
  UPDATEFREQ_1_SEC = 0x01,
  UPDATEFREQ_1_MIN = 0x02
} UPDATEFREQ;

#define BIRTHDAYS_COUNT 100
#define BIRTHDAY_DATE_LENGTH 2
#define BIRTHDAY_NAME_LENGTH PCF2103_LCD_WIDTH
#define BIRTHDAY_RECORD_LENGTH (BIRTHDAY_DATE_LENGTH + BIRTHDAY_NAME_LENGTH)

typedef struct {
  uint8_t date[BIRTHDAY_DATE_LENGTH];
  uint8_t name[BIRTHDAY_NAME_LENGTH];
} BIRTHDAY;

#define LED1_OFF Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED1, false);
#define LED1_ON Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED1, true);
#define LED2_OFF Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
#define LED2_ON Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);
#define SLEEP Chip_PMU_Sleep(LPC_PMU, (CHIP_PMU_MCUPOWER_T)sleep);
#define WRITE_LCD(str) write_LCD(PCF2103_I2C_ADDR_7BIT, 0x80, PCF2103_LCD_SEND_WIDTH, str);

#define WRITE_LCD_SLEEP(str) \
   WRITE_LCD(str)\
   SLEEP

#define SET_MENU(str,num) for (uint32_t _i = 0; _i < PCF2103_LCD_WIDTH; _i++) {\
                            str[_i+2] = strMenu[num][_i+2];\
                          }

#endif
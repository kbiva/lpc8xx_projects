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
  BLINK_ON = 0x00,
  BLINK_OFF = 0x01,
  BLINK_1MIN = 0x02
} BLINK;

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

#endif
#ifndef CLOCK_H_
#define CLOCK_H_

#include <stdint.h>
#include <stdbool.h>
#include "PCF2103.h"
#include "24C32WI.h"

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

typedef struct _PACKED {
  bool separator_flip;
  uint8_t freq;
  uint8_t sleep;
  bool scroll;
  uint8_t blink;
  uint8_t icons_mode;
  uint8_t aging_offset;
  bool swd;
  uint8_t format[12];
} PARAMETERS;

typedef union {
  PARAMETERS s;
  uint8_t buf[_24C32WI_SETTINGS_LENGTH];
} SETTINGS;

#define LED1_OFF Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED1, false);
#define LED1_ON Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED1, true);
#define LED2_OFF Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
#define LED2_ON Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);
#define SLEEP Chip_PMU_Sleep(LPC_PMU, (CHIP_PMU_MCUPOWER_T)settings.s.sleep);
#define WRITE_LCD(str) write_LCD(PCF2103_I2C_ADDR_7BIT, 0x80, PCF2103_LCD_SEND_WIDTH, str);

#define WRITE_LCD_SLEEP(str) \
   WRITE_LCD(str)\
   SLEEP

#endif
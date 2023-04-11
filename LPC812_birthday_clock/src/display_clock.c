/*
 * display_clock.c
 *
 *      Author: Kestutis Bivainis
 */

#include "chip.h"
#include "PCA2129T.h"
#include "pins.h"
#include "i2c.h"
#include "clock.h"

extern SETTINGS settings;
extern volatile bool clock_interrupt;
static bool birthday = false;

// time and date read from PCA2129T
static uint8_t time_and_date[9];

static uint32_t old_day;

extern BIRTHDAY birthdays[];

static uint8_t icons[PCF2103_LCD_SEND_WIDTH]=
  {0x4C,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10};

static uint8_t buffer_LCD[PCF2103_LCD_SEND_WIDTH]=
  {0x8C,0x40,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};

extern bool sep_flip;

extern uint32_t max_scroll_position;
extern uint32_t scroll_position;
extern bool scroll_direction;

static uint8_t months[][3]={{},
                            {'J'+0x80,'a'+0x80,'n'+0x80},
                            {'F'+0x80,'e'+0x80,'b'+0x80},
                            {'M'+0x80,'a'+0x80,'r'+0x80},
                            {'A'+0x80,'p'+0x80,'r'+0x80},
                            {'M'+0x80,'a'+0x80,'y'+0x80},
                            {'J'+0x80,'u'+0x80,'n'+0x80},
                            {'J'+0x80,'u'+0x80,'l'+0x80},
                            {'A'+0x80,'u'+0x80,'g'+0x80},
                            {'S'+0x80,'e'+0x80,'p'+0x80},
                            {'O'+0x80,'c'+0x80,'t'+0x80},
                            {'N'+0x80,'o'+0x80,'v'+0x80},
                            {'D'+0x80,'e'+0x80,'c'+0x80}};

void display_clock(void) {

  uint32_t i;
  uint32_t month, day;

  switch (settings.s.blink) {
    case BLINK_OFF:break;
    case BLINK_ON:
      if (birthday) {
        LED2_ON
      }
      else {
        LED1_ON
      }
      break;
    case BLINK_1MIN:
      if ((settings.s.freq == UPDATEFREQ_1_SEC) && (time_and_date[2] == 0x59) ||
          (settings.s.freq == UPDATEFREQ_1_MIN) ) {
        if (birthday) {
          LED2_ON
        }
        else {
          LED1_ON
        }
      }
      break;
  }

  if (clock_interrupt) {

    clock_interrupt = false;

    read_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_CONTROL3_REGISTER, PCA2129T_CONTROL3_LENGTH + PCA2129T_TIMEDATE_LENGHT + 1, time_and_date);
    // mask OSF from seconds
    time_and_date[2] &= 0x7F;

    if (old_day != time_and_date[5]) {
      // new day -> check for birthdays
      month = time_and_date[7];
      day = time_and_date[5];

      for (i = 0; i < BIRTHDAYS_COUNT; i++ ) {
        if ((birthdays[i].date[0] == month) && (birthdays[i].date[1] == day)) {
          birthday = true;
          switch (settings.s.blink) {
            case BLINK_OFF: break;
            case BLINK_ON:
            case BLINK_1MIN:
              LED1_OFF
            break;
          }
          break;
        }
      }
      if (i == BIRTHDAYS_COUNT) {
        birthday = false;
        switch (settings.s.blink) {
          case BLINK_OFF: break;
          case BLINK_ON:
          case BLINK_1MIN:
            LED2_OFF
          break;
        }
      }
      old_day = time_and_date[5];
    }

    //clear icons
    for (uint32_t i = 2; i < PCF2103_LCD_SEND_WIDTH; i++){
      icons[i] = 0x00;
    }
    //clear lcd
    for (uint32_t i = 2; i < PCF2103_LCD_SEND_WIDTH; i++){
      buffer_LCD[i] = 0x20;
    }

    switch (settings.s.icons_mode) {
      case ICONSMODE_POWER:
        icons[12] = time_and_date[1] & 0x04 ? 0x04 : 0x1F;
        icons[13] = 0x10;
        switch (settings.s.sleep) {
          case PMU_MCU_SLEEP: icons[2] = 0x10; break;
          case PMU_MCU_DEEP_SLEEP: icons[2] = 0x1C; break;
          case PMU_MCU_POWER_DOWN: icons[2] = 0x1F; break;
          default: icons[2] = 0x00; break;
        }
        break;
      case ICONSMODE_WEEKDAYS:
        switch (time_and_date[6]) {
          case 0: icons[12] = 0x1F; icons[13] = 0x10; break;
          case 1: icons[2] = 0x1F; break;
          case 2: icons[4] = 0x1E; break;
          case 3: icons[5] = 0x18; break;
          case 4: icons[7] = 0x10; break;
          case 5: icons[8] = 0x18; break;
          case 6: icons[10] = 0x10; break;
        }
        break;
      case ICONSMODE_NONE:
        break;
    }

    WRITE_LCD(icons)

    i = 0;
    while (i < 12 - scroll_position) {
      switch (settings.s.format[i]) {
        case 'H':
          buffer_LCD[scroll_position + 2 + i] = FROM_BCD_HIGH(time_and_date[4]) + 0xb0;
          buffer_LCD[scroll_position + 3 + i] = FROM_BCD_LOW(time_and_date[4]) + 0xb0;
          i += 2;
          break;
        case 'M':
          buffer_LCD[scroll_position + 2 + i] = FROM_BCD_HIGH(time_and_date[3]) + 0xb0;
          buffer_LCD[scroll_position + 3 + i] = FROM_BCD_LOW(time_and_date[3]) + 0xb0;
          i += 2;
          break;
        case 'S':
          buffer_LCD[scroll_position + 2 + i] = FROM_BCD_HIGH(time_and_date[2]) + 0xb0;
          buffer_LCD[scroll_position + 3 + i] = FROM_BCD_LOW(time_and_date[2]) + 0xb0;
          i += 2;
          break;
        case 'O':
          buffer_LCD[scroll_position + 2 + i] = FROM_BCD_HIGH(time_and_date[7]) + 0xb0;
          buffer_LCD[scroll_position + 3 + i] = FROM_BCD_LOW(time_and_date[7]) + 0xb0;
          i += 2;
          break;
        case 'N':
          buffer_LCD[scroll_position + 2 + i] = months[FROM_BCD(time_and_date[7])][0];
          buffer_LCD[scroll_position + 3 + i] = months[FROM_BCD(time_and_date[7])][1];
          buffer_LCD[scroll_position + 4 + i] = months[FROM_BCD(time_and_date[7])][2];
          i += 3;
          break;
        case 'D':
          buffer_LCD[scroll_position + 2 + i] = FROM_BCD_HIGH(time_and_date[5]) + 0xb0;
          buffer_LCD[scroll_position + 3 + i] = FROM_BCD_LOW(time_and_date[5]) + 0xb0;
          i += 2;
          break;
        case 'Y':
          buffer_LCD[scroll_position + 2 + i] = FROM_BCD_HIGH(time_and_date[8]) + 0xb0;
          buffer_LCD[scroll_position + 3 + i] = FROM_BCD_LOW(time_and_date[8]) + 0xb0;
          i += 2;
          break;
        case ':':
          if (settings.s.separator_flip) {
            if (sep_flip)
              buffer_LCD[scroll_position + 2 + i] = '-' + 0x80;
            else
              buffer_LCD[scroll_position + 2 + i] = ':' + 0x80;
            sep_flip = !sep_flip;
          }
          else
            buffer_LCD[scroll_position + 2 + i] = ':' + 0x80;
          i++;
          break;
        case '.':
          buffer_LCD[scroll_position + 2 + i] = '.' + 0x80;
          i++;
          break;
        case ' ':
          buffer_LCD[scroll_position + 2 + i] = ' ' + 0x80;
          i++;
          break;
        default:
          i++;
          break;
      }
    }

    WRITE_LCD(buffer_LCD)

    if (settings.s.scroll && max_scroll_position) {
      if (scroll_direction) {
        if (scroll_position == max_scroll_position) {
           scroll_direction = false;
           scroll_position--;
        }
        else scroll_position++;
      }
      else {
        if (scroll_position == 0) {
          scroll_direction = true;
          scroll_position++;
        }
        else scroll_position--;
      }
    }
  }

  switch (settings.s.blink) {
    case BLINK_OFF:break;
    case BLINK_ON:
    case BLINK_1MIN:
      if (birthday) {
        LED2_OFF
      }
      else {
        LED1_OFF
      }
      break;
  }
}

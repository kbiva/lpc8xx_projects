/*
 * clock.c
 *
 *      Author: Kestutis Bivainis
 */

#include "chip.h"
#include "PCA2129T.h"
#include "i2c.h"
#include "PCF2103.h"
#include "24C32WI.h"
#include "pins.h"
#include "clock.h"

extern volatile int16_t counter;
static volatile int16_t old_counter;
extern volatile bool button_state;
extern volatile bool clock_interrupt;

static uint32_t menu_position;

// true:clock, false:settings
static volatile bool clock_mode = true;

static uint8_t strMenu[][PCF2103_LCD_SEND_WIDTH]={
    //0 Birthdays today
    {0x8C,0x40,'T'+0x80,'o'+0x80,'d'+0x80,'a'+0x80,'y'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x77},
    //1 Settings
    {0x8C,0x40,'S'+0x80,'e'+0x80,'t'+0x80,'t'+0x80,'i'+0x80,'n'+0x80,'g'+0x80,'s'+0x80,0x20,0x20,0x20,0x77},
    //2 Power mode
    {0x8C,0x40,'P'+0x80,'o'+0x80,'w'+0x80,'e'+0x80,'r'+0x80,0x20,'m'+0x80,'o'+0x80,'d'+0x80,'e'+0x80,0x20,0x77},
    //3 Information
    {0x8C,0x40,'I'+0x80,'n'+0x80,'f'+0x80,'o'+0x80,'r'+0x80,'m'+0x80,'a'+0x80,'t'+0x80,'i'+0x80,'o'+0x80,'n'+0x80,0x77},
    //4 Birthdays
    {0x8C,0x40,'B'+0x80,'i'+0x80,'r'+0x80,'t'+0x80,'h'+0x80,'d'+0x80,'a'+0x80,'y'+0x80,'s'+0x80,0x20,0x20,0x77},
    //5 Tests
    {0x8C,0x40,'T'+0x80,'e'+0x80,'s'+0x80,'t'+0x80,'s'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x77},
    //6 About
    {0x8C,0x40,'A'+0x80,'b'+0x80,'o'+0x80,'u'+0x80,'t'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x77},
    //7 Exit
    {0x8C,0x40,'E'+0x80,'x'+0x80,'i'+0x80,'t'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x77},

    //8 Set date
    {0x8C,0x40,'S'+0x80,'e'+0x80,'t'+0x80,0x20,'d'+0x80,'a'+0x80,'t'+0x80,'e'+0x80,0x20,0x20,0x20,0x77},
    //9 Set time
    {0x8C,0x40,'S'+0x80,'e'+0x80,'t'+0x80,0x20,'t'+0x80,'i'+0x80,'m'+0x80,'e'+0x80,0x20,0x20,0x20,0x77},
    //10 Set format
    {0x8C,0x40,'S'+0x80,'e'+0x80,'t'+0x80,0x20,'f'+0x80,'o'+0x80,'r'+0x80,'m'+0x80,'a'+0x80,'t'+0x80,0x20,0x77},
    //11 Update freq
    {0x8C,0x40,'U'+0x80,'p'+0x80,'d'+0x80,'a'+0x80,'t'+0x80,'e'+0x80,0x20,'f'+0x80,'r'+0x80,'e'+0x80,'q'+0x80,0x77},
    //12 Set scroll
    {0x8C,0x40,'S'+0x80,'e'+0x80,'t'+0x80,0x20,'s'+0x80,'c'+0x80,'r'+0x80,'o'+0x80,'l'+0x80,'l'+0x80,0x20,0x77},
    //13 ppm offset
    {0x8C,0x40,'p'+0x80,'p'+0x80,'m'+0x80,0x20,'o'+0x80,'f'+0x80,'f'+0x80,'s'+0x80,'e'+0x80,'t'+0x80,0x20,0x77},
    //14 LED blink
    {0x8C,0x40,'L'+0x80,'E'+0x80,'D'+0x80,0x20,'b'+0x80,'l'+0x80,'i'+0x80,'n'+0x80,'k'+0x80,0x20,0x20,0x77},
    //15 Icons mode
    {0x8C,0x40,'I'+0x80,'c'+0x80,'o'+0x80,'n'+0x80,'s'+0x80,0x20,'m'+0x80,'o'+0x80,'d'+0x80,'e'+0x80,0x20,0x77},
    //16 Sep. flip
    {0x8C,0x40,'S'+0x80,'e'+0x80,'p'+0x80,'.'+0x80,0x20,'f'+0x80,'l'+0x80,'i'+0x80,'p'+0x80,0x20,0x20,0x77},
    //17 SWD Port
    {0x8C,0x40,'S'+0x80,'W'+0x80,'D'+0x80,0x20,'P'+0x80,'o'+0x80,'r'+0x80,'t'+0x80,0x20,0x20,0x20,0x77},
    //18 Back
    {0x8C,0x40,'B'+0x80,'a'+0x80,'c'+0x80,'k'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x77},

    //19 HH:MM:SS
    {0x8C,0x40,'H'+0x80,'H'+0x80,':'+0x80,'M'+0x80,'M'+0x80,':'+0x80,'S'+0x80,'S'+0x80,0x20,0x20,0x20,0x20},
    //20 HH:MM
    {0x8C,0x40,'H'+0x80,'H'+0x80,':'+0x80,'M'+0x80,'M'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
    //21 HHMMSS
    {0x8C,0x40,'H'+0x80,'H'+0x80,'M'+0x80,'M'+0x80,'S'+0x80,'S'+0x80,0x20,0x20,0x20,0x20,0x20,0x20},
    //22 HHMM
    {0x8C,0x40,'H'+0x80,'H'+0x80,'M'+0x80,'M'+0x80,0x020,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
    //23 HH:MM MI.DD
    {0x8C,0x40,'H'+0x80,'H'+0x80,':'+0x80,'M'+0x80,'M'+0x80,0x20,'M'+0x80,'I'+0x80,'.'+0x80,'D'+0x80,'D'+0x80,0x20},
    //24 MI.DD HH:MM
    {0x8C,0x40,'M'+0x80,'I'+0x80,'.'+0x80,'D'+0x80,'D'+0x80,0x20,'H'+0x80,'H'+0x80,':'+0x80,'M'+0x80,'M'+0x80,0x20},
    //25 HHMMSS MIDD
    {0x8C,0x40,'H'+0x80,'H'+0x80,'M'+0x80,'M'+0x80,'S'+0x80,'S'+0x80,0x20,'M'+0x80,'I'+0x80,'D'+0x80,'D'+0x80,0x20},
    //26 HHMMSSDDMIYY
    {0x8C,0x40,'H'+0x80,'H'+0x80,'M'+0x80,'M'+0x80,'S'+0x80,'S'+0x80,'D'+0x80,'D'+0x80,'M'+0x80,'I'+0x80,'Y'+0x80,'Y'+0x80},
    //27 DDMIYYHHMMSS
    {0x8C,0x40,'D'+0x80,'D'+0x80,'M'+0x80,'I'+0x80,'Y'+0x80,'Y'+0x80,'H'+0x80,'H'+0x80,'M'+0x80,'M'+0x80,'S'+0x80,'S'+0x80},

    //28 1 sec
    {0x8C,0x40,'1'+0x80,0x20,'s'+0x80,'e'+0x80,'c'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //29 1 min
    {0x8C,0x40,'1'+0x80,0x20,'m'+0x80,'i'+0x80,'n'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},

    //30 On scroll
    {0x8C,0x40,'O'+0x80,'n'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //31 Off scroll
    {0x8C,0x40,'O'+0x80,'f'+0x80,'f'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},

    //32 ppm offset:
    {0x8C,0x40,'O'+0x80,'f'+0x80,'f'+0x80,'s'+0x80,'e'+0x80,'t'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20},

    //33 On blink
    {0x8C,0x40,'O'+0x80,'n'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //34 Off blink
    {0x8C,0x40,'O'+0x80,'f'+0x80,'f'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //35 Every minute
    {0x8C,0x40,'E'+0x80,'v'+0x80,'e'+0x80,'r'+0x80,'y'+0x80,0x20,'m'+0x80,'i'+0x80,'n'+0x80,0x20,0x20,0x1f},

    //36 Show Power
    {0x8C,0x40,'S'+0x80,'h'+0x80,'o'+0x80,'w'+0x80,0x20,'p'+0x80,'o'+0x80,'w'+0x80,'e'+0x80,'r'+0x80,0x20,0x1f},
    //37 Weekdays
    {0x8C,0x40,'W'+0x80,'e'+0x80,'e'+0x80,'k'+0x80,'d'+0x80,'a'+0x80,'y'+0x80,'s'+0x80,0x20,0x20,0x20,0x1f},
    //38 None
    {0x8C,0x40,'N'+0x80,'o'+0x80,'n'+0x80,'e'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},

    //39 On Sep. flip
    {0x8C,0x40,'O'+0x80,'n'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //40 Off Sep. flip
    {0x8C,0x40,'O'+0x80,'f'+0x80,'f'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},

    //41 Off SWD Port
    {0x8C,0x40,'O'+0x80,'f'+0x80,'f'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //42 On SWD Port
    {0x8C,0x40,'O'+0x80,'n'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},

    //43 Power-down
    {0x8C,0x40,'P'+0x80,'o'+0x80,'w'+0x80,'e'+0x80,'r'+0x80,'-'+0x80,'d'+0x80,'o'+0x80,'w'+0x80,'n'+0x80,0x20,0x1f},
    //44 Deep-sleep
    {0x8C,0x40,'D'+0x80,'e'+0x80,'e'+0x80,'p'+0x80,'-'+0x80,'s'+0x80,'l'+0x80,'e'+0x80,'e'+0x80,'p'+0x80,0x20,0x1f},
    //45 Sleep
    {0x8C,0x40,'S'+0x80,'l'+0x80,'e'+0x80,'e'+0x80,'p'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},

    //46 Birthday nr:
    {0x8C,0x40,0x20,0x20,':'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //47 Month:
    {0x8C,0x40,'M'+0x80,'o'+0x80,'n'+0x80,'t'+0x80,'h'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20,0x1f},
    //48 Day:
    {0x8C,0x40,'D'+0x80,'a'+0x80,'y'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},

    //49 Font table
    {0x8C,0x40,'F'+0x80,'o'+0x80,'n'+0x80,'t'+0x80,0x20,'t'+0x80,'a'+0x80,'b'+0x80,'l'+0x80,'e'+0x80,0x20,0x77},
    //50 Icons
    {0x8C,0x40,'I'+0x80,'c'+0x80,'o'+0x80,'n'+0x80,'s'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x77},
    //51 LEDs
    {0x8C,0x40,'L'+0x80,'E'+0x80,'D'+0x80,'s'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x77},
    //52 Back
    {0x8C,0x40,'B'+0x80,'a'+0x80,'c'+0x80,'k'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x77},

    //53 Year:
    {0x8C,0x40,'Y'+0x80,'e'+0x80,'a'+0x80,'r'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //54 Month:
    {0x8C,0x40,'M'+0x80,'o'+0x80,'n'+0x80,'t'+0x80,'h'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20,0x1f},
    //55 Day:
    {0x8C,0x40,'D'+0x80,'a'+0x80,'y'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //56 Weekday:
    {0x8C,0x40,'W'+0x80,'e'+0x80,'e'+0x80,'k'+0x80,'d'+0x80,'a'+0x80,'y'+0x80,':'+0x80,0x20,0x20,0x20,0x1f},
    //57 Hour:
    {0x8C,0x40,'H'+0x80,'o'+0x80,'u'+0x80,'r'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //58 Minute:
    {0x8C,0x40,'M'+0x80,'i'+0x80,'n'+0x80,'u'+0x80,'t'+0x80,'e'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x1f},
    //59 Second:
    {0x8C,0x40,'S'+0x80,'e'+0x80,'c'+0x80,'o'+0x80,'n'+0x80,'d'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x1f},
  };

static uint8_t about[]=
   "Kestutis Bivainis, 2021. MCU: NXP LPC812 ARM Cortex M0+. "
   "RTC chip: NXP PCA2129T. LCD: Ericsson GA628 PCF2103. "
   "EEPROM: CSI 24C32WI. Bus Protocol: I2C";

static uint8_t information[]=
  "Power Mode: xxxxxxxxx DEV-ID: xxxx Speed: xx Mhz "
  "Debug: xxxxxxxx "
  "I2C Addresses: LCD:0x74, Clock:0xA2, EEPROM:0xA0";

static uint8_t birthday_record[15];
static bool birthday = false;
static uint32_t old_day;
static BIRTHDAY birthdays[BIRTHDAYS_COUNT];
static uint8_t no_birthday[PCF2103_LCD_SEND_WIDTH]=
  {0x8C,0x40,'N'+0x80,'o'+0x80,0x20,'b'+0x80,'i'+0x80,'r'+0x80,'t'+0x80,'h'+0x80,'d'+0x80,'a'+0x80,'y'+0x80,0x20};

static uint8_t icons[PCF2103_LCD_SEND_WIDTH]=
  {0x4C,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10};

static uint8_t buffer_LCD[PCF2103_LCD_SEND_WIDTH]=
  {0x8C,0x40,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};

// settings
static DISPLAYFORMAT format, old_format;
static bool separator_flip, old_separator_flip;
static UPDATEFREQ freq, old_freq;
static CHIP_PMU_MCUPOWER_T sleep, old_sleep;
static bool scroll, old_scroll;
static BLINK blink, old_blink;
static uint32_t icons_mode, old_icons_mode;
static uint32_t aging_offset, old_aging_offset;
static bool swd, old_swd;

// current state of separator
static bool sep_flip;

static uint32_t scroll_position;
static bool scroll_direction = true;

static uint8_t menu_forward[] = {
  1, 2, 3, 4, 5, 6, 7, 0, 9,10,
 11,12,13,14,15,16,17,18, 8,20,
 21,22,23,24,25,26,27,19,29,28,
 31,30, 0,34,35,33,37,38,36,40,
 39,42,41,44,45,43, 0, 0, 0,50,
 51,52,49, 0, 0, 0, 0, 0, 0, 0
};

static uint8_t menu_backward[ ]= {
  7, 0, 1, 2, 3, 4, 5, 6,18, 8,
  9,10,11,12,13,14,15,16,17,27,
 19,20,21,22,23,24,25,26,29,28,
 31,30, 0,35,33,34,38,36,37,40,
 39,42,41,45,43,44, 0, 0, 0,52,
 49,50,51, 0, 0, 0, 0, 0, 0, 0
};

static uint8_t menu_enter[] = {
  0, 8,43, 3, 4,49, 6, 0, 8, 9,
 19,28,30,13,33,36,39,41, 1,10,
 10,10,10,10,10,10,10,10,11,11,
 12,12, 1,14,14,14,15,15,15,16,
 16,17,17, 2, 2, 2, 0, 0, 0,49,
 50,51, 5, 0, 0, 0, 0, 0, 0, 0
};

static uint8_t buf[16];
// time and date read from PCA2129T
static uint8_t time_and_date[9];

static void set_menu(uint8_t* str, uint32_t num) {

   for (uint32_t i = 0; i < PCF2103_LCD_WIDTH; i++) {
      str[i + 2] = strMenu[num][i + 2];
   }
}

static void clear_icons(void) {

  for (uint32_t i = 2; i < PCF2103_LCD_SEND_WIDTH; i++){
    icons[i] = 0x00;
  }
}

static void clear_lcd(void) {

  for (uint32_t i = 2; i < PCF2103_LCD_SEND_WIDTH; i++){
    buffer_LCD[i] = 0x20;
  }
}

static void set_aging_offset(void) {

  uint32_t displayed_offset;
  uint32_t sign;

  set_menu(buffer_LCD, 32);

  while (button_state) {
    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        if (aging_offset > PCA2129T_MIN_OFFSET) aging_offset--;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        if (aging_offset < PCA2129T_MAX_OFFSET) aging_offset++;
      }
    }
    if (aging_offset > 8) {
      displayed_offset = aging_offset - 8;
      sign = '-' + 0x80;
    }
    else if (aging_offset < 8) {
      displayed_offset = 8 - aging_offset;
      sign = '+' + 0x80;
    }
    else {
      displayed_offset = 0;
      sign = 0x20;
    }
    buffer_LCD[9] = sign;
    buffer_LCD[10] = displayed_offset + 0xb0;
    WRITE_LCD_SLEEP(buffer_LCD)
  }
  buf[0] = aging_offset;
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_AGING_OFFSET_REGISTER, PCA2129T_OFFSET_LENGHT, buf);
}

static void set_date(void) {

  uint32_t set_day, set_month, set_year, set_wday;

  read_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_DATE_REGISTER, PCA2129T_DATE_LENGHT, buf);

  set_day = FROM_BCD(buf[1]);
  set_wday = FROM_BCD(buf[2]);
  set_month = FROM_BCD(buf[3]);
  set_year = FROM_BCD(buf[4]);

  set_menu(buffer_LCD, 53);

  while (button_state) {
    if (counter > old_counter) {
      if (counter -  3 > old_counter) {
        old_counter = counter;
        if (set_year < PCA2129T_MAX_YEAR) set_year++;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        if (set_year > PCA2129T_MIN_YEAR) set_year--;
      }
    }
    buffer_LCD[7] = TENS_PLACE_DIGIT(set_year);
    buffer_LCD[8] = ONES_PLACE_DIGIT(set_year);
    WRITE_LCD_SLEEP(buffer_LCD)
  }

  set_menu(buffer_LCD, 54);

  button_state = true;

  while (button_state) {
    if (counter > old_counter) {
      if (counter  -3 > old_counter) {
        old_counter = counter;
        if (set_month < PCA2129T_MAX_MONTH) set_month++;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        if (set_month > PCA2129T_MIN_MONTH) set_month--;
      }
    }
    buffer_LCD[8] = TENS_PLACE_DIGIT(set_month);
    buffer_LCD[9] = ONES_PLACE_DIGIT(set_month);
    WRITE_LCD_SLEEP(buffer_LCD)
  }

  button_state = true;

  set_menu(buffer_LCD, 55);

  while (button_state) {
    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        if (set_day < PCA2129T_MAX_DAY) set_day++;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        if (set_day > PCA2129T_MIN_DAY) set_day--;
      }
    }
    buffer_LCD[6] = TENS_PLACE_DIGIT(set_day);
    buffer_LCD[7] = ONES_PLACE_DIGIT(set_day);
    WRITE_LCD_SLEEP(buffer_LCD)
  }

  button_state = true;

  set_menu(buffer_LCD, 56);

  while (button_state) {
    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        if (set_wday < PCA2129T_MAX_WEEKDAY) set_wday++;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        if (set_wday > PCA2129T_MIN_WEEKDAY) set_wday--;
      }
    }
    buffer_LCD[10] = set_wday + 0xb0;
    WRITE_LCD_SLEEP(buffer_LCD)
  }

  buf[0] = (uint8_t)TO_BCD(set_day);
  buf[1] = (uint8_t)TO_BCD(set_wday);
  buf[2] = (uint8_t)TO_BCD(set_month);
  buf[3] = (uint8_t)TO_BCD(set_year);
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_DATE_REGISTER, PCA2129T_DATE_LENGHT, buf);
}

static void set_time(void) {

  uint32_t set_hour, set_minute, set_second;

  read_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_TIME_REGISTER, PCA2129T_TIME_LENGHT, buf);
  // mask OSF from seconds
  buf[1] &= 0x7F;

  set_hour = FROM_BCD(buf[3]);
  set_minute = FROM_BCD(buf[2]);
  set_second = FROM_BCD(buf[1]);

  set_menu(buffer_LCD, 57);

  while (button_state) {
    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        if (set_hour < PCA2129T_MAX_HOUR) set_hour++;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        if (set_hour > PCA2129T_MIN_HOUR) set_hour--;
      }
    }
    buffer_LCD[7] = TENS_PLACE_DIGIT(set_hour);
    buffer_LCD[8] = ONES_PLACE_DIGIT(set_hour);
    WRITE_LCD_SLEEP(buffer_LCD)
  }

  button_state = true;

  set_menu(buffer_LCD, 58);

  while (button_state) {
    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        if (set_minute < PCA2129T_MAX_MINUTE) set_minute++;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        if (set_minute > PCA2129T_MIN_MINUTE) set_minute--;
      }
    }
    buffer_LCD[9] = TENS_PLACE_DIGIT(set_minute);
    buffer_LCD[10] = ONES_PLACE_DIGIT(set_minute);
    WRITE_LCD_SLEEP(buffer_LCD)
  }

  button_state = true;

  set_menu(buffer_LCD, 59);

  while (button_state) {
    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        if (set_second < PCA2129T_MAX_SECOND) set_second++;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        if (set_second > PCA2129T_MIN_SECOND) set_second--;
      }
    }
    buffer_LCD[9] = TENS_PLACE_DIGIT(set_second);
    buffer_LCD[10] = ONES_PLACE_DIGIT(set_second);
    WRITE_LCD_SLEEP(buffer_LCD)
  }

  buf[0] = (uint8_t)TO_BCD(set_second);
  buf[1] = (uint8_t)TO_BCD(set_minute);
  buf[2] = (uint8_t)TO_BCD(set_hour);
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_TIME_REGISTER, PCA2129T_TIME_LENGHT, buf);
}

static void display_clock(void) {

  uint32_t i;
  uint32_t month, day;

  switch (blink) {
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
      if ((freq == UPDATEFREQ_1_SEC) && (time_and_date[2] == 0x59) ||
          (freq == UPDATEFREQ_1_MIN) ) {
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
          switch (blink) {
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
        switch (blink) {
          case BLINK_OFF: break;
          case BLINK_ON:
          case BLINK_1MIN:
            LED2_OFF
          break;
        }
      }
      old_day = time_and_date[5];
    }

    clear_icons();
    clear_lcd();

    switch (icons_mode) {
      case ICONSMODE_POWER:
        icons[12] = time_and_date[1] & 0x04 ? 0x04 : 0x1F;
        icons[13] = 0x10;
        switch (sleep) {
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

    switch (format) {
      // HHMM_SEP
      case DISPLAYFORMAT_HHMM_SEP:
        if (scroll) {
          if (scroll_direction) {
            if (scroll_position == 7) {
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
        buffer_LCD[scroll_position + 2] = FROM_BCD_HIGH(time_and_date[4]) + 0xb0;
        buffer_LCD[scroll_position + 3] = FROM_BCD_LOW(time_and_date[4]) + 0xb0;
        buffer_LCD[scroll_position + 4] = ':' + 0x80;
        if (separator_flip) {
          if (sep_flip)
            buffer_LCD[scroll_position + 4] = '-' + 0x80;
          sep_flip = !sep_flip;
        }
        buffer_LCD[scroll_position + 5] = FROM_BCD_HIGH(time_and_date[3]) + 0xb0;
        buffer_LCD[scroll_position + 6] = FROM_BCD_LOW(time_and_date[3]) + 0xb0;
        break;

      // HHMMSS_SEP
      case DISPLAYFORMAT_HHMMSS_SEP:
        if (scroll) {
          if (scroll_direction) {
            if (scroll_position == 4) {
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
        buffer_LCD[scroll_position + 2] = FROM_BCD_HIGH(time_and_date[4]) + 0xb0;
        buffer_LCD[scroll_position + 3] = FROM_BCD_LOW(time_and_date[4]) + 0xb0;
        buffer_LCD[scroll_position + 4] = ':' + 0x80;
        buffer_LCD[scroll_position + 7] = ':' + 0x80;
        if (separator_flip) {
          if (sep_flip) {
            buffer_LCD[scroll_position + 4] = '-' + 0x80;
            buffer_LCD[scroll_position + 7] = '-' + 0x80;
          }
          sep_flip = !sep_flip;
        }
        buffer_LCD[scroll_position + 5] = FROM_BCD_HIGH(time_and_date[3]) + 0xb0;
        buffer_LCD[scroll_position + 6] = FROM_BCD_LOW(time_and_date[3]) + 0xb0;
        buffer_LCD[scroll_position + 8] = FROM_BCD_HIGH(time_and_date[2]) + 0xb0;
        buffer_LCD[scroll_position + 9] = FROM_BCD_LOW(time_and_date[2]) + 0xb0;
        break;

      // HHMMSS
      case DISPLAYFORMAT_HHMMSS:
        if (scroll) {
          if (scroll_direction) {
            if (scroll_position == 6) {
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
        buffer_LCD[scroll_position + 2] = FROM_BCD_HIGH(time_and_date[4]) + 0xb0;
        buffer_LCD[scroll_position + 3] = FROM_BCD_LOW(time_and_date[4]) + 0xb0;
        buffer_LCD[scroll_position + 4] = FROM_BCD_HIGH(time_and_date[3]) + 0xb0;
        buffer_LCD[scroll_position + 5] = FROM_BCD_LOW(time_and_date[3]) + 0xb0;
        buffer_LCD[scroll_position + 6] = FROM_BCD_HIGH(time_and_date[2]) + 0xb0;
        buffer_LCD[scroll_position + 7] = FROM_BCD_LOW(time_and_date[2]) + 0xb0;
        break;

      // HHMM
      case DISPLAYFORMAT_HHMM:
        if (scroll) {
          if (scroll_direction) {
            if (scroll_position == 8) {
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
        buffer_LCD[scroll_position + 2] = FROM_BCD_HIGH(time_and_date[4]) + 0xb0;
        buffer_LCD[scroll_position + 3] = FROM_BCD_LOW(time_and_date[4]) + 0xb0;
        buffer_LCD[scroll_position + 4] = FROM_BCD_HIGH(time_and_date[3]) + 0xb0;
        buffer_LCD[scroll_position + 5] = FROM_BCD_LOW(time_and_date[3]) + 0xb0;
        break;

      // HHMM_MIDD
      case DISPLAYFORMAT_HHMM_MIDD:
        if (scroll) {
          if (scroll_direction) {
            if (scroll_position == 1) {
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
        buffer_LCD[scroll_position + 2] = FROM_BCD_HIGH(time_and_date[4]) + 0xb0;
        buffer_LCD[scroll_position + 3] = FROM_BCD_LOW(time_and_date[4]) + 0xb0;
        buffer_LCD[scroll_position + 4] = ':' + 0x80;
        if (separator_flip) {
          if (sep_flip)
            buffer_LCD[scroll_position + 4] = '-' + 0x80;
          sep_flip = !sep_flip;
        }
        buffer_LCD[scroll_position + 5] = FROM_BCD_HIGH(time_and_date[3]) + 0xb0;
        buffer_LCD[scroll_position + 6] = FROM_BCD_LOW(time_and_date[3]) + 0xb0;
        buffer_LCD[scroll_position + 7] = 0x20;
        buffer_LCD[scroll_position + 8] = FROM_BCD_HIGH(time_and_date[7]) + 0xb0;
        buffer_LCD[scroll_position + 9] = FROM_BCD_LOW(time_and_date[7]) + 0xb0;
        buffer_LCD[scroll_position + 10] = '.' + 0x80;
        buffer_LCD[scroll_position + 11] = FROM_BCD_HIGH(time_and_date[5]) + 0xb0;
        buffer_LCD[scroll_position + 12] = FROM_BCD_LOW(time_and_date[5]) + 0xb0;
        break;

      // MIDD_HHMM
      case DISPLAYFORMAT_MIDD_HHMM:
        if (scroll) {
          if (scroll_direction) {
            if (scroll_position == 1) {
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
        buffer_LCD[scroll_position + 2] = FROM_BCD_HIGH(time_and_date[7]) + 0xb0;
        buffer_LCD[scroll_position + 3] = FROM_BCD_LOW(time_and_date[7]) + 0xb0;
        buffer_LCD[scroll_position + 4] = '.' + 0x80;
        buffer_LCD[scroll_position + 5] = FROM_BCD_HIGH(time_and_date[5]) + 0xb0;
        buffer_LCD[scroll_position + 6] = FROM_BCD_LOW(time_and_date[5]) + 0xb0;
        buffer_LCD[scroll_position + 7] = 0x20;
        buffer_LCD[scroll_position + 8] = FROM_BCD_HIGH(time_and_date[4]) + 0xb0;
        buffer_LCD[scroll_position + 9] = FROM_BCD_LOW(time_and_date[4]) + 0xb0;
        buffer_LCD[scroll_position + 10] = ':' + 0x80;
        if (separator_flip) {
          if (sep_flip)
            buffer_LCD[scroll_position + 10] = '-' + 0x80;
          sep_flip = !sep_flip;
        }
        buffer_LCD[scroll_position + 11] = FROM_BCD_HIGH(time_and_date[3]) + 0xb0;
        buffer_LCD[scroll_position + 12] = FROM_BCD_LOW(time_and_date[3]) + 0xb0;
        break;

      // HHMMSS MIDD
      case DISPLAYFORMAT_HHMMSS_MIDD:
        if (scroll) {
          if (scroll_direction) {
            if (scroll_position == 1) {
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
        buffer_LCD[scroll_position + 2] = FROM_BCD_HIGH(time_and_date[4]) + 0xb0;
        buffer_LCD[scroll_position + 3] = FROM_BCD_LOW(time_and_date[4]) + 0xb0;
        buffer_LCD[scroll_position + 4] = FROM_BCD_HIGH(time_and_date[3]) + 0xb0;
        buffer_LCD[scroll_position + 5] = FROM_BCD_LOW(time_and_date[3]) + 0xb0;
        buffer_LCD[scroll_position + 6] = FROM_BCD_HIGH(time_and_date[2]) + 0xb0;
        buffer_LCD[scroll_position + 7] = FROM_BCD_LOW(time_and_date[2]) + 0xb0;
        buffer_LCD[scroll_position + 9] = FROM_BCD_HIGH(time_and_date[7]) + 0xb0;
        buffer_LCD[scroll_position + 10] = FROM_BCD_LOW(time_and_date[7]) + 0xb0;
        buffer_LCD[scroll_position + 11] = FROM_BCD_HIGH(time_and_date[5]) + 0xb0;
        buffer_LCD[scroll_position + 12] = FROM_BCD_LOW(time_and_date[5]) + 0xb0;
        break;

      //  HHMMSSYYMIDD
      case DISPLAYFORMAT_HHMMSSYYMIDD:
        buffer_LCD[2] = FROM_BCD_HIGH(time_and_date[4]) + 0xb0;
        buffer_LCD[3] = FROM_BCD_LOW(time_and_date[4]) + 0xb0;
        buffer_LCD[4] = FROM_BCD_HIGH(time_and_date[3]) + 0xb0;
        buffer_LCD[5] = FROM_BCD_LOW(time_and_date[3]) + 0xb0;
        buffer_LCD[6] = FROM_BCD_HIGH(time_and_date[2]) + 0xb0;
        buffer_LCD[7] = FROM_BCD_LOW(time_and_date[2]) + 0xb0;
        buffer_LCD[8] = FROM_BCD_HIGH(time_and_date[8]) + 0xb0;
        buffer_LCD[9] = FROM_BCD_LOW(time_and_date[8]) + 0xb0;
        buffer_LCD[10] = FROM_BCD_HIGH(time_and_date[7]) + 0xb0;
        buffer_LCD[11] = FROM_BCD_LOW(time_and_date[7]) + 0xb0;
        buffer_LCD[12] = FROM_BCD_HIGH(time_and_date[5]) + 0xb0;
        buffer_LCD[13] = FROM_BCD_LOW(time_and_date[5]) + 0xb0;
        break;

      //  YYMIDDHHMMSS
      case DISPLAYFORMAT_YYMIDDHHMMSS:
        buffer_LCD[2] = FROM_BCD_HIGH(time_and_date[8]) + 0xb0;
        buffer_LCD[3] = FROM_BCD_LOW(time_and_date[8]) + 0xb0;
        buffer_LCD[4] = FROM_BCD_HIGH(time_and_date[7]) + 0xb0;
        buffer_LCD[5] = FROM_BCD_LOW(time_and_date[7]) + 0xb0;
        buffer_LCD[6] = FROM_BCD_HIGH(time_and_date[5]) + 0xb0;
        buffer_LCD[7] = FROM_BCD_LOW(time_and_date[5]) + 0xb0;
        buffer_LCD[8] = FROM_BCD_HIGH(time_and_date[4]) + 0xb0;
        buffer_LCD[9] = FROM_BCD_LOW(time_and_date[4]) + 0xb0;
        buffer_LCD[10] = FROM_BCD_HIGH(time_and_date[3]) + 0xb0;
        buffer_LCD[11] = FROM_BCD_LOW(time_and_date[3]) + 0xb0;
        buffer_LCD[12] = FROM_BCD_HIGH(time_and_date[2]) + 0xb0;
        buffer_LCD[13] = FROM_BCD_LOW(time_and_date[2]) + 0xb0;
        break;
    }
    WRITE_LCD(buffer_LCD)
  }

  switch (blink) {
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

static void display_info(void) {

  uint32_t i = 0, j;

  information[30] = ((LPC_SYSCTL->DEVICEID >> 12) & 0x0F) + 0x30;
  information[31] = ((LPC_SYSCTL->DEVICEID >> 8) & 0x0F) + 0x30;
  information[32] = ((LPC_SYSCTL->DEVICEID >> 4) & 0x0F) + 0x30;
  information[33] = (LPC_SYSCTL->DEVICEID & 0x0F) + 0x30;

  if (information[30] > 0x39) information[30] += 7;
  if (information[31] > 0x39) information[31] += 7;
  if (information[32] > 0x39) information[32] += 7;
  if (information[33] > 0x39) information[33] += 7;

  information[42] = (uint8_t)(((SystemCoreClock / 1000000) / 10) + 0x30);
  information[43] = (uint8_t)(((SystemCoreClock / 1000000) % 10) + 0x30);

  information[56] = format + 0x30;
  information[57] = freq + 0x30;
  information[58] = sleep + 0x30;
  information[59] = scroll + 0x30;
  information[60] = blink + 0x30;
  information[61] = icons_mode + 0x30;
  information[62] = separator_flip + 0x30;
  information[63] = aging_offset + 0x30;

  switch (sleep){
    case PMU_MCU_SLEEP:
      information[12] = 'S';
      information[13] = 'l';
      information[14] = 'e';
      information[15] = 'e';
      information[16] = 'p';
      information[17] = 0x20;
      information[18] = 0x20;
      information[19] = 0x20;
      information[20] = 0x20;
      break;
    case PMU_MCU_DEEP_SLEEP:
      information[12] = 'D';
      information[13] = 'e';
      information[14] = 'e';
      information[15] = 'p';
      information[16] = 'S';
      information[17] = 'l';
      information[18] = 'e';
      information[19] = 'e';
      information[20] = 'p';
      break;
    case PMU_MCU_POWER_DOWN:
      information[12] = 'P';
      information[13] = 'o';
      information[14] = 'w';
      information[15] = 'e';
      information[16] = 'r';
      information[17] = 'D';
      information[18] = 'o';
      information[19] = 'w';
      information[20] = 'n';
      break;
    default:
      break;
  }

  while (button_state) {
    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        if (i < 101) i++;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        if (i > 0) i--;
      }
    }
    for (j = 0; j < 12; j++) {
      buffer_LCD[j + 2] = information[i + j] + 0x80;
    }
    WRITE_LCD_SLEEP(buffer_LCD)
  }
}

static void display_font(void) {

  uint8_t i = 0;

  while (button_state) {
    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        i += 8;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        i -= 8;
      }
    }
    buffer_LCD[2] = 'h' + 0x80;
    buffer_LCD[3] = ((i >> 4) > 9 ? (i >> 4) + 7 :(i >> 4)) + 0xb0;
    buffer_LCD[4] = (i & 0x0F) + 0xb0;
    buffer_LCD[5] = ':' + 0x80;
    buffer_LCD[6] = i;
    buffer_LCD[7] = i + 1;
    buffer_LCD[8] = i + 2;
    buffer_LCD[9] = i + 3;
    buffer_LCD[10] = i + 4;
    buffer_LCD[11] = i + 5;
    buffer_LCD[12] = i + 6;
    buffer_LCD[13] = i + 7;
    WRITE_LCD_SLEEP(buffer_LCD)
  }
}

static void display_about(void) {

  uint32_t i = 0, j;

  while (button_state) {
    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        if (i < 136) i++;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        if (i > 0) i--;
      }
    }
    for (j = 0; j < 12; j++) {
      buffer_LCD[j + 2] = about[i + j] + 0x80;
    }
    WRITE_LCD_SLEEP(buffer_LCD)
  }
}

static void display_icons(void) {

  uint32_t i = 0,j = 0;
  uint8_t icon[][2] = {
    {2,0x10},{2,0x08},{2,0x04},{2,0x02},{2,0x01},
    {4,0x02},{4,0x04},{4,0x08},{4,0x10},
    {5,0x08},{5,0x10},
    {7,0x10},
    {8,0x08},{8,0x10},
    {10,0x10},
    {12,0x04},{13,0x10},{12,0x02},{12,0x08},{12,0x01},{12,0x10},
  };

  clear_icons();
  clear_lcd();
  WRITE_LCD(buffer_LCD)

  while (button_state) {
    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        if (j < 20) j++;
        icons[icon[j][0]] |= icon[j][1];
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        icons[icon[j][0]] &= ~icon[j][1];
        if (j > 0) j--;
      }
    }
    WRITE_LCD_SLEEP(icons)
  }

  clear_icons();
  WRITE_LCD(icons)
}

static void display_leds(void) {

  uint32_t stateLED;

  clear_lcd();
  WRITE_LCD(buffer_LCD)

  while (button_state) {

    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        stateLED = Chip_GPIO_GetPinState(LPC_GPIO_PORT, 0, PIN_LED1) |
                   (Chip_GPIO_GetPinState(LPC_GPIO_PORT, 0, PIN_LED2) << 1);
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        stateLED = Chip_GPIO_GetPinState(LPC_GPIO_PORT, 0, PIN_LED1) |
                   (Chip_GPIO_GetPinState(LPC_GPIO_PORT, 0, PIN_LED2) << 1);
      }
    }

    switch (stateLED) {
      case 0: LED1_ON
              break;
      case 1: LED1_OFF
              LED2_ON
              break;
      case 2: LED1_ON
              break;
      case 3: LED1_OFF
              LED2_OFF
              break;
    }

    SLEEP
  }
  LED1_OFF
  LED2_OFF
}

static void load_settings(void) {

  read_EEPROM(_24C32WI_I2C_ADDR_7BIT, _24C32WI_SETTINGS_ADDRESS, _24C32WI_SETTINGS_LENGTH, buf);

  if ((buf[1] == 0) || (buf[1] > 8)) buf[1] = 1;
  if ((buf[2] == 0) || (buf[2] > 2)) buf[2] = 1;
  if (buf[3] > 2) buf[3] = 0;
  if (buf[4] > 1) buf[4] = 0;
  if (buf[5] > 2) buf[5] = 0;
  if (buf[6] > 2) buf[6] = 0;
  if (buf[7] > 1) buf[7] = 0;
  if (buf[8] > 15) buf[8] = 8;

  format = buf[1];
  freq = buf[2];
  sleep = (CHIP_PMU_MCUPOWER_T)buf[3];
  scroll = buf[4];
  blink = buf[5];
  icons_mode = buf[6];
  separator_flip = buf[7];
  aging_offset = buf[8];
}

static void save_settings(void) {

  if ((old_scroll != scroll) ||
      (old_format != format) ||
      (old_sleep != sleep) ||
      (old_freq != freq) ||
      (old_blink != blink) ||
      (old_icons_mode != icons_mode) ||
      (old_separator_flip != separator_flip) ||
      (old_aging_offset != aging_offset) ) {

    buf[0] = (uint8_t)format;
    buf[1] = (uint8_t)freq;
    buf[2] = (uint8_t)sleep;
    buf[3] = scroll;
    buf[4] = (uint8_t)blink;
    buf[5] = icons_mode;
    buf[6] = separator_flip;
    buf[7] = aging_offset;

    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_EEPROM_WP, false);
    write_EEPROM(_24C32WI_I2C_ADDR_7BIT, _24C32WI_SETTINGS_ADDRESS, _24C32WI_SETTINGS_LENGTH, buf);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_EEPROM_WP, true);
  }
}

static void load_birthdays(void) {

  uint32_t i, j;

  for (i = 0; i < BIRTHDAYS_COUNT; i++) {
    read_EEPROM(_24C32WI_I2C_ADDR_7BIT, _24C32WI_BIRTHDAYS_ADDRESS + _24C32WI_BIRTHDAY_LENGTH * i, BIRTHDAY_RECORD_LENGTH + 1, birthday_record);
    birthdays[i].date[0] = birthday_record[1];
    birthdays[i].date[1] = birthday_record[2];
    for (j = 0; j < BIRTHDAY_NAME_LENGTH; j++ ) {
      birthdays[i].name[j]=birthday_record[j + 3];
    }
  }
}

static void edit_birthdays(void) {

  uint32_t i;
  uint32_t birthday_nr = 0;
  uint32_t month,day;
  uint32_t current_position;

  set_menu(buffer_LCD, 46);

  while (button_state) {
    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        if (birthday_nr < BIRTHDAYS_COUNT - 1) birthday_nr++;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        if (birthday_nr > 0) birthday_nr--;
      }
    }
    buffer_LCD[2] = (birthday_nr /10) + 0xb0;
    buffer_LCD[3] = (birthday_nr % 10) + 0xb0;

    if (birthdays[birthday_nr].date[0]) {
      buffer_LCD[5] = FROM_BCD_HIGH(birthdays[birthday_nr].date[0]) + 0xb0;
      buffer_LCD[6] = FROM_BCD_LOW(birthdays[birthday_nr].date[0]) + 0xb0;
      buffer_LCD[7] = FROM_BCD_HIGH(birthdays[birthday_nr].date[1]) + 0xb0;
      buffer_LCD[8] = FROM_BCD_LOW(birthdays[birthday_nr].date[1]) + 0xb0;
      buffer_LCD[9] = birthdays[birthday_nr].name[0] ? birthdays[birthday_nr].name[0] : 0x20;
      buffer_LCD[10] = birthdays[birthday_nr].name[1] ? birthdays[birthday_nr].name[1] : 0x20;
      buffer_LCD[11] = birthdays[birthday_nr].name[2] ? birthdays[birthday_nr].name[2] : 0x20;
      buffer_LCD[12] = birthdays[birthday_nr].name[3] ? birthdays[birthday_nr].name[3] : 0x20;
    }
    else {
      buffer_LCD[5] = 0x20;
      buffer_LCD[6] = 0x20;
      buffer_LCD[7] = 0x20;
      buffer_LCD[8] = 0x20;
      buffer_LCD[9] = 0x20;
      buffer_LCD[10] = 0x20;
      buffer_LCD[11] = 0x20;
      buffer_LCD[12] = 0x20;
    }
    WRITE_LCD_SLEEP(buffer_LCD)
  }
  button_state = true;

  month = FROM_BCD(birthdays[birthday_nr].date[0]);

  set_menu(buffer_LCD, 47);

  while (button_state) {
    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        if (month < PCA2129T_MAX_MONTH) month++;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        if (month > 0) month--;
      }
    }
    buffer_LCD[8] = (month / 10) + 0xb0;
    buffer_LCD[9] = (month % 10) + 0xb0;
    WRITE_LCD_SLEEP(buffer_LCD)
  }
  button_state = true;

  day = FROM_BCD(birthdays[birthday_nr].date[1]);

  set_menu(buffer_LCD, 48);

  while (button_state) {
    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        if (day < PCA2129T_MAX_DAY) day++;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        if (day > 0) day--;
      }
    }
    buffer_LCD[6] = (day / 10) + 0xb0;
    buffer_LCD[7] = (day % 10) + 0xb0;
    WRITE_LCD_SLEEP(buffer_LCD)
  }

  button_state = true;

  if ((month == 0) && (day == 0)) {
    // clear birthday
    for (i = 0; i < BIRTHDAY_RECORD_LENGTH; i++) {
      birthday_record[i] = 0;
    }
    birthdays[birthday_nr].date[0] = 0;
    birthdays[birthday_nr].date[1] = 0;
    for (i = 0; i < BIRTHDAY_NAME_LENGTH; i++) {
      birthdays[birthday_nr].name[i] = 0;
    }
  }
  else {
    // update birthday
    for (i = 0; i < BIRTHDAY_NAME_LENGTH; i++) {
      birthdays[birthday_nr].name[i] = birthdays[birthday_nr].name[i] ? birthdays[birthday_nr].name[i] : 0xA0;
    }

    current_position = 0;

    while (current_position < BIRTHDAY_NAME_LENGTH) {
      while (button_state) {
        if (counter > old_counter) {
          if (counter - 3 > old_counter) {
            old_counter = counter;
             birthdays[birthday_nr].name[current_position]++;
          }
        }
        else if (counter < old_counter) {
          if (counter + 3 < old_counter) {
            old_counter = counter;
            birthdays[birthday_nr].name[current_position]--;
          }
        }
        for (i = 0; i < PCF2103_LCD_WIDTH; i++) {
          buffer_LCD[i + 2] = birthdays[birthday_nr].name[i];
        }
        WRITE_LCD_SLEEP(buffer_LCD)
      }
      button_state = true;
      current_position++;
    }
    // save birthday
    birthday_record[0] = (uint8_t)TO_BCD(month);
    birthday_record[1] = (uint8_t)TO_BCD(day);
    birthdays[birthday_nr].date[0] = birthday_record[0];
    birthdays[birthday_nr].date[1] = birthday_record[1];
    for (i = 0; i < BIRTHDAY_NAME_LENGTH; i++) {
      birthday_record[i + 2] = birthdays[birthday_nr].name[i];
    }
  }

  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_EEPROM_WP, false);
  write_EEPROM(_24C32WI_I2C_ADDR_7BIT, _24C32WI_BIRTHDAYS_ADDRESS + 32 * birthday_nr, BIRTHDAY_RECORD_LENGTH, birthday_record);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_EEPROM_WP, true);

  SLEEP
}

static void display_birthdays(void) {

  uint32_t i, j, birthday_nr = 1, birthday_count;
  uint32_t month, day;

  read_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_DATE_REGISTER, PCA2129T_DATE_LENGHT, buf);

  month = buf[3];
  day = buf[1];

  while (button_state) {
    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        birthday_nr++;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        if (birthday_nr > 1)
          birthday_nr--;
      }
    }
    birthday_count = 0;

    for (i = 0; i < BIRTHDAYS_COUNT; i++ ) {
      if ((birthdays[i].date[0] == month) && (birthdays[i].date[1] == day)) {
        birthday_count++;
        if (birthday_count == birthday_nr) {
          for (j = 0; j < PCF2103_LCD_WIDTH; j++) {
            buffer_LCD[j + 2] = birthdays[i].name[j] ? birthdays[i].name[j] : 0x20;
          }
          WRITE_LCD_SLEEP(buffer_LCD)
          break;
        }
      }
    }
    if (i == BIRTHDAYS_COUNT) {
      birthday_nr--;
    }
    if (birthday_count == 0) {
      WRITE_LCD_SLEEP(no_birthday)
    }
  }
}

int main(void) {

  uint32_t idle = 0;

  // Read clock settings and update SystemCoreClock variable
  SystemCoreClockUpdate();

  // startup delay 0.5s
  Chip_MRT_SetInterval(LPC_MRT_CH1,(500000*12-3));

  init_pins();
  configure_SWD(false);

  /* Allocate I2C handle, setup I2C rate, and initialize I2C
     clocking */
  setup_I2C_master();

/*
    //22:21:40 23 friday 04 2021
    buf[0]=0x40;
    buf[1]=0x21;
    buf[2]=0x22;
    buf[3]=0x23;
    buf[4]=0x05;
    buf[5]=0x04;
    buf[6]=0x21;
    writeClock(PCA2129T_I2C_ADDR_7BIT,0x03,7,buf);
*/

  // init display
  buf[0] = 0x31;//Function set -> extended
  buf[1] = 0x02;//Screen configuration
  buf[2] = 0x05;//Display configuration
  buf[3] = 0x08;//Icon control
  buf[4] = 0x30;//Function set -> normal
  buf[5] = 0x0C;//Display control
  buf[6] = 0x06;//Entry mode set
  buf[7] = 0x02;//Return home
  write_LCD(PCF2103_I2C_ADDR_7BIT, 0x00, 8, buf);

  // clear icons
  clear_icons();
  WRITE_LCD(icons)

  load_settings();

  //write register 0x0f (CLKOUT disabled)
  buf[0] = 0x07;
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_CLKOUT_CONTROL_REGISTER, PCA2129T_CLKOUT_CONTROL_LENGHT, buf);

  //write register 0x10 ( configure pulse interrupt active low )
  buf[0] = 0x23;
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_WATHCDOG_TIMER_CONTROL_REGISTER, PCA2129T_WATHCDOG_TIMER_CONTROL_LENGHT, buf);

  //write register 0 ( set seconds interrupt=1, minutes=2)
  buf[0] = (uint8_t)freq;
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_CONTROL1_REGISTER, PCA2129T_CONTROL1_LENGTH, buf);

  buf[0] = 0x00;
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_CONTROL2_REGISTER, PCA2129T_CONTROL2_LENGTH, buf);

  buf[0] = 0x00;
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_CONTROL3_REGISTER, PCA2129T_CONTROL3_LENGTH, buf);

  //timestamp disable
  buf[0] = 0x40;
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_TIMESTAMP_CONTROL_REGISTER, PCA2129T_TIMESTAMP_CONTROL_LENGTH, buf);

  //write register 0x19 ( aging offset )
  buf[0] = aging_offset;
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_AGING_OFFSET_REGISTER, PCA2129T_OFFSET_LENGHT, buf);

  Chip_SYSCTL_EnablePINTWakeup(1);
  Chip_SYSCTL_EnablePINTWakeup(2);
  Chip_SYSCTL_EnablePINTWakeup(3);
  Chip_SYSCTL_EnablePINTWakeup(4);

  Chip_SYSCTL_SetDeepSleepPD(SYSCTL_DEEPSLP_BOD_PD | SYSCTL_DEEPSLP_WDTOSC_PD);
  Chip_SYSCTL_SetWakeup(~(SYSCTL_SLPWAKE_IRCOUT_PD | SYSCTL_SLPWAKE_IRC_PD | SYSCTL_SLPWAKE_FLASH_PD));
  Chip_PMU_DisableDeepPowerDown(LPC_PMU);

  old_counter = counter;

  load_birthdays();

  while (1) {

    SLEEP

    if (clock_mode) {
      // clock mode
      if (!button_state) {
        // entering settings if button is pressed
        // clear icons
        clear_icons();
        WRITE_LCD(icons)
        menu_position = 0;
        WRITE_LCD(strMenu[menu_position])
        clock_mode = false;
        button_state = true;
        old_counter = counter;
        idle = 0;
        old_format = format;
        old_freq = freq;
        old_sleep = sleep;
        old_scroll = scroll;
        old_blink = blink;
        old_icons_mode = icons_mode;
        old_separator_flip = separator_flip;
        old_aging_offset = aging_offset;
        buf[0] = UPDATEFREQ_1_SEC;
        write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_CONTROL1_REGISTER, PCA2129T_CONTROL1_LENGTH, buf);
      }
      else {
        display_clock();
      }
    }
    else {
      // settings mode
      if (idle++ > 60) {
        button_state = false;
        menu_position = 7;
      }

      if (!button_state) {
        idle = 0;
        button_state = true;
        switch (menu_position) {
          case 0: display_birthdays(); break;
          case 3: display_info(); break;
          case 4: edit_birthdays(); break;
          case 6: display_about(); break;
          case 8: set_date(); break;
          case 9: set_time(); break;
          case 13: set_aging_offset(); break;
          case 19: format = DISPLAYFORMAT_HHMMSS_SEP; scroll_position = 0; break;
          case 20: format = DISPLAYFORMAT_HHMM_SEP; scroll_position = 0; break;
          case 21: format = DISPLAYFORMAT_HHMMSS; scroll_position = 0; break;
          case 22: format = DISPLAYFORMAT_HHMM; scroll_position = 0; break;
          case 23: format = DISPLAYFORMAT_HHMM_MIDD; scroll_position = 0; break;
          case 24: format = DISPLAYFORMAT_MIDD_HHMM; scroll_position = 0; break;
          case 25: format = DISPLAYFORMAT_HHMMSS_MIDD; scroll_position = 0; break;
          case 26: format = DISPLAYFORMAT_HHMMSSYYMIDD; scroll_position = 0; break;
          case 27: format = DISPLAYFORMAT_YYMIDDHHMMSS; scroll_position = 0; break;
          case 28: freq = UPDATEFREQ_1_SEC; break;
          case 29: freq = UPDATEFREQ_1_MIN; break;
          case 30: scroll = true; break;
          case 31: scroll = false; scroll_position = 0; break;
          case 33: blink = BLINK_ON; break;
          case 34: blink = BLINK_OFF; break;
          case 35: blink = BLINK_1MIN; break;
          case 36: icons_mode = ICONSMODE_POWER; break;
          case 37: icons_mode = ICONSMODE_WEEKDAYS; break;
          case 38: icons_mode = ICONSMODE_NONE; break;
          case 39: separator_flip = true; break;
          case 40: separator_flip = false; break;
          case 41: configure_SWD(false); break;
          case 42: configure_SWD(true); break;
          case 43: sleep = PMU_MCU_POWER_DOWN; break;
          case 44: sleep = PMU_MCU_DEEP_SLEEP; break;
          case 45: sleep = PMU_MCU_SLEEP; break;
          case 49: display_font(); break;
          case 50: display_icons(); break;
          case 51: display_leds(); break;
          default: break;
        }
        button_state = true;
        if (menu_position == 7) { // exit settings
          clock_mode = true;
          if (old_freq != freq) {
            buf[0] = freq;
            write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_CONTROL1_REGISTER, PCA2129T_CONTROL1_LENGTH, buf);
          }
          save_settings();
          clock_interrupt = true;
          display_clock();
        }
        else {
          menu_position = menu_enter[menu_position];
          switch (menu_position) {
            case 19: menu_position += format; break;
            case 28:
              switch (freq) {
                case UPDATEFREQ_1_SEC: break;
                case UPDATEFREQ_1_MIN: menu_position = 29; break;
                default: break;
              }; break;
            case 30: if (!scroll) menu_position = 31; break;
            case 33: menu_position += blink; break;
            case 36: menu_position += icons_mode; break;
            case 39: if (!separator_flip) menu_position = 40; break;
            case 43:
              switch (sleep) {
                case PMU_MCU_POWER_DOWN: break;
                case PMU_MCU_DEEP_SLEEP: menu_position = 44; break;
                case PMU_MCU_SLEEP: menu_position = 45; break;
                default: break;
              }; break;
            default: break;
          }
          WRITE_LCD(strMenu[menu_position])
        }
      } else if (counter > old_counter) {
        if (counter - 3 > old_counter) {
          idle = 0;
          menu_position = menu_forward[menu_position];
          WRITE_LCD(strMenu[menu_position])
          old_counter = counter;
        }
      }
      else if (counter < old_counter) {
        if (counter + 3 < old_counter) {
          idle = 0;
          menu_position = menu_backward[menu_position];
          WRITE_LCD(strMenu[menu_position])
          old_counter = counter;
        }
      }
    }
  }
}

/*
 * clock.c
 *
 *      Author: Kestutis Bivainis
 */

#include "chip.h"
#include "system_LPC812.h"
#include "PCA2129T.h"
#include "i2c.h"
#include "PCF2103.h"
#include "24C32WI.h"
#include "pins.h"
#include "clock.h"

/* System oscillator rate and clock rate on the CLKIN pin */
const uint32_t OscRateIn = CLK_IRC_12MHZ;
const uint32_t ExtRateIn = EXT_CLK_IN;

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
    //10 Adj. hour
    {0x8C,0x40,'A'+0x80,'d'+0x80,'j'+0x80,'.'+0x80,0x20,'h'+0x80,'o'+0x80,'u'+0x80,'r'+0x80,0x20,0x20,0x77},
    //11 Set format
    {0x8C,0x40,'S'+0x80,'e'+0x80,'t'+0x80,0x20,'f'+0x80,'o'+0x80,'r'+0x80,'m'+0x80,'a'+0x80,'t'+0x80,0x20,0x77},
    //12 Update freq
    {0x8C,0x40,'U'+0x80,'p'+0x80,'d'+0x80,'a'+0x80,'t'+0x80,'e'+0x80,0x20,'f'+0x80,'r'+0x80,'e'+0x80,'q'+0x80,0x77},
    //13 Set scroll
    {0x8C,0x40,'S'+0x80,'e'+0x80,'t'+0x80,0x20,'s'+0x80,'c'+0x80,'r'+0x80,'o'+0x80,'l'+0x80,'l'+0x80,0x20,0x77},
    //14 ppm offset
    {0x8C,0x40,'p'+0x80,'p'+0x80,'m'+0x80,0x20,'o'+0x80,'f'+0x80,'f'+0x80,'s'+0x80,'e'+0x80,'t'+0x80,0x20,0x77},
    //15 LED blink
    {0x8C,0x40,'L'+0x80,'E'+0x80,'D'+0x80,0x20,'b'+0x80,'l'+0x80,'i'+0x80,'n'+0x80,'k'+0x80,0x20,0x20,0x77},
    //16 Icons mode
    {0x8C,0x40,'I'+0x80,'c'+0x80,'o'+0x80,'n'+0x80,'s'+0x80,0x20,'m'+0x80,'o'+0x80,'d'+0x80,'e'+0x80,0x20,0x77},
    //17 Sep. flip
    {0x8C,0x40,'S'+0x80,'e'+0x80,'p'+0x80,'.'+0x80,0x20,'f'+0x80,'l'+0x80,'i'+0x80,'p'+0x80,0x20,0x20,0x77},
    //18 SWD Port
    {0x8C,0x40,'S'+0x80,'W'+0x80,'D'+0x80,0x20,'P'+0x80,'o'+0x80,'r'+0x80,'t'+0x80,0x20,0x20,0x20,0x77},
    //19 Back
    {0x8C,0x40,'B'+0x80,'a'+0x80,'c'+0x80,'k'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x77},

    //20 1 sec
    {0x8C,0x40,'1'+0x80,0x20,'s'+0x80,'e'+0x80,'c'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //21 1 min
    {0x8C,0x40,'1'+0x80,0x20,'m'+0x80,'i'+0x80,'n'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},

    //22 On scroll
    {0x8C,0x40,'O'+0x80,'n'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //23 Off scroll
    {0x8C,0x40,'O'+0x80,'f'+0x80,'f'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},

    //24 ppm offset:
    {0x8C,0x40,'O'+0x80,'f'+0x80,'f'+0x80,'s'+0x80,'e'+0x80,'t'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20},

    //25 On blink
    {0x8C,0x40,'O'+0x80,'n'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //26 Off blink
    {0x8C,0x40,'O'+0x80,'f'+0x80,'f'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //27 Every minute
    {0x8C,0x40,'E'+0x80,'v'+0x80,'e'+0x80,'r'+0x80,'y'+0x80,0x20,'m'+0x80,'i'+0x80,'n'+0x80,0x20,0x20,0x1f},

    //28 Show Power
    {0x8C,0x40,'S'+0x80,'h'+0x80,'o'+0x80,'w'+0x80,0x20,'p'+0x80,'o'+0x80,'w'+0x80,'e'+0x80,'r'+0x80,0x20,0x1f},
    //29 Weekdays
    {0x8C,0x40,'W'+0x80,'e'+0x80,'e'+0x80,'k'+0x80,'d'+0x80,'a'+0x80,'y'+0x80,'s'+0x80,0x20,0x20,0x20,0x1f},
    //30 None
    {0x8C,0x40,'N'+0x80,'o'+0x80,'n'+0x80,'e'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},

    //31 On Sep. flip
    {0x8C,0x40,'O'+0x80,'n'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //32 Off Sep. flip
    {0x8C,0x40,'O'+0x80,'f'+0x80,'f'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},

    //33 Off SWD Port
    {0x8C,0x40,'O'+0x80,'f'+0x80,'f'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //34 On SWD Port
    {0x8C,0x40,'O'+0x80,'n'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},

    //35 Power-down
    {0x8C,0x40,'P'+0x80,'o'+0x80,'w'+0x80,'e'+0x80,'r'+0x80,'-'+0x80,'d'+0x80,'o'+0x80,'w'+0x80,'n'+0x80,0x20,0x1f},
    //36 Deep-sleep
    {0x8C,0x40,'D'+0x80,'e'+0x80,'e'+0x80,'p'+0x80,'-'+0x80,'s'+0x80,'l'+0x80,'e'+0x80,'e'+0x80,'p'+0x80,0x20,0x1f},
    //37 Sleep
    {0x8C,0x40,'S'+0x80,'l'+0x80,'e'+0x80,'e'+0x80,'p'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},

    //38 Birthday nr:
    {0x8C,0x40,0x20,0x20,':'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //39 Month:
    {0x8C,0x40,'M'+0x80,'o'+0x80,'n'+0x80,'t'+0x80,'h'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20,0x1f},
    //40 Day:
    {0x8C,0x40,'D'+0x80,'a'+0x80,'y'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},

    //41 Font table
    {0x8C,0x40,'F'+0x80,'o'+0x80,'n'+0x80,'t'+0x80,0x20,'t'+0x80,'a'+0x80,'b'+0x80,'l'+0x80,'e'+0x80,0x20,0x77},
    //42 Icons
    {0x8C,0x40,'I'+0x80,'c'+0x80,'o'+0x80,'n'+0x80,'s'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x77},
    //43 LEDs
    {0x8C,0x40,'L'+0x80,'E'+0x80,'D'+0x80,'s'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x77},
    //44 Back
    {0x8C,0x40,'B'+0x80,'a'+0x80,'c'+0x80,'k'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x77},

    //45 Year:
    {0x8C,0x40,'Y'+0x80,'e'+0x80,'a'+0x80,'r'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //46 Month:
    {0x8C,0x40,'M'+0x80,'o'+0x80,'n'+0x80,'t'+0x80,'h'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20,0x1f},
    //47 Day:
    {0x8C,0x40,'D'+0x80,'a'+0x80,'y'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //48 Weekday:
    {0x8C,0x40,'W'+0x80,'e'+0x80,'e'+0x80,'k'+0x80,'d'+0x80,'a'+0x80,'y'+0x80,':'+0x80,0x20,0x20,0x20,0x1f},
    //49 Hour:
    {0x8C,0x40,'H'+0x80,'o'+0x80,'u'+0x80,'r'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //50 Minute:
    {0x8C,0x40,'M'+0x80,'i'+0x80,'n'+0x80,'u'+0x80,'t'+0x80,'e'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x1f},
    //51 Second:
    {0x8C,0x40,'S'+0x80,'e'+0x80,'c'+0x80,'o'+0x80,'n'+0x80,'d'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x1f},

    //52 -1
    {0x8C,0x40,'-'+0x80,'1'+0x80,0x20,'H'+0x80,'o'+0x80,'u'+0x80,'r'+0x80,0x20,0x20,0x20,0x20,0x1f},
    //53 +1
    {0x8C,0x40,'+'+0x80,'1'+0x80,0x20,'H'+0x80,'o'+0x80,'u'+0x80,'r'+0x80,0x20,0x20,0x20,0x20,0x1f},
    //54 Back
    {0x8C,0x40,'B'+0x80,'a'+0x80,'c'+0x80,'k'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x77}
  };

static uint8_t about[]=
   "Kestutis Bivainis, 2021. MCU: NXP LPC812 ARM Cortex M0+. "
   "RTC chip: NXP PCA2129T. LCD: Ericsson GA628 PCF2103. "
   "EEPROM: CSI 24C32WI. Bus Protocol: I2C";

static uint8_t information[]=
  "Power Mode: xxxxxxxxx DEV-ID: xxxx Speed: xx Mhz "
  "Debug: xxxxxxxxxxxxxxxxxxxx";

static uint8_t birthday_record[BIRTHDAY_RECORD_LENGTH];
BIRTHDAY birthdays[BIRTHDAYS_COUNT];
static uint8_t no_birthday[PCF2103_LCD_SEND_WIDTH]=
  {0x8C,0x40,'N'+0x80,'o'+0x80,0x20,'b'+0x80,'i'+0x80,'r'+0x80,'t'+0x80,'h'+0x80,'d'+0x80,'a'+0x80,'y'+0x80,0x20};

static uint8_t icons[PCF2103_LCD_SEND_WIDTH]=
  {0x4C,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10};

static uint8_t buffer_LCD[PCF2103_LCD_SEND_WIDTH]=
  {0x8C,0x40,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};

typedef struct {
  uint8_t c;
  uint8_t size;
} FORMAT_CHAR;

static FORMAT_CHAR format_chars[]=
  {{0x3D    ,1}, // space (separator)
   {'H'+0x80,2}, // hour
   {'M'+0x80,2}, // minute
   {'S'+0x80,2}, // second
   {'D'+0x80,2}, // day
   {'O'+0x80,2}, // month number
   {'N'+0x80,3}, // month 3 letters
   {'Y'+0x80,2}, // year
   {':'+0x80,1}, // : or - (separator)
   {'.'+0x80,1}};// . (separator)


static uint32_t format_chars_array_size = sizeof(format_chars) / sizeof(format_chars[0]);

SETTINGS settings, old_settings;
uint32_t max_scroll_position;

// current state of separator
bool sep_flip;

uint32_t scroll_position;
bool scroll_direction = true;

static uint8_t menu_forward[] = {
  1, 2, 3, 4, 5, 6, 7, 0, 9,10,
 11,12,13,14,15,16,17,18,19, 8,
 21,20,23,22, 0,26,27,25,29,30,
 28,32,31,34,33,36,37,35, 0, 0,
  0,42,43,44,41, 0, 0, 0, 0, 0,
  0, 0,53,54,52
};

static uint8_t menu_backward[ ]= {
  7, 0, 1, 2, 3, 4, 5, 6,19, 8,
  9,10,11,12,13,14,15,16,17,18,
 21,20,23,22, 0,27,25,26,30,28,
 29,32,31,34,33,37,35,36, 0, 0,
  0,44,41,42,43, 0, 0, 0, 0, 0,
  0, 0,54,52,53
};

static uint8_t menu_enter[] = {
  0, 8,35, 3, 4,41, 6, 0, 8, 9,
 52,11,20,22,14,25,28,31,33, 1,
 12,12,13,13, 1,15,15,15,16,16,
 16,17,17,18,18, 2, 2, 2, 0, 0,
  0,41,42,43, 5, 0, 0, 0, 0, 0,
  0, 0,10,10,10
};

extern void display_clock(void);

static uint32_t idle_count;
static uint32_t idle_threshhold;

static void write_LCD_sleep(uint8_t *str) {

   WRITE_LCD(str)
   idle_count = 0;
   do {
     clock_interrupt = 0;
     SLEEP
     if (clock_interrupt) idle_count++;
     if (idle_count > idle_threshhold) break;
   }
   while (clock_interrupt);
}

static uint8_t buf[16];

static void set_menu(uint8_t* str, uint32_t num) {

   for (uint32_t i = 0; i < PCF2103_LCD_WIDTH; i++) {
      str[i + 2] = strMenu[num][i + 2];
   }
}

void clear_icons(void) {

  for (uint32_t i = 2; i < PCF2103_LCD_SEND_WIDTH; i++){
    icons[i] = 0x00;
  }
}

void clear_lcd(void) {

  for (uint32_t i = 2; i < PCF2103_LCD_SEND_WIDTH; i++){
    buffer_LCD[i] = 0x20;
  }
}

static void set_aging_offset(void) {

  uint32_t displayed_offset;
  uint32_t sign;

  set_menu(buffer_LCD, 24);

  while (button_state) {
    if (counter > old_counter) {
      if (counter - 3 > old_counter) {
        old_counter = counter;
        if (settings.s.aging_offset > PCA2129T_MIN_OFFSET) settings.s.aging_offset--;
      }
    }
    else if (counter < old_counter) {
      if (counter + 3 < old_counter) {
        old_counter = counter;
        if (settings.s.aging_offset < PCA2129T_MAX_OFFSET) settings.s.aging_offset++;
      }
    }
    if (settings.s.aging_offset > 8) {
      displayed_offset = settings.s.aging_offset - 8;
      sign = '-' + 0x80;
    }
    else if (settings.s.aging_offset < 8) {
      displayed_offset = 8 - settings.s.aging_offset;
      sign = '+' + 0x80;
    }
    else {
      displayed_offset = 0;
      sign = 0x20;
    }
    buffer_LCD[9] = sign;
    buffer_LCD[10] = displayed_offset + 0xb0;
    write_LCD_sleep(buffer_LCD);
    if (idle_count > idle_threshhold) return;
  }
  buf[0] = settings.s.aging_offset;
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_AGING_OFFSET_REGISTER, PCA2129T_OFFSET_LENGHT, buf);
}

static void set_date(void) {

  uint32_t set_day, set_month, set_year, set_wday;

  read_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_DATE_REGISTER, PCA2129T_DATE_LENGHT, buf);

  set_day = FROM_BCD(buf[1]);
  set_wday = FROM_BCD(buf[2]);
  set_month = FROM_BCD(buf[3]);
  set_year = FROM_BCD(buf[4]);

  set_menu(buffer_LCD, 45);

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
    write_LCD_sleep(buffer_LCD);
    if (idle_count > idle_threshhold) return;
  }

  set_menu(buffer_LCD, 46);

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
    write_LCD_sleep(buffer_LCD);
    if (idle_count> idle_threshhold) return;
  }

  button_state = true;

  set_menu(buffer_LCD, 47);

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
    write_LCD_sleep(buffer_LCD);
    if (idle_count > idle_threshhold) return;
  }

  button_state = true;

  set_menu(buffer_LCD, 48);

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
    write_LCD_sleep(buffer_LCD);
    if (idle_count > idle_threshhold) return;
  }

  buf[0] = (uint8_t)TO_BCD(set_day);
  buf[1] = (uint8_t)TO_BCD(set_wday);
  buf[2] = (uint8_t)TO_BCD(set_month);
  buf[3] = (uint8_t)TO_BCD(set_year);
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_DATE_REGISTER, PCA2129T_DATE_LENGHT, buf);
}

static void set_time(void) {

  uint32_t set_hour, set_minute, set_second;

  read_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_TIME_REGISTER, PCA2129T_TIME_LENGTH, buf);
  // mask OSF from seconds
  buf[1] &= 0x7F;

  set_hour = FROM_BCD(buf[3]);
  set_minute = FROM_BCD(buf[2]);
  set_second = FROM_BCD(buf[1]);

  set_menu(buffer_LCD, 49);

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
    write_LCD_sleep(buffer_LCD);
    if (idle_count > idle_threshhold) return;
  }

  button_state = true;

  set_menu(buffer_LCD, 50);

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
    write_LCD_sleep(buffer_LCD);
    if (idle_count > idle_threshhold) return;
  }

  button_state = true;

  set_menu(buffer_LCD, 51);

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
    write_LCD_sleep(buffer_LCD);
    if (idle_count > idle_threshhold) return;
  }

  buf[0] = (uint8_t)TO_BCD(set_second);
  buf[1] = (uint8_t)TO_BCD(set_minute);
  buf[2] = (uint8_t)TO_BCD(set_hour);
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_TIME_REGISTER, PCA2129T_TIME_LENGTH, buf);
}

static void updateHour(int32_t val) {

  uint32_t set_hour;

  read_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_TIME_REGISTER, PCA2129T_TIME_LENGTH, buf);
  // mask OSF from seconds
  buf[1] &= 0x7F;

  set_hour = FROM_BCD(buf[3]);

  if (val == 1) {
    if (set_hour < 23)
      set_hour++;
  }
  else {
    if (set_hour > 0)
      set_hour--;
  }

  buf[0] = buf[1];
  buf[1] = buf[2];
  buf[2] = (uint8_t)TO_BCD(set_hour);

  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_TIME_REGISTER, PCA2129T_TIME_LENGTH, buf);

}

static void update_max_scroll_position(void) {

  max_scroll_position = 0;
  for (int32_t i = 11; i >= 0; i--) {
    if (settings.s.format[i] == ' ')
      max_scroll_position = 12 - i;
    else break;
  }
  scroll_position = max_scroll_position;
}

static void set_format(void) {

  uint32_t i, j, current_position_lcd, current_position_format, format_char_size;

  clear_lcd();
  format_char_size = 1;
  buffer_LCD[2] = 0x3D;
  WRITE_LCD(buffer_LCD)

  current_position_lcd = 0;
  current_position_format = 0;

  while (current_position_lcd < 12) {
    while (button_state) {
      if (counter > old_counter) {
        if (counter - 3 > old_counter) {
          old_counter = counter;

          do {
            current_position_format++;
            if (current_position_format >= format_chars_array_size)
              current_position_format = 0;
            format_char_size = format_chars[current_position_format].size;
          }  while (current_position_lcd + format_char_size > 12 );

          for (i = current_position_lcd; i < current_position_lcd + format_char_size; i++) {
            buffer_LCD[2 + i] = format_chars[current_position_format].c;
          }
          for (j = i; j < 12; j++) {
            buffer_LCD[2 + j] = ' ' + 0x80;
          }
        }
      }
      else if (counter < old_counter) {
        if (counter + 3 < old_counter) {
          old_counter = counter;

          do {
            if (current_position_format == 0)
              current_position_format = format_chars_array_size;
            current_position_format--;
            format_char_size = format_chars[current_position_format].size;
          }  while (current_position_lcd + format_char_size > 12 );

          for (i = current_position_lcd; i < current_position_lcd + format_char_size; i++) {
             buffer_LCD[2 + i] = format_chars[current_position_format].c;
          }
          for (j = i; j < 12; j++) {
            buffer_LCD[2 + j] = ' ' + 0x80;
          }
        }
      }
      write_LCD_sleep(buffer_LCD);
      if (idle_count > idle_threshhold) return;
    }
    button_state = true;
    current_position_lcd += format_char_size;
    format_char_size = 1;
    if(current_position_lcd < 12) {
      buffer_LCD[2 + current_position_lcd] = 0x3D;
      current_position_format = 0;
      WRITE_LCD(buffer_LCD)
    }
  }

  for (i = 0; i < 12; i++) {
    if (buffer_LCD[2 + i] == 0x3D)
      settings.s.format[i] = ' ';
    else
      settings.s.format[i] = buffer_LCD[2 + i] - 0x80;
  }

  update_max_scroll_position();
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

  for (j = 0; j < 8; j++) {
    information[56 + j] = settings.buf[j] + 0x30;
  }
  for (j = 8; j < 20; j++) {
    information[56 + j] = settings.buf[j] ? settings.buf[j] : 0x20;
  }

  switch (settings.s.sleep){
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
        if (i < 64) i++;
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
    write_LCD_sleep(buffer_LCD);
    if (idle_count > idle_threshhold) return;
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
    write_LCD_sleep(buffer_LCD);
    if (idle_count > idle_threshhold) return;
  }
}

static void display_about(void) {

  uint32_t i = 0;

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
    for (uint32_t j = 0; j < 12; j++) {
      buffer_LCD[j + 2] = about[i + j] + 0x80;
    }
    write_LCD_sleep(buffer_LCD);
    if (idle_count > idle_threshhold) return;
  }
}

static void display_icons(void) {

  uint32_t j = 0;
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
    write_LCD_sleep(icons);
    if (idle_count > idle_threshhold) break;
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

    write_LCD_sleep(buffer_LCD);
    if (idle_count > idle_threshhold) break;
  }
  LED1_OFF
  LED2_OFF
}

static void load_settings(void) {

  read_EEPROM(_24C32WI_I2C_ADDR_7BIT, _24C32WI_SETTINGS_ADDRESS, _24C32WI_SETTINGS_LENGTH, settings.buf);

  update_max_scroll_position();
}

static void save_settings(void) {

  for (uint32_t i = 0; i < _24C32WI_SETTINGS_LENGTH + 1; i++ ) {
    if (settings.buf[i] != old_settings.buf[i]) {
      Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_EEPROM_WP, false);
      write_EEPROM(_24C32WI_I2C_ADDR_7BIT, _24C32WI_SETTINGS_ADDRESS, _24C32WI_SETTINGS_LENGTH, settings.buf);
      Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_EEPROM_WP, true);
      return;
    }
  }
}

static void load_birthdays(void) {

  for (uint32_t i = 0; i < BIRTHDAYS_COUNT; i++) {
    read_EEPROM(_24C32WI_I2C_ADDR_7BIT, _24C32WI_BIRTHDAYS_ADDRESS + _24C32WI_BIRTHDAY_LENGTH * i, BIRTHDAY_RECORD_LENGTH, birthday_record);
    birthdays[i].date[0] = birthday_record[0];
    birthdays[i].date[1] = birthday_record[1];
    for (uint32_t j = 0; j < BIRTHDAY_NAME_LENGTH; j++ ) {
      birthdays[i].name[j] = birthday_record[j + 2];
    }
  }
}

static void edit_birthdays(void) {

  uint32_t i;
  uint32_t birthday_nr = 0;
  uint32_t month,day;
  uint32_t current_position;

  set_menu(buffer_LCD, 38);

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
    write_LCD_sleep(buffer_LCD);
    if (idle_count > idle_threshhold) return;
  }
  button_state = true;

  month = FROM_BCD(birthdays[birthday_nr].date[0]);

  set_menu(buffer_LCD, 39);

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
    write_LCD_sleep(buffer_LCD);
    if (idle_count > idle_threshhold) return;
  }
  button_state = true;

  day = FROM_BCD(birthdays[birthday_nr].date[1]);

  set_menu(buffer_LCD, 40);

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
    write_LCD_sleep(buffer_LCD);
    if (idle_count > idle_threshhold) return;
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
        write_LCD_sleep(buffer_LCD);
        if (idle_count > idle_threshhold) return;
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
          write_LCD_sleep(buffer_LCD);
          if (idle_count > idle_threshhold) return;
          break;
        }
      }
    }
    if (i == BIRTHDAYS_COUNT) {
      birthday_nr--;
    }
    if (birthday_count == 0) {
      write_LCD_sleep(no_birthday);
      if (idle_count > idle_threshhold) return;
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

  load_settings();
  configure_SWD(settings.s.swd);

  //write register 0x0f (CLKOUT disabled)
  buf[0] = 0x07;
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_CLKOUT_CONTROL_REGISTER, PCA2129T_CLKOUT_CONTROL_LENGHT, buf);

  //write register 0x10 ( configure pulse interrupt active low )
  buf[0] = 0x23;
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_WATHCDOG_TIMER_CONTROL_REGISTER, PCA2129T_WATHCDOG_TIMER_CONTROL_LENGHT, buf);

  //write register 0 ( set seconds interrupt=1, minutes=2)
  buf[0] = (uint8_t)settings.s.freq;
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_CONTROL1_REGISTER, PCA2129T_CONTROL1_LENGTH, buf);

  buf[0] = 0x00;
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_CONTROL2_REGISTER, PCA2129T_CONTROL2_LENGTH, buf);

  buf[0] = 0x00;
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_CONTROL3_REGISTER, PCA2129T_CONTROL3_LENGTH, buf);

  //timestamp disable
  buf[0] = 0x40;
  write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_TIMESTAMP_CONTROL_REGISTER, PCA2129T_TIMESTAMP_CONTROL_LENGTH, buf);

  //write register 0x19 ( aging offset )
  buf[0] = settings.s.aging_offset;
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
        for (uint32_t i = 0; i < _24C32WI_SETTINGS_LENGTH; i++) {
          old_settings.buf[i] = settings.buf[i];
        }
        // idle threshhold 1 minute or 60 seconds
        idle_threshhold = (settings.s.freq == UPDATEFREQ_1_MIN) ? 1 : 60;
      }
      else {
        display_clock();
      }
    }
    else {
      // settings mode
      if (idle++ > idle_threshhold) {
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
          case 11: set_format(); break;
          case 14: set_aging_offset(); break;
          case 20: settings.s.freq = UPDATEFREQ_1_SEC; break;
          case 21: settings.s.freq = UPDATEFREQ_1_MIN; break;
          case 22: settings.s.scroll = true; break;
          case 23: settings.s.scroll = false; scroll_position = 0; break;
          case 25: settings.s.blink = BLINK_ON; break;
          case 26: settings.s.blink = BLINK_OFF; break;
          case 27: settings.s.blink = BLINK_1MIN; break;
          case 28: settings.s.icons_mode = ICONSMODE_POWER; break;
          case 29: settings.s.icons_mode = ICONSMODE_WEEKDAYS; break;
          case 30: settings.s.icons_mode = ICONSMODE_NONE; break;
          case 31: settings.s.separator_flip = true; break;
          case 32: settings.s.separator_flip = false; break;
          case 33: settings.s.swd = false; configure_SWD(false); break;
          case 34: settings.s.swd = true; configure_SWD(true); break;
          case 35: settings.s.sleep = PMU_MCU_POWER_DOWN; break;
          case 36: settings.s.sleep = PMU_MCU_DEEP_SLEEP; break;
          case 37: settings.s.sleep = PMU_MCU_SLEEP; break;
          case 41: display_font(); break;
          case 42: display_icons(); break;
          case 43: display_leds(); break;
          case 52: updateHour(-1); break;
          case 53: updateHour(1); break;
          default: break;
        }
        if (idle_count > idle_threshhold) {
          idle_count = 0;
          menu_position = 7;
        }
        button_state = true;
        if (menu_position == 7) { // exit settings
          clock_mode = true;
          if (old_settings.s.freq != settings.s.freq) {
            buf[0] = settings.s.freq;
            write_clock(PCA2129T_I2C_ADDR_7BIT, PCA2129T_CONTROL1_REGISTER, PCA2129T_CONTROL1_LENGTH, buf);
          }
          save_settings();
          clock_interrupt = true;
          display_clock();
        }
        else {
          menu_position = menu_enter[menu_position];
          switch (menu_position) {
            case 20:
              switch (settings.s.freq) {
                case UPDATEFREQ_1_SEC: break;
                case UPDATEFREQ_1_MIN: menu_position = 21; break;
                default: break;
              }; break;
            case 22: if (!settings.s.scroll) menu_position = 23; break;
            case 25: menu_position += settings.s.blink; break;
            case 28: menu_position += settings.s.icons_mode; break;
            case 31: if (!settings.s.separator_flip) menu_position = 32; break;
            case 33: if (settings.s.swd) menu_position = 34; break;
            case 35:
              switch (settings.s.sleep) {
                case PMU_MCU_POWER_DOWN: break;
                case PMU_MCU_DEEP_SLEEP: menu_position = 36; break;
                case PMU_MCU_SLEEP: menu_position = 37; break;
                default: break;
              }; break;
            default: break;
          }
          WRITE_LCD(strMenu[menu_position])
        }
      } else if (counter > old_counter) {
        idle = 0;
        if (counter - 3 > old_counter) {
          menu_position = menu_forward[menu_position];
          WRITE_LCD(strMenu[menu_position])
          old_counter = counter;
        }
      }
      else if (counter < old_counter) {
        idle = 0;
        if (counter + 3 < old_counter) {
          menu_position = menu_backward[menu_position];
          WRITE_LCD(strMenu[menu_position])
          old_counter = counter;
        }
      }
    }
  }
}

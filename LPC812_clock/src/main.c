/*
 * main.c
 *
 *      Author: Kestutis Bivainis
 */

#include "chip.h"
#include "PCA2129T.h"
#include "i2c.h"
#include "PCF2103.h"
#include "24C32WI.h"
#include "pins.h"

/* System oscillator rate and clock rate on the CLKIN pin */
const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

uint8_t strMenu[51][PCF2103_LCD_SEND_WIDTH]={
    //0 Settings
    {0x8C,0x40,'S'+0x80,'e'+0x80,'t'+0x80,'t'+0x80,'i'+0x80,'n'+0x80,'g'+0x80,'s'+0x80,0x20,0x20,0x20,0x77},
    //1 Set time
    {0x8C,0x40,'S'+0x80,'e'+0x80,'t'+0x80,0x20,'t'+0x80,'i'+0x80,'m'+0x80,'e'+0x80,0x20,0x20,0x20,0x77},
    //2 Set format
    {0x8C,0x40,'S'+0x80,'e'+0x80,'t'+0x80,0x20,'f'+0x80,'o'+0x80,'r'+0x80,'m'+0x80,'a'+0x80,'t'+0x80,0x20,0x77},
    //3 Update freq
    {0x8C,0x40,'U'+0x80,'p'+0x80,'d'+0x80,'a'+0x80,'t'+0x80,'e'+0x80,0x20,'f'+0x80,'r'+0x80,'e'+0x80,'q'+0x80,0x77},
    //4 1 sec
    {0x8C,0x40,'1'+0x80,0x20,'s'+0x80,'e'+0x80,'c'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //5 1 min
    {0x8C,0x40,'1'+0x80,0x20,'m'+0x80,'i'+0x80,'n'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //6 About
    {0x8C,0x40,'A'+0x80,'b'+0x80,'o'+0x80,'u'+0x80,'t'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x77},
    //7 CPU:LPC812
    {0x8C,0x40,'C'+0x80,'P'+0x80,'U'+0x80,':'+0x80,'L'+0x80,'P'+0x80,'C'+0x80,'8'+0x80,'1'+0x80,'2'+0x80,0x20,0x20},
    //8 ARMCortexM0+
    {0x8C,0x40,'A'+0x80,'R'+0x80,'M'+0x80,'C'+0x80,'o'+0x80,'r'+0x80,'t'+0x80,'e'+0x80,'x'+0x80,'M'+0x80,'0'+0x80,'+'+0x80},
    //9 RTC chip:
    {0x8C,0x40,'R'+0x80,'T'+0x80,'C'+0x80,0x20,'c'+0x80,'h'+0x80,'i'+0x80,'p'+0x80,':'+0x80,0x20,0x20,0x20},    
    //10 LCD:GA628
    {0x8C,0x40,'L'+0x80,'C'+0x80,'D'+0x80,':'+0x80,'G'+0x80,'A'+0x80,'6'+0x80,'2'+0x80,'8'+0x80,0x20,0x20,0x20},
    //11 24C32WI
    {0x8C,0x40,'2'+0x80,'4'+0x80,'C'+0x80,'3'+0x80,'2'+0x80,'W'+0x80,'I'+0x80,0x20,0x20,0x20,0x20,0x20},
    //12 I2C protocol
    {0x8C,0x40,'I'+0x80,'2'+0x80,'C'+0x80,0x20,'p'+0x80,'r'+0x80,'o'+0x80,'t'+0x80,'o'+0x80,'c'+0x80,'o'+0x80,'l'+0x80},
    //13 Kestutis
    {0x8C,0x40,0x20,0x20,'K'+0x80,'e'+0x80,'s'+0x80,'t'+0x80,'u'+0x80,'t'+0x80,'i'+0x80,'s'+0x80,0x20,0x20},
    //14 Bivainis
    {0x8C,0x40,0x20,0x20,'B'+0x80,'i'+0x80,'v'+0x80,'a'+0x80,'i'+0x80,'n'+0x80,'i'+0x80,'s'+0x80,0x20,0x20},
    //15 Exit
    {0x8C,0x40,'E'+0x80,'x'+0x80,'i'+0x80,'t'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x77},
    //16 HH:MM:SS
    {0x8C,0x40,'H'+0x80,'H'+0x80,':'+0x80,'M'+0x80,'M'+0x80,':'+0x80,'S'+0x80,'S'+0x80,0x20,0x20,0x20,0x20},
    //17 HH:MM
    {0x8C,0x40,'H'+0x80,'H'+0x80,':'+0x80,'M'+0x80,'M'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
    //18 HH:MM MI.DD
    {0x8C,0x40,'H'+0x80,'H'+0x80,':'+0x80,'M'+0x80,'M'+0x80,0x20,'M'+0x80,'I'+0x80,'.'+0x80,'D'+0x80,'D'+0x80,0x20},
    //19 HHMMSSDDMIYY
    {0x8C,0x40,'H'+0x80,'H'+0x80,'M'+0x80,'M'+0x80,'S'+0x80,'S'+0x80,'D'+0x80,'D'+0x80,'M'+0x80,'I'+0x80,'Y'+0x80,'Y'+0x80},
    //20 DDMIYY  
    {0x8C,0x40,'D'+0x80,'D'+0x80,'M'+0x80,'I'+0x80,'Y'+0x80,'Y'+0x80,0x20,0x20,0x20,0x20,0x20,0x20},
    //21 Weekday:
    {0x8C,0x40,'W'+0x80,'e'+0x80,'e'+0x80,'k'+0x80,'d'+0x80,'a'+0x80,'y'+0x80,':'+0x80,0x20,0x20,0x20,0x20},
    //22 HHMMSS
    {0x8C,0x40,'H'+0x80,'H'+0x80,'M'+0x80,'M'+0x80,'S'+0x80,'S'+0x80,0x20,0x20,0x20,0x20,0x20,0x20},
    //23 Set offset
    {0x8C,0x40,'S'+0x80,'e'+0x80,'t'+0x80,0x20,'o'+0x80,'f'+0x80,'f'+0x80,'s'+0x80,'e'+0x80,'t'+0x80,0x20,0x77},
    //24 Offset:
    {0x8C,0x40,'O'+0x80,'f'+0x80,'f'+0x80,'s'+0x80,'e'+0x80,'t'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20},
    //25 Set scroll
    {0x8C,0x40,'S'+0x80,'e'+0x80,'t'+0x80,0x20,'s'+0x80,'c'+0x80,'r'+0x80,'o'+0x80,'l'+0x80,'l'+0x80,0x20,0x77},
    //26 Set sleep
    {0x8C,0x40,'S'+0x80,'e'+0x80,'t'+0x80,0x20,'s'+0x80,'l'+0x80,'e'+0x80,'e'+0x80,'p'+0x80,0x20,0x20,0x77},
    //27 Sleep
    {0x8C,0x40,'S'+0x80,'l'+0x80,'e'+0x80,'e'+0x80,'p'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //28 Deep-sleep
    {0x8C,0x40,'D'+0x80,'e'+0x80,'e'+0x80,'p'+0x80,'-'+0x80,'s'+0x80,'l'+0x80,'e'+0x80,'e'+0x80,'p'+0x80,0x20,0x1f},
    //29 Power-down
    {0x8C,0x40,'P'+0x80,'o'+0x80,'w'+0x80,'e'+0x80,'r'+0x80,'-'+0x80,'d'+0x80,'o'+0x80,'w'+0x80,'n'+0x80,0x20,0x1f},
    //30 Off
    {0x8C,0x40,'O'+0x80,'f'+0x80,'f'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //31 On
    {0x8C,0x40,'O'+0x80,'n'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //32 Information
    {0x8C,0x40,'I'+0x80,'n'+0x80,'f'+0x80,'o'+0x80,'r'+0x80,'m'+0x80,'a'+0x80,'t'+0x80,'i'+0x80,'o'+0x80,'n'+0x80,0x77},
    //33 DEV-ID:
    {0x8C,0x40,'D'+0x80,'E'+0x80,'V'+0x80,'-'+0x80,'I'+0x80,'D'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20},
    //34 Speed:  Mhz  
    {0x8C,0x40,'S'+0x80,'p'+0x80,'e'+0x80,'e'+0x80,'d'+0x80,':'+0x80,0x20,0x20,'M'+0x80,'h'+0x80,'z'+0x80,0x20},
    // Mode:
    {0x8C,0x40,'M'+0x80,'o'+0x80,'d'+0x80,'e'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
    //36 PCA2129T
    {0x8C,0x40,'P'+0x80,'C'+0x80,'A'+0x80,'2'+0x80,'1'+0x80,'2'+0x80,'9'+0x80,'T'+0x80,0x20,0x20,0x20,0x20},
    //37 PCF2103
    {0x8C,0x40,'P'+0x80,'C'+0x80,'F'+0x80,'2'+0x80,'1'+0x80,'0'+0x80,'3'+0x80,0x20,0x20,0x20,0x20,0x20},
    //38 EEPROM:
    {0x8C,0x40,'E'+0x80,'E'+0x80,'P'+0x80,'R'+0x80,'O'+0x80,'M'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20},
    //39    2014
    {0x8C,0x40,0x20,0x20,0x20,0x20,'2'+0x80,'0'+0x80,'1'+0x80,'4'+0x80,0x20,0x20,0x20,0x20},
    //40 LCD Test
    {0x8C,0x40,'L'+0x80,'C'+0x80,'D'+0x80,0x20,'T'+0x80,'e'+0x80,'s'+0x80,'t'+0x80,0x20,0x20,0x20,0x77},
    //41 Font table
    {0x8C,0x40,'F'+0x80,'o'+0x80,'n'+0x80,'t'+0x80,0x20,'t'+0x80,'a'+0x80,'b'+0x80,'l'+0x80,'e'+0x80,0x20,0x77},
    //42 Offset:
    {0x8C,0x40,'O'+0x80,'f'+0x80,'f'+0x80,'s'+0x80,'e'+0x80,'t'+0x80,':'+0x80,0x20,0x20,0x20,0x20,0x20},
    //43 MI.DD HH:MM
    {0x8C,0x40,'M'+0x80,'I'+0x80,'.'+0x80,'D'+0x80,'D'+0x80,0x20,'H'+0x80,'H'+0x80,':'+0x80,'M'+0x80,'M'+0x80,0x20},
    //44 LED blink
    {0x8C,0x40,'L'+0x80,'E'+0x80,'D'+0x80,0x20,'b'+0x80,'l'+0x80,'i'+0x80,'n'+0x80,'k'+0x80,0x20,0x20,0x77},
    //45 On
    {0x8C,0x40,'O'+0x80,'n'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //46 Off
    {0x8C,0x40,'O'+0x80,'f'+0x80,'f'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
    //47 Icons mode
    {0x8C,0x40,'I'+0x80,'c'+0x80,'o'+0x80,'n'+0x80,'s'+0x80,0x20,'m'+0x80,'o'+0x80,'d'+0x80,'e'+0x80,0x20,0x77},
    //48 Show Power
    {0x8C,0x40,'S'+0x80,'h'+0x80,'o'+0x80,'w'+0x80,0x20,'p'+0x80,'o'+0x80,'w'+0x80,'e'+0x80,'r'+0x80,0x20,0x1f},
    //49 Weekdays
    {0x8C,0x40,'W'+0x80,'e'+0x80,'e'+0x80,'k'+0x80,'d'+0x80,'a'+0x80,'y'+0x80,'s'+0x80,0x20,0x20,0x20,0x1f},
    //50 None
    {0x8C,0x40,'N'+0x80,'o'+0x80,'n'+0x80,'e'+0x80,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1f},
  };
  
uint8_t icons[PCF2103_LCD_SEND_WIDTH]=
  {0x4C,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10};
  
uint8_t timeset[PCF2103_LCD_SEND_WIDTH];

uint8_t setday,setmonth,setyear,setwday,sethour,setminute,setsecond,setoffset;
  
uint8_t idle;  

volatile uint8_t inmenu;  
volatile uint8_t button_middle_pressed;
volatile uint8_t button_bottom_pressed;
volatile uint8_t button_top_pressed;
  
volatile uint8_t pos;
  
uint8_t format,oldformat,flipformat;
uint8_t freq,oldfreq;  
CHIP_PMU_MCUPOWER_T sleep,oldsleep;
uint8_t scroll,scrollpos,oldscroll;
uint8_t blink,oldblink;
uint8_t iconsmode,oldiconsmode;

uint8_t menuinit=false;

// middle button press - menu changes {"from","to"}
uint8_t button_middle_menu[12][2]={{0,1},
                   {1,20},
                   {2,16},
                   {3,4},                   
                   {6,7},
                   {23,24},
                   {25,30},
                   {26,27},
                   {32,33},
                   {40,41},
                   {44,45},
                   {47,48},
};

// bottom button press - menu changes {"from","to"}
uint8_t button_bottom_menu[46][2]={{0,40},
                   {1,2},
                   {2,3},
                   {3,23},
                   {4,5},
                   {5,4},
                   {6,15},
                   {7,8},
                   {8,9},
                   {9,36},
                   {10,37},
                   {11,12},
                   {12,13},
                   {13,14},
                   {14,39},
                   {15,0},
                   {16,17},
                   {17,18},
                   {18,43},
                   {19,16},
                   {23,25},
                   {25,26},
                   {26,44},
                   {27,28},
                   {28,29},
                   {29,27},
                   {30,31},
                   {31,30},
                   {32,6},
                   {33,34},
                   {34,35},
                   {35,42},
                   {36,10},
                   {37,38},
                   {38,11},
                   {39,7},
                   {40,32},
                   {42,33},
                   {43,19},
                   {44,47},
                   {45,46},
                   {46,45},
                   {47,1},
                   {48,49},
                   {49,50},
                   {50,48},
};

// top button press - menu changes {"from","to"}
uint8_t button_top_menu[43][2]={{1,0},
                   {2,0},
                   {3,0},
                   {4,3},
                   {5,3},
                   {7,6},
                   {8,6},
                   {9,6},
                   {10,6},
                   {11,6},
                   {12,6},
                   {13,6},
                   {14,6},
                   {16,2},
                   {17,2},
                   {18,2},
                   {19,2},
                   {23,0},
                   {25,0},
                   {26,0},
                   {27,26},
                   {28,26},
                   {29,26},
                   {30,25},
                   {31,25},
                   {32,15},
                   {33,32},
                   {34,32},
                   {35,32},
                   {36,6},
                   {37,6},
                   {38,6},
                   {39,6},
                   {41,40},
                   {42,32},
                   {43,2},
                   {44,0},
                   {45,44},
                   {46,44},
                   {47,0},
                   {48,47},
                   {49,47},
                   {50,47},
};

uint8_t val[16];
uint8_t str[12];

void setOffset(void){
  uint32_t i;
  uint8_t off;
  uint8_t sign;

  for(i=0;i<PCF2103_LCD_SEND_WIDTH;i++) {
    timeset[i]=strMenu[24][i];
  }
  
  readValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_AGING_OFFSET_REGISTER,PCA2129T_OFFSET_LENGHT,val);  
  
  setoffset=val[1];
  
  if(setoffset>8){
    off=setoffset-8;
    sign='+'+0x80;
  }
  else if(setoffset<8){
    off=8-setoffset;
    sign='-'+0x80;
  }
  else {
    off=0;
    sign=0x20;
  }

  timeset[9]=sign;
  timeset[10]=off+0xb0;

  
  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
        
  while(!button_bottom_pressed) {
  
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
    Chip_PMU_Sleep(LPC_PMU,(CHIP_PMU_MCUPOWER_T)sleep);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);
    
    if(button_top_pressed) {
      setoffset++;
      if(setoffset>PCA2129T_MAX_OFFSET) setoffset=PCA2129T_MIN_OFFSET;
      
       if(setoffset>8){
        off=setoffset-8;
        sign='+'+0x80;
      }
      else if(setoffset<8){
        off=8-setoffset;
        sign='-'+0x80;
      }
      else {
        off=0;
        sign=0x20;
      }

      timeset[9]=sign;
      timeset[10]=off+0xb0;
      
      setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
      button_top_pressed=false;
    }      
    else if(button_middle_pressed) {
      return;
    }
  }
  val[0]=TO_BCD(setoffset);
  setValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_AGING_OFFSET_REGISTER,PCA2129T_OFFSET_LENGHT,val);
}

void setTimeAndDate(void){
  int i;

  for(i=0;i<PCF2103_LCD_SEND_WIDTH;i++) {
    timeset[i]=strMenu[20][i];
  }
  
  readValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_TIMEDATE_REGISTER,PCA2129T_TIMEDATE_LENGHT,val);
  
  sethour=FROM_BCD(val[3]);
  setminute=FROM_BCD(val[2]);
  setsecond=FROM_BCD(val[1]);
  setday=FROM_BCD(val[4]);
  setwday=FROM_BCD(val[5]);
  setmonth=FROM_BCD(val[6]);
  setyear=FROM_BCD(val[7]);
  
  timeset[8]=TENS_PLACE_DIGIT(setday);
  timeset[9]=ONES_PLACE_DIGIT(setday);
  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
        
  while(!button_bottom_pressed) {
  
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
    Chip_PMU_Sleep(LPC_PMU,sleep);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);
    
    if(button_top_pressed) {
      setday++;
      if(setday>PCA2129T_MAX_DAY) setday=PCA2129T_MIN_DAY;
      timeset[8]=TENS_PLACE_DIGIT(setday);
      timeset[9]=ONES_PLACE_DIGIT(setday);
      setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
      button_top_pressed=false;
    }      
    else if(button_middle_pressed) {
      return;
    }
  }
  
  timeset[10]=TENS_PLACE_DIGIT(setmonth);
  timeset[11]=ONES_PLACE_DIGIT(setmonth);

  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);  
  button_bottom_pressed=false;
  while(!button_bottom_pressed) {
  
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
    Chip_PMU_Sleep(LPC_PMU,sleep);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);
    
    if(button_top_pressed) {
      setmonth++;
      if(setmonth>PCA2129T_MAX_MONTH) setmonth=PCA2129T_MIN_MONTH;
      timeset[10]=TENS_PLACE_DIGIT(setmonth);
      timeset[11]=ONES_PLACE_DIGIT(setmonth);
      setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
      button_top_pressed=false;
    }      
    else if(button_middle_pressed) {
      return;
    }
  }
  
  timeset[12]=TENS_PLACE_DIGIT(setyear);
  timeset[13]=ONES_PLACE_DIGIT(setyear);
  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);  
  button_bottom_pressed=false;
  while(!button_bottom_pressed) {
  
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
    Chip_PMU_Sleep(LPC_PMU,sleep);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);
    
    if(button_top_pressed) {
      setyear++;
      if(setyear>PCA2129T_MAX_YEAR) setyear=PCA2129T_MIN_YEAR;
      timeset[12]=TENS_PLACE_DIGIT(setyear);
      timeset[13]=ONES_PLACE_DIGIT(setyear);
      setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
      button_top_pressed=false;
    }      
    else if(button_middle_pressed) {
      return;
    }
  }
  
  for(i=0;i<PCF2103_LCD_SEND_WIDTH;i++) {
    timeset[i]=strMenu[21][i];
  }
  
  timeset[10]=setwday+0xb0;
  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
  button_bottom_pressed=false;
  while(!button_bottom_pressed) {
  
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
    Chip_PMU_Sleep(LPC_PMU,sleep);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);
    
    if(button_top_pressed) {
      setwday++;
      if(setwday>PCA2129T_MAX_WORKDAY) setwday=PCA2129T_MIN_WORKDAY;
      timeset[10]=setwday+0xb0;
      setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
      button_top_pressed=false;
    }      
    else if(button_middle_pressed) {
      return;
    }
  }
  for(i=0;i<PCF2103_LCD_SEND_WIDTH;i++){
    timeset[i]=strMenu[22][i];
  }
  
  timeset[8]=TENS_PLACE_DIGIT(sethour);
  timeset[9]=ONES_PLACE_DIGIT(sethour);
  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
  button_bottom_pressed=false;      
  while(!button_bottom_pressed) {
  
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
    Chip_PMU_Sleep(LPC_PMU,sleep);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);
    
    if(button_top_pressed) {
      sethour++;
      if(sethour>PCA2129T_MAX_HOUR) sethour=PCA2129T_MIN_HOUR;
      timeset[8]=TENS_PLACE_DIGIT(sethour);
      timeset[9]=ONES_PLACE_DIGIT(sethour);
      setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
      button_top_pressed=false;
    }      
    else if(button_middle_pressed) {
      return;
    }
  }
  
  timeset[10]=TENS_PLACE_DIGIT(setminute);
  timeset[11]=ONES_PLACE_DIGIT(setminute);
  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);  
  button_bottom_pressed=false;
  while(!button_bottom_pressed) {
  
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
    Chip_PMU_Sleep(LPC_PMU,sleep);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);
    
    if(button_top_pressed) {
      setminute++;
      if(setminute>PCA2129T_MAX_MINUTE) setminute=PCA2129T_MIN_MINUTE;
      timeset[10]=TENS_PLACE_DIGIT(setminute);
      timeset[11]=ONES_PLACE_DIGIT(setminute);
      setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
      button_top_pressed=false;
    }      
    else if(button_middle_pressed) {
      return;
    }
  }
  
  timeset[12]=TENS_PLACE_DIGIT(setsecond);
  timeset[13]=ONES_PLACE_DIGIT(setsecond);
  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);  
  button_bottom_pressed=false;
  Chip_SYSCTL_EnablePINTWakeup(IRQ_BUTTON2);
  while(!button_bottom_pressed) {
  
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
    Chip_PMU_Sleep(LPC_PMU,sleep);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);
    
    if(button_top_pressed) {
      setsecond++;
      if(setsecond>PCA2129T_MAX_SECOND) setsecond=PCA2129T_MIN_SECOND;
      timeset[12]=TENS_PLACE_DIGIT(setsecond);
      timeset[13]=ONES_PLACE_DIGIT(setsecond);
      setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
      button_top_pressed=false;
    }      
    else if(button_middle_pressed) {
      return;
    }
  }
  
  val[0]=TO_BCD(setsecond);
  val[1]=TO_BCD(setminute);
  val[2]=TO_BCD(sethour);
  val[3]=TO_BCD(setday);
  val[4]=TO_BCD(setwday);
  val[5]=TO_BCD(setmonth);
  val[6]=TO_BCD(setyear);
  
  setValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_TIMEDATE_REGISTER,PCA2129T_TIMEDATE_LENGHT,val);
  Chip_SYSCTL_DisablePINTWakeup(IRQ_BUTTON2);
}  

void displayTimeAndPower(void) {
  
  uint32_t i;
  
  readValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_CONTROL3_REGISTER,PCA2129T_CONTROL3_LENGTH+PCA2129T_TIMEDATE_LENGHT,val);  
  
  for(i=2;i<PCF2103_LCD_SEND_WIDTH;i++){
    icons[i]=0x00;
  }
  
  switch(iconsmode) {
    case ICONSMODE_POWER:
      if(val[1]&0x04) {  
        icons[12]=0x04;    
      }
      else {
        icons[12]=0x1F;
      }
      icons[13]=0x10;
      
      switch(sleep){
        case PMU_MCU_SLEEP:
          icons[2]=0x10;
        break;
        case PMU_MCU_DEEP_SLEEP:
          icons[2]=0x1C;
        break;
        case PMU_MCU_POWER_DOWN:
          icons[2]=0x1F;
        break;
        default:
          icons[2]=0x00;
        break;
      }
      break;
    case ICONSMODE_WEEKDAYS:
      switch(val[6]) {
        case 0://sun
          icons[12]=0x1F;
          icons[13]=0x10;
        break;
        case 1://mon
          icons[2]=0x1F;
        break;
        case 2://tue
          icons[4]=0xFF;
        break;
        case 3://wed
          icons[5]=0xFF;
        break;
        case 4://thu
          icons[7]=0xFF;
        break;
        case 5://fri
          icons[8]=0xFF;
        break;
        case 6://sat
          icons[10]=0xFF;
        break;        
      }
      break;
    case ICONSMODE_NONE:
      break;
  }
  
  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,icons);
  
  switch(format){
  
    case FORMAT_HHMM:
      
      str[0]=FROM_BCD_HIGH(val[4])+0xb0;
      str[1]=FROM_BCD_LOW(val[4])+0xb0;
      str[2]=FROM_BCD_HIGH(val[3])+0xb0;
      str[3]=FROM_BCD_LOW(val[3])+0xb0;
    
      if(scroll){
        scrollpos++;
        if(scrollpos>7)
          scrollpos=0;
      }
      for(i=2;i<PCF2103_LCD_SEND_WIDTH;i++){
        val[i]=0x20;
      }
      val[scrollpos+2]=str[0];
      val[scrollpos+3]=str[1];
      if(freq==UPDATE_1_MIN)
        val[scrollpos+4]=':'+0x80;
      else {
        if(flipformat) 
          val[scrollpos+4]=':'+0x80;
        else
          val[scrollpos+4]='-'+0x80;
        flipformat=!flipformat;
      }
      val[scrollpos+5]=str[2];
      val[scrollpos+6]=str[3];          
      break;
    case FORMAT_HHMM_MIDD:
      
      str[0]=FROM_BCD_HIGH(val[4])+0xb0;
      str[1]=FROM_BCD_LOW(val[4])+0xb0;
      str[2]=FROM_BCD_HIGH(val[3])+0xb0;
      str[3]=FROM_BCD_LOW(val[3])+0xb0;      
      str[6]=FROM_BCD_HIGH(val[7])+0xb0;
      str[7]=FROM_BCD_LOW(val[7])+0xb0;
      str[8]=FROM_BCD_HIGH(val[5])+0xb0;
      str[9]=FROM_BCD_LOW(val[5])+0xb0;
    
      if(scroll){
        scrollpos++;
        if(scrollpos>1)
          scrollpos=0;
      }
      for(i=2;i<PCF2103_LCD_SEND_WIDTH;i++){
        val[i]=0x20;
      }
      
      val[scrollpos+2]=str[0];
      val[scrollpos+3]=str[1];
      if(freq==UPDATE_1_MIN)
        val[scrollpos+4]=':'+0x80;
      else {
        if(flipformat) 
          val[scrollpos+4]=':'+0x80;
        else
          val[scrollpos+4]='-'+0x80;
        flipformat=!flipformat;
      }
      val[scrollpos+5]=str[2];
      val[scrollpos+6]=str[3];
      val[scrollpos+7]=0x20;
      val[scrollpos+8]=str[6];
      val[scrollpos+9]=str[7];
      val[scrollpos+10]='.'+0x80;
      val[scrollpos+11]=str[8];
      val[scrollpos+12]=str[9];
      break;
    case FORMAT_MIDD_HHMM:
      
      str[0]=FROM_BCD_HIGH(val[4])+0xb0;
      str[1]=FROM_BCD_LOW(val[4])+0xb0;
      str[2]=FROM_BCD_HIGH(val[3])+0xb0;
      str[3]=FROM_BCD_LOW(val[3])+0xb0;      
      str[6]=FROM_BCD_HIGH(val[7])+0xb0;
      str[7]=FROM_BCD_LOW(val[7])+0xb0;
      str[8]=FROM_BCD_HIGH(val[5])+0xb0;
      str[9]=FROM_BCD_LOW(val[5])+0xb0;
    
      if(scroll){
        scrollpos++;
        if(scrollpos>1)
          scrollpos=0;
      }
      for(i=2;i<PCF2103_LCD_SEND_WIDTH;i++){
        val[i]=0x20;
      }
      val[scrollpos+2]=str[6];
      val[scrollpos+3]=str[7];
      val[scrollpos+4]='.'+0x80;
      val[scrollpos+5]=str[8];
      val[scrollpos+6]=str[9];
      val[scrollpos+7]=0x20;
      val[scrollpos+8]=str[0];
      val[scrollpos+9]=str[1];
      if(freq==UPDATE_1_MIN)
        val[scrollpos+10]=':'+0x80;
      else {
        if(flipformat) 
          val[scrollpos+10]=':'+0x80;
        else
          val[scrollpos+10]='-'+0x80;
        flipformat=!flipformat;
      }
      val[scrollpos+11]=str[2];
      val[scrollpos+12]=str[3];
      break;
    case FORMAT_HHMMSSYYMIDD:
      
      str[0]=FROM_BCD_HIGH(val[4])+0xb0;
      str[1]=FROM_BCD_LOW(val[4])+0xb0;
      str[2]=FROM_BCD_HIGH(val[3])+0xb0;
      str[3]=FROM_BCD_LOW(val[3])+0xb0;
      str[4]=FROM_BCD_HIGH(val[2])+0xb0;
      str[5]=FROM_BCD_LOW(val[2])+0xb0;
      str[6]=FROM_BCD_HIGH(val[8])+0xb0;
      str[7]=FROM_BCD_LOW(val[8])+0xb0;
      str[8]=FROM_BCD_HIGH(val[7])+0xb0;
      str[9]=FROM_BCD_LOW(val[7])+0xb0;
      str[10]=FROM_BCD_HIGH(val[5])+0xb0;
      str[11]=FROM_BCD_LOW(val[5])+0xb0;
    
      val[2]=str[0];
      val[3]=str[1];
      val[4]=str[2];
      val[5]=str[3];
      val[6]=str[4];
      val[7]=str[5];
      val[8]=str[6];
      val[9]=str[7];
      val[10]=str[8];
      val[11]=str[9];
      val[12]=str[10];
      val[13]=str[11];
      break;    
    default://FORMAT_HHMMSS
      
      str[0]=FROM_BCD_HIGH(val[4])+0xb0;
      str[1]=FROM_BCD_LOW(val[4])+0xb0;
      str[2]=FROM_BCD_HIGH(val[3])+0xb0;
      str[3]=FROM_BCD_LOW(val[3])+0xb0;
      str[4]=FROM_BCD_HIGH(val[2])+0xb0;
      str[5]=FROM_BCD_LOW(val[2])+0xb0;
    
      if(scroll){
        scrollpos++;
        if(scrollpos>4)
          scrollpos=0;
      }
      for(i=2;i<PCF2103_LCD_SEND_WIDTH;i++){
        val[i]=0x20;
      }
      val[scrollpos+2]=str[0];
      val[scrollpos+3]=str[1];
      val[scrollpos+4]=':'+0x80;
      val[scrollpos+5]=str[2];
      val[scrollpos+6]=str[3];
      val[scrollpos+7]=':'+0x80;
      val[scrollpos+8]=str[4];
      val[scrollpos+9]=str[5];
      break;
  }
  
  val[0]=0x8C;
  val[1]=0x40;
  
  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,val);
}

void displayTime(void) {
  
  uint32_t i;
  
  readValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_TIMEDATE_REGISTER,PCA2129T_TIMEDATE_LENGHT,val);  
  
  switch(format){
  
    case FORMAT_HHMM:
      
      str[0]=FROM_BCD_HIGH(val[3])+0xb0;
      str[1]=FROM_BCD_LOW(val[3])+0xb0;
      str[2]=FROM_BCD_HIGH(val[2])+0xb0;
      str[3]=FROM_BCD_LOW(val[2])+0xb0;
    
      if(scroll){
        scrollpos++;
        if(scrollpos>7)
          scrollpos=0;
      }
      for(i=2;i<PCF2103_LCD_SEND_WIDTH;i++){
        val[i]=0x20;
      }
      val[scrollpos+2]=str[0];
      val[scrollpos+3]=str[1];
      if(freq==UPDATE_1_MIN)
        val[scrollpos+4]=':'+0x80;
      else {
        if(flipformat) 
          val[scrollpos+4]=':'+0x80;
        else
          val[scrollpos+4]='-'+0x80;
        flipformat=!flipformat;
      }
      val[scrollpos+5]=str[2];
      val[scrollpos+6]=str[3];          
      break;
    case FORMAT_HHMM_MIDD:
      
      str[0]=FROM_BCD_HIGH(val[3])+0xb0;
      str[1]=FROM_BCD_LOW(val[3])+0xb0;
      str[2]=FROM_BCD_HIGH(val[2])+0xb0;
      str[3]=FROM_BCD_LOW(val[2])+0xb0;      
      str[6]=FROM_BCD_HIGH(val[6])+0xb0;
      str[7]=FROM_BCD_LOW(val[6])+0xb0;
      str[8]=FROM_BCD_HIGH(val[4])+0xb0;
      str[9]=FROM_BCD_LOW(val[4])+0xb0;
    
      if(scroll){
        scrollpos++;
        if(scrollpos>1)
          scrollpos=0;
      }
      for(i=2;i<PCF2103_LCD_SEND_WIDTH;i++){
        val[i]=0x20;
      }
      val[scrollpos+2]=str[0];
      val[scrollpos+3]=str[1];
      if(freq==UPDATE_1_MIN)
        val[scrollpos+4]=':'+0x80;
      else {
        if(flipformat) 
          val[scrollpos+4]=':'+0x80;
        else
          val[scrollpos+4]='-'+0x80;
        flipformat=!flipformat;
      }
      val[scrollpos+5]=str[2];
      val[scrollpos+6]=str[3];
      val[scrollpos+7]=0x20;
      val[scrollpos+8]=str[6];
      val[scrollpos+9]=str[7];
      val[scrollpos+10]='.'+0x80;
      val[scrollpos+11]=str[8];
      val[scrollpos+12]=str[9];
      break;
    case FORMAT_MIDD_HHMM:
      
      str[0]=FROM_BCD_HIGH(val[3])+0xb0;
      str[1]=FROM_BCD_LOW(val[3])+0xb0;
      str[2]=FROM_BCD_HIGH(val[2])+0xb0;
      str[3]=FROM_BCD_LOW(val[2])+0xb0;      
      str[6]=FROM_BCD_HIGH(val[6])+0xb0;
      str[7]=FROM_BCD_LOW(val[6])+0xb0;
      str[8]=FROM_BCD_HIGH(val[4])+0xb0;
      str[9]=FROM_BCD_LOW(val[4])+0xb0;
    
      if(scroll){
        scrollpos++;
        if(scrollpos>1)
          scrollpos=0;
      }
      for(i=2;i<PCF2103_LCD_SEND_WIDTH;i++){
        val[i]=0x20;
      }
      val[scrollpos+2]=str[6];
      val[scrollpos+3]=str[7];
      val[scrollpos+4]='.'+0x80;
      val[scrollpos+5]=str[8];
      val[scrollpos+6]=str[9];
      val[scrollpos+7]=0x20;
      val[scrollpos+8]=str[0];
      val[scrollpos+9]=str[1];
      if(freq==UPDATE_1_MIN)
        val[scrollpos+4]=':'+0x80;
      else {
        if(flipformat) 
          val[scrollpos+10]=':'+0x80;
        else
          val[scrollpos+10]='-'+0x80;
        flipformat=!flipformat;
      }
      val[scrollpos+11]=str[2];
      val[scrollpos+12]=str[3];
      break;
    case FORMAT_HHMMSSYYMIDD:
      
      val[2]=FROM_BCD_HIGH(val[3])+0xb0;
      val[3]=FROM_BCD_LOW(val[3])+0xb0;
      val[4]=FROM_BCD_HIGH(val[2])+0xb0;
      val[5]=FROM_BCD_LOW(val[2])+0xb0;
      val[6]=FROM_BCD_HIGH(val[1])+0xb0;
      val[7]=FROM_BCD_LOW(val[1])+0xb0;
      val[8]=FROM_BCD_HIGH(val[7])+0xb0;
      val[9]=FROM_BCD_LOW(val[7])+0xb0;
      val[10]=FROM_BCD_HIGH(val[6])+0xb0;
      val[11]=FROM_BCD_LOW(val[6])+0xb0;
      val[12]=FROM_BCD_HIGH(val[4])+0xb0;
      val[13]=FROM_BCD_LOW(val[4])+0xb0;
      break;    
    default://FORMAT_HHMMSS
      
      str[0]=FROM_BCD_HIGH(val[3])+0xb0;
      str[1]=FROM_BCD_LOW(val[3])+0xb0;
      str[2]=FROM_BCD_HIGH(val[2])+0xb0;
      str[3]=FROM_BCD_LOW(val[2])+0xb0;
      str[4]=FROM_BCD_HIGH(val[1])+0xb0;
      str[5]=FROM_BCD_LOW(val[1])+0xb0;
    
      if(scroll){
        scrollpos++;
        if(scrollpos>4)
          scrollpos=0;
      }
      for(i=2;i<PCF2103_LCD_SEND_WIDTH;i++){
        val[i]=0x20;
      }
      val[scrollpos+2]=str[0];
      val[scrollpos+3]=str[1];
      val[scrollpos+4]=':'+0x80;
      val[scrollpos+5]=str[2];
      val[scrollpos+6]=str[3];
      val[scrollpos+7]=':'+0x80;
      val[scrollpos+8]=str[4];
      val[scrollpos+9]=str[5];
      break;
  }
  
  val[0]=0x8C;
  val[1]=0x40;
  
  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,val);
}

void displayPower(void) {
  
  readValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_CONTROL3_REGISTER,PCA2129T_CONTROL3_LENGTH,val);
  
  if(val[1]&0x04) {  
    icons[12]=0x04;    
  }
  else {
    icons[12]=0x1F;
  }
  
  switch(sleep){
    case PMU_MCU_SLEEP:
      icons[2]=0x10;
      break;
    case PMU_MCU_DEEP_SLEEP:
      icons[2]=0x1C;
      break;
    case PMU_MCU_POWER_DOWN:
      icons[2]=0x1F;
      break;
    default:
      icons[2]=0x00;
      break;
  }
  
  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,icons);
}

void displayInfo(void) {
  
  uint32_t i;
  
  uint8_t off;
  uint8_t sign;

  for(i=0;i<PCF2103_LCD_SEND_WIDTH;i++) {
    timeset[i]=strMenu[33][i];
  }
  
  timeset[9]=((LPC_SYSCTL->DEVICEID>>12)&0x0F)+0xb0;
  timeset[10]=((LPC_SYSCTL->DEVICEID>>8)&0x0F)+0xb0;
  timeset[11]=((LPC_SYSCTL->DEVICEID>>4)&0x0F)+0xb0;
  timeset[12]=((LPC_SYSCTL->DEVICEID)&0x0F)+0xb0;  
  
  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
  button_bottom_pressed=false;
  while(!button_bottom_pressed) {
  
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
    Chip_PMU_Sleep(LPC_PMU,(CHIP_PMU_MCUPOWER_T)sleep);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);
            
    if(button_middle_pressed|button_top_pressed) {
      pos=32;
      return;
    }
  }
  
  for(i=0;i<PCF2103_LCD_SEND_WIDTH;i++) {
    timeset[i]=strMenu[34][i];
  }
  
  timeset[8]=TENS_PLACE_DIGIT(SystemCoreClock/1000000);
  timeset[9]=ONES_PLACE_DIGIT(SystemCoreClock/1000000);
  
  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
  button_bottom_pressed=false;  
  while(!button_bottom_pressed) {
  
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
    Chip_PMU_Sleep(LPC_PMU,(CHIP_PMU_MCUPOWER_T)sleep);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);
            
    if(button_middle_pressed|button_top_pressed) {
      pos=32;
      return;
    }
  }
  
  for(i=0;i<PCF2103_LCD_SEND_WIDTH;i++) {
    timeset[i]=strMenu[35][i];
  }
  
  switch(sleep){
    case PMU_MCU_SLEEP:
      timeset[7]='S'+0x80;
      timeset[8]='l'+0x80;
      timeset[9]='e'+0x80;
      timeset[10]='e'+0x80;
      timeset[11]='p'+0x80;
      break;
    case PMU_MCU_DEEP_SLEEP:
      timeset[7]='D'+0x80;
      timeset[8]='e'+0x80;
      timeset[9]='e'+0x80;
      timeset[10]='p'+0x80;
      timeset[11]='S'+0x80;
      timeset[12]='l'+0x80;
      timeset[13]='p'+0x80;
      break;
    case PMU_MCU_POWER_DOWN:
      timeset[7]='P'+0x80;
      timeset[8]='w'+0x80;
      timeset[9]='r'+0x80;
      timeset[10]='D'+0x80;
      timeset[11]='o'+0x80;
      timeset[12]='w'+0x80;
      timeset[13]='n'+0x80;
      break;
    default:
      break;
  }
  
  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
  
  button_bottom_pressed=false;  
  while(!button_bottom_pressed) {
  
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
    Chip_PMU_Sleep(LPC_PMU,(CHIP_PMU_MCUPOWER_T)sleep);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);
            
    if(button_middle_pressed|button_top_pressed) {
      pos=32;
      return;
    }
  }
  
  for(i=0;i<PCF2103_LCD_SEND_WIDTH;i++) {
    timeset[i]=strMenu[42][i];
  }
  
  readValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_AGING_OFFSET_REGISTER,PCA2129T_OFFSET_LENGHT,val);  
  
  if(val[1]>8){
    off=val[1]-8;
    sign='+'+0x80;
  }
  else if(val[1]<8){
    off=8-val[1];
    sign='-'+0x80;
  }
  else {
    off=0;
    sign=0x20;
  }

  timeset[9]=sign;
  timeset[10]=off+0xb0;

  setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,timeset);
  
  button_bottom_pressed=false;  
  while(!button_bottom_pressed) {
  
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
    Chip_PMU_Sleep(LPC_PMU,(CHIP_PMU_MCUPOWER_T)sleep);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);
            
    if(button_middle_pressed|button_top_pressed) {
      pos=32;
      return;
    }
  }
  
  pos=32;
}

void displayTest(void) {
  
  uint8_t i=0,flip=true;  
  
  button_bottom_pressed=false;
  button_top_pressed=false;
  button_middle_pressed=false;    
  while((!button_middle_pressed)&(!button_top_pressed)) {
  
    if(button_bottom_pressed) {
      if(flip) {
        val[0]=0x4C;
        val[1]=0x40;
        val[2]=0x1F;
        val[3]=0x00;
        val[4]=0xFF;
        val[5]=0xFF;
        val[6]=0x00;
        val[7]=0xFF;
        val[8]=0xFF;
        val[9]=0x00;
        val[10]=0xFF;
        val[11]=0x00;
        val[12]=0x1F;
        val[13]=0x10;
      }
      else {
        val[0]=0x4C;
        val[1]=0x40;
        val[2]=0x00;
        val[3]=0x00;
        val[4]=0x00;
        val[5]=0x00;
        val[6]=0x00;
        val[7]=0x00;
        val[8]=0x00;
        val[9]=0x00;
        val[10]=0x00;
        val[11]=0x00;
        val[12]=0x00;
        val[13]=0x00;      
      }
    
      flip=!flip;
      
      setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,val);
    
      val[0]=0x8C;
      val[1]=0x40;
      val[2]='h'+0x80;
      val[3]=((i>>4)>9?(i>>4)+7:(i>>4))+0xb0;
      val[4]=(i&0x0F)+0xb0;
      val[5]=':'+0x80;
      val[6]=i++;
      val[7]=i++;
      val[8]=i++;
      val[9]=i++;
      val[10]=i++;
      val[11]=i++;
      val[12]=i++;
      val[13]=i++;
  
      setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,val);
      button_bottom_pressed=false;
    }
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
    Chip_PMU_Sleep(LPC_PMU,(CHIP_PMU_MCUPOWER_T)sleep);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);
            
  }
  pos=40;
  displayPower();
}

int main(void)
{
  int i;
  volatile int j;
  uint32_t command[3];
  uint32_t response[3];

  // Read clock settings and update SystemCoreClock variable
  SystemCoreClockUpdate();
  
  command[0]=12;
  command[1]=PWR_LOW_CURRENT;
  command[2]=12;
  
  LPC_PWRD_API->set_power(command,response);

  Init_Pins();
  
  /* Allocate I2C handle, setup I2C rate, and initialize I2C
     clocking */
  setupI2CMaster();

  j=1;
  for(i=0;i<100000;i++) {
    j+=1;
  }

  readEEPROM(_24C32WI_I2C_ADDR_7BIT,_24C32WI_SETTINGS_ADDRESS,_24C32WI_SETTINGS_LENGTH,val);

  format=val[1];
  freq=val[2];
  sleep=(CHIP_PMU_MCUPOWER_T)val[3];
  scroll=val[4];
  blink=val[5];
  iconsmode=val[6];

  if(format>4) format=FORMAT_HHMMSS;
  if(freq>2) freq=UPDATE_1_SEC;
  if(freq<1) freq=UPDATE_1_SEC;
  if(sleep>PMU_MCU_POWER_DOWN) sleep=PMU_MCU_POWER_DOWN;
  if(scroll>1) scroll=false;
  if(blink>1) blink=false;
  if(iconsmode>2) iconsmode=ICONSMODE_POWER;


  //write register 0x0f (CLKOUT disabled)
  val[0]=0x07;
  setValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_CLKOUT_CONTROL_REGISTER,PCA2129T_CLKOUT_CONTROL_LENGHT,val);

  //write register 0x10 ( configure pulse interrupt active low )
  val[0]=0x23;
  setValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_WATHCDOG_TIMER_CONTROL_REGISTER,PCA2129T_WATHCDOG_TIMER_CONTROL_LENGHT,val);

  //write register 0 ( set seconds interrupt=1, minutes=2)
  val[0]=freq;
  setValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_CONTROL1_REGISTER,PCA2129T_CONTROL1_LENGTH,val);

  val[0]=0x00;
  setValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_CONTROL2_REGISTER,PCA2129T_CONTROL2_LENGTH,val);

  val[0]=0x00;
  setValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_CONTROL3_REGISTER,PCA2129T_CONTROL3_LENGTH,val);
  
  //timestamp disable
  val[0]=0x40;
  setValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_TIMESTAMP_CONTROL_REGISTER,PCA2129T_TIMESTAMP_CONTROL_LENGTH,val);

  // init display
  val[0]=0x31;//Function set -> extended
  val[1]=0x02;//Screen configuration
  val[2]=0x05;//Display configuration
  val[3]=0x08;//Icon control
  val[4]=0x30;//Function set -> normal
  val[5]=0x0C;//Display control
  val[6]=0x06;//Entry mode set
  val[7]=0x02;//Return home
  
  setValues(PCF2103_I2C_ADDR_7BIT,0x00,8,val);

  displayTimeAndPower();

  Chip_SYSCTL_EnablePINTWakeup(IRQ_BUTTON1);
  Chip_SYSCTL_EnablePINTWakeup(IRQ_BUTTON2);
  Chip_SYSCTL_EnablePINTWakeup(IRQ_BUTTON3);
  Chip_SYSCTL_EnablePINTWakeup(IRQ_TIME);
  
  Chip_SYSCTL_SetDeepSleepPD(SYSCTL_DEEPSLP_BOD_PD | SYSCTL_DEEPSLP_WDTOSC_PD);
  Chip_SYSCTL_SetWakeup(~(SYSCTL_SLPWAKE_IRCOUT_PD | SYSCTL_SLPWAKE_IRC_PD | SYSCTL_SLPWAKE_FLASH_PD ));
  Chip_PMU_DisableDeepPowerDown(LPC_PMU);  

  while (1) {    

    if(!inmenu) {      
      
      Chip_PMU_Sleep(LPC_PMU,sleep);
      
      if(blink)
        Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED1, true);
      if(button_bottom_pressed) {
        if(format==FORMAT_HHMMSS) format=FORMAT_HHMM;
        else if(format==FORMAT_HHMM) format=FORMAT_HHMM_MIDD;        
        else if(format==FORMAT_HHMM_MIDD) format=FORMAT_MIDD_HHMM;
        else if(format==FORMAT_MIDD_HHMM) format=FORMAT_HHMMSSYYMIDD;
        else if(format==FORMAT_HHMMSSYYMIDD) format=FORMAT_HHMMSS;
        button_bottom_pressed=false;
      }
      else if(button_top_pressed) {
        if(freq==UPDATE_1_MIN)freq=UPDATE_1_SEC;
        else if(UPDATE_1_SEC)freq=UPDATE_1_MIN;
        val[0]=freq;
        setValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_CONTROL1_REGISTER,PCA2129T_CONTROL1_LENGTH,val);
        button_top_pressed=false;
      }
      displayTimeAndPower();
      if(blink)
        Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED1, false);
    }
    else {      
      
      if(menuinit) {
        if(freq==UPDATE_1_MIN) {
          val[0]=UPDATE_1_SEC;
          setValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_CONTROL1_REGISTER,PCA2129T_CONTROL1_LENGTH,val);
        }
      
        Chip_SYSCTL_DisablePINTWakeup(IRQ_BUTTON1);
        Chip_SYSCTL_DisablePINTWakeup(IRQ_BUTTON2);
        Chip_SYSCTL_DisablePINTWakeup(IRQ_BUTTON3);
        
        menuinit=false;
        oldfreq=freq;
        oldformat=format;
        oldscroll=scroll;
        oldsleep=sleep;
        oldblink=blink;
        
        idle=0;
      }
      
      setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,strMenu[pos]);
      
      button_bottom_pressed=false;
      button_top_pressed=false;
      button_middle_pressed=false;      
      
      Chip_PMU_Sleep(LPC_PMU,sleep);
      Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, true);            
      
      if(idle++>60) {
        
        if(freq==UPDATE_1_MIN) {
          val[0]=UPDATE_1_MIN;
          setValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_CONTROL1_REGISTER,PCA2129T_CONTROL1_LENGTH,val);
        }
          
        displayTimeAndPower();
        inmenu=false;

        Chip_SYSCTL_EnablePINTWakeup(IRQ_BUTTON1);
        Chip_SYSCTL_EnablePINTWakeup(IRQ_BUTTON2);
        Chip_SYSCTL_EnablePINTWakeup(IRQ_BUTTON3);
        pos=0;
      }      
      else if(button_bottom_pressed) {
        for(i=0;i<46;i++){
          if(button_bottom_menu[i][0]==pos){
            pos=button_bottom_menu[i][1];
            setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,strMenu[pos]);
            button_bottom_pressed=false;            
            break;
          }
        }
        idle=0;        
      }
      else if(button_top_pressed) {
        for(i=0;i<43;i++){
          if(button_top_menu[i][0]==pos){
            pos=button_top_menu[i][1];
            setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,strMenu[pos]);
            button_top_pressed=false;
            break;
          }
        }
        idle=0;
      }      
      else if(button_middle_pressed) {
        if(pos==15) {
          
          if((scroll!=oldscroll)||(oldformat!=format)||(oldsleep!=sleep)||(oldfreq!=freq)||(oldblink!=blink)||(oldiconsmode!=iconsmode)) {
          
            val[0]=_24C32WI_SETTINGS_ADDRESS;
            val[1]=format;
            val[2]=freq;
            val[3]=sleep;
            val[4]=scroll;
            val[5]=blink;
            val[6]=iconsmode;
   
            Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_EEPROM_WP, false);
            setValues(_24C32WI_I2C_ADDR_7BIT,_24C32WI_SETTINGS_ADDRESS>>8,_24C32WI_SETTINGS_LENGTH,val);
            Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_EEPROM_WP, true);            
          }
          
          if(freq==UPDATE_1_MIN) {
            val[0]=UPDATE_1_MIN;
            setValues(PCA2129T_I2C_ADDR_7BIT,PCA2129T_CONTROL1_REGISTER,PCA2129T_CONTROL1_LENGTH,val);
          }
          
          displayTimeAndPower();
          inmenu=false;
          button_middle_pressed=false;
          Chip_SYSCTL_EnablePINTWakeup(IRQ_BUTTON1);
          Chip_SYSCTL_EnablePINTWakeup(IRQ_BUTTON2);
          Chip_SYSCTL_EnablePINTWakeup(IRQ_BUTTON3);
        }
        else
        for(i=0;i<12;i++){
          if(button_middle_menu[i][0]==pos){            
            pos=button_middle_menu[i][1];
            if(scroll&&(pos==30))pos=31;
            else if((freq==UPDATE_1_MIN)&&(pos==4))pos=5;
            else if((sleep==PMU_MCU_DEEP_SLEEP)&&(pos==27))pos=28;
            else if((sleep==PMU_MCU_POWER_DOWN)&&(pos==27))pos=29;
            else if((format==FORMAT_HHMM)&&(pos==16))pos=17;
            else if((format==FORMAT_HHMM_MIDD)&&(pos==16))pos=18;
            else if((format==FORMAT_MIDD_HHMM)&&(pos==16))pos=43;
            else if((format==FORMAT_HHMMSSYYMIDD)&&(pos==16))pos=19;
            else if((blink==false)&&(pos==45))pos=46;
            
            setValues(PCF2103_I2C_ADDR_7BIT,0x80,PCF2103_LCD_SEND_WIDTH,strMenu[pos]);
            button_middle_pressed=false;
            break;
          }
        }
        idle=0;
      }
      
      switch(pos) {
        case 4:
          freq=UPDATE_1_SEC;
          break;
        case 5:
          freq=UPDATE_1_MIN;
        break;
        case 16:
          format=FORMAT_HHMMSS;
          break;
        case 17:
          format=FORMAT_HHMM;
          break;
        case 18:
          format=FORMAT_HHMM_MIDD;
          break;
        case 19:
          format=FORMAT_HHMMSSYYMIDD;
          break;
        case 20:
          setTimeAndDate();
          pos=1;        
          break;
        case 24:
          setOffset();
          pos=23;
          break;
        case 27:
          sleep=PMU_MCU_SLEEP;
          displayPower();
          break;
        case 28:
          sleep=PMU_MCU_DEEP_SLEEP;
          displayPower();        
          break;
        case 29:
          sleep=PMU_MCU_POWER_DOWN;
          displayPower();
          break;
        case 30:
          scroll=false;
          scrollpos=0;
          break;
        case 31:
          scroll=true;
          scrollpos=0;
          break;
        case 33:
          displayInfo();
          break;
        case 41:
          displayTest();
          break;
        case 43:
          format=FORMAT_MIDD_HHMM;
          break;
        case 45:
          blink=true;
          break;
        case 46:
          blink=false;
          break;
        case 48:
          iconsmode=ICONSMODE_POWER;
          break;
        case 49:
          iconsmode=ICONSMODE_WEEKDAYS;
          break;
        case 50:
          iconsmode=ICONSMODE_NONE;
          break;
      }
      Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);
    }  
  }
}

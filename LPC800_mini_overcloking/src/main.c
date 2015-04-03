/*
 *  main.c
 *
 *  Author: Kestutis Bivainis
 *
 */

#include "chip.h"
#include "spi.h"
#include "nokia6100.h"
#include "xprintf.h"
#include "delay.h"

const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

typedef struct {
  uint8_t MSEL;
  uint8_t PSEL;
  uint8_t DIV;
} MODE;

MODE modes[] = {
  {0,3,6},// 2 MHz
  {0,3,5},// 2.4 MHz
  {0,3,4},// 3 MHz
  {0,3,3},// 4 MHz
  {1,2,5},// 4.8 MHz
  {0,3,2},// 6 MHz
  {2,2,5},// 7.2 MHz
  {1,2,3},// 8 MHz
  {2,2,4},// 9 MHz
  {3,1,5},// 9.6 MHz
  {4,1,6},// 10 MHz
  {0,3,1},// 12 MHz
  {6,1,6},// 14 MHz
  {5,1,5},// 14.4 MHz
  {4,1,4},// 15 MHz
  {3,1,3},// 16 MHz
  {6,1,5},// 16.8 MHz
  {2,2,2},// 18 MHz
  {4,1,3},// 20 MHz
  {6,1,4},// 21 MHz
  {1,2,1},// 24 MHz
  {6,1,3},// 28 MHz
  {4,1,2},// 30 MHz max frequency from datasheet
  {2,2,1},// 36 MHz overclocked
  {6,1,2},// 42 MHz overclocked
  {3,1,1},// 48 MHz overclocked
  {4,1,1},// 60 MHz overclocked
  {5,1,1},// 72 MHz overclocked
};

volatile uint32_t counter=0;

void MRT_IRQHandler(void) {

  if (Chip_MRT_IntPending(LPC_MRT_CH0)) {
    Chip_MRT_IntClear(LPC_MRT_CH0);
    counter++;
  }
}

static uint32_t x;
static uint32_t y;
static uint32_t z;
static uint32_t w;
// Xorshift pseudorandom number generator
uint32_t xor128(void) {

  uint32_t t;

  t = x ^ (x << 11);
  x = y;
  y = z;
  z = w;
  return w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}

void display_info(uint32_t clk,uint8_t pos,uint32_t time){

  char buf[24];
  uint32_t after_decimal;

  // clear screen
  LCDRect(START_X,START_Y,END_X,END_Y,BLACK);

  LCDPutStr("LPC800 Mini-Kit", 120, 16);
  LCDPutStr("from 2 to 72 Mhz", 110,14);
  xsprintf(buf,"MSEL:%d PSEL:%d DIV:%d",modes[pos].MSEL,modes[pos].PSEL,modes[pos].DIV);
  LCDPutStr(buf,80,4);
  after_decimal=clk-(clk/1000)*1000;
  xsprintf(buf,"Clock: %d.%d MHz",clk/1000,after_decimal/100);
  LCDPutStr(buf,65,4);
  if(time) {
    xsprintf(buf,"Time: %d ms",counter);
    LCDPutStr(buf,20,4);
  }
  delay_ms(2000);
  // clear screen
  LCDRect(START_X,START_Y,END_X,END_Y,BLACK);
}

int32_t main(void) {

  uint32_t i,j,clk;

  uint32_t p1x;
  uint32_t p1y;
  uint32_t p2x;
  uint32_t p2y;
  uint32_t pcol;

  SystemCoreClockUpdate();

// init GPIO
  Chip_GPIO_Init(LPC_GPIO_PORT);
  Chip_SYSCTL_PeriphReset(RESET_GPIO);

// init LCD reset pin
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, RESET_PIN, true);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,0,RESET_PIN);

  // init SWM
  Chip_SWM_Init();
  //Chip_SWM_DisableFixedPin(SWM_FIXED_SWCLK);//PIO0_3
  //Chip_SWM_DisableFixedPin(SWM_FIXED_SWDIO);//PIO0_2
  Chip_SWM_DisableFixedPin(SWM_FIXED_ACMP_I2);//PIO0_1
  Chip_SWM_DisableFixedPin(SWM_FIXED_ACMP_I1);//PIO0_0
  Chip_SWM_MovablePinAssign(SWM_SPI0_SCK_IO,CLK_PIN);
  Chip_SWM_MovablePinAssign(SWM_SPI0_MOSI_IO,MOSI_PIN);

// init SPI0 at SystemCoreClock speed
  Chip_SPI_Init(LPC_SPI0);
  Chip_SPI_ConfigureSPI(LPC_SPI0,SPI_MODE_MASTER|
                                 SPI_CLOCK_CPHA0_CPOL0|
                                 SPI_DATA_MSB_FIRST|
                                 SPI_SSEL_ACTIVE_LO);
  //Chip_SPIM_SetClockRate(LPC_SPI0,SystemCoreClock);
  LPC_SPI0->DIV=0;
  Chip_SPI_Enable(LPC_SPI0);

  // init MRT
  Chip_MRT_Init();
// Channel0 for miliseconds delay
  Chip_MRT_SetMode(LPC_MRT_CH0,MRT_MODE_REPEAT);
  Chip_MRT_SetInterval(LPC_MRT_CH0, SystemCoreClock/1000 | MRT_INTVAL_LOAD);
  Chip_MRT_SetEnabled(LPC_MRT_CH0);
  NVIC_EnableIRQ(MRT_IRQn);

// init LCD
  LCDInit();
  // clear screen
  LCDRect(START_X,START_Y,END_X,END_Y,BLACK);

  LCDPutStr("LPC800 Mini-Kit", 120, 16);
  LCDPutStr("ARM Cortex M0+", 110, 20);
  LCDPutStr("overclocked to 72 Mhz", 100, 4);
  LCDPutStr("Demo shows 100 filled", 80, 4);
  LCDPutStr("rectangles drawn", 70, 4);
  LCDPutStr("on Nokia 6020 LCD", 60, 4);
  LCDPutStr("Clock speed increases", 40, 4);
  LCDPutStr("from 2 to 72 Mhz", 30,4);
  LCDPutStr("More at:", 10,4);
  LCDPutStr("kbiva.wordpress.com", 1,4);
  delay_ms(5000);

  // go from 2 to 72 Mhz
  for(j=0;j<sizeof(modes)/sizeof(MODE);j++) {

    // setup new clock speed using PLL
    // power down PLL when changing MSEL and PSEL, chapter 4.7.4.3.3 in LPC81x User manual (UM10601)
    Chip_Clock_SetMainClockSource(SYSCTL_MAINCLKSRC_IRC);
    Chip_SYSCTL_PowerDown(SYSCTL_SLPWAKE_SYSPLL_PD);
    Chip_Clock_SetupSystemPLL(modes[j].MSEL,modes[j].PSEL);
    Chip_SYSCTL_PowerUp(SYSCTL_SLPWAKE_SYSPLL_PD);
    while(!Chip_Clock_IsSystemPLLLocked()){};
    Chip_Clock_SetSysClockDiv(modes[j].DIV);
    Chip_Clock_SetMainClockSource(SYSCTL_MAINCLKSRC_PLLOUT);

    SystemCoreClockUpdate();

    clk=SystemCoreClock/1000;

    // Set new millisecond interval because clock speed changed
    Chip_MRT_SetInterval(LPC_MRT_CH0, clk | MRT_INTVAL_LOAD);

    display_info(clk,j,0);

    // reset pseudo-random number generator
    x = 123456789;
    y = 362436069;
    z = 521288629;
    w = 88675123;

    // reset milliseconds counter
    counter=0;

    // do 100 filled rectangles
    for (i = 0; i < 100; i++) {
      p1x = xor128() % MAX_X;
      p1y = xor128() % MAX_Y;
      p2x = xor128() % MAX_X;
      p2y = xor128() % MAX_Y;
      pcol = xor128() % 256;
      LCDRect(p1x,p1y,p2x,p2y, pcol);
    }
    // display results
    display_info(clk,j,counter);
  }
  while(1){};
}

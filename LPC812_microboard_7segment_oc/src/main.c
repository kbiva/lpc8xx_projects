/*
 *  main.c
 *
 *  LPC812 microboard and 2.5 digits indicator.
 *  Overclocks MCU up to 72Mhz.
 *  Displays current frequency on a 2.5 digits indicator.
 *  LED blinks at constant speed (period is 1s at any MCU frequency)
 *
 *  Author: Kestutis Bivainis
 */

#include "chip.h"
#include "board.h"
#include "2_5_digits_indicator.h"

extern uint32_t SystemCoreClock;

typedef struct {
  uint8_t MSEL;
  uint8_t PSEL;
  uint8_t DIV;
} MODE;

MODE modes[] = {
  {0,3,12},// 1 MHz
  {0,3,6},// 2 MHz
  {0,3,4},// 3 MHz
  {0,3,3},// 4 MHz
  {0,3,2},// 6 MHz
  {1,2,3},// 8 MHz
  {2,2,4},// 9 MHz
  {4,1,6},// 10 MHz
  {0,3,1},// 12 MHz
  {6,1,6},// 14 MHz
  {4,1,4},// 15 MHz
  {3,1,3},// 16 MHz
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

volatile uint32_t counter = 0;

void MRT_IRQHandler(void) {

  if (Chip_MRT_IntPending(LPC_MRT_CH0)) {
    Chip_MRT_IntClear(LPC_MRT_CH0);
    counter++;
    if (counter % 500 == 0)
      Board_LED_Toggle(0);
  }
}

void delay_ms(uint32_t msec) {

  counter = 0;
  while (counter < msec) {};
}

int main(void) {

  volatile uint32_t i;
  uint32_t j,clk;

  // Read clock settings and update SystemCoreClock variable
  SystemCoreClockUpdate();

  Board_Init();

  Init_2_5_Digits_Indicator(4, 0);

  // init MRT
  Chip_MRT_Init();
// Channel0 for miliseconds delay
  Chip_MRT_SetMode(LPC_MRT_CH0,MRT_MODE_REPEAT);
  Chip_MRT_SetInterval(LPC_MRT_CH0, SystemCoreClock / 1000 | MRT_INTVAL_LOAD);
  Chip_MRT_SetEnabled(LPC_MRT_CH0);
  NVIC_EnableIRQ(MRT_IRQn);

  Indicator_Display_Minus();
  delay_ms(2000);

  // go from 2 to 72 Mhz
  for (j = 0; j < sizeof(modes) / sizeof(MODE); j++) {

    // setup new clock speed using PLL
    // power down PLL when changing MSEL and PSEL, chapter 4.7.4.3.3 in LPC81x User manual (UM10601)
    Chip_Clock_SetMainClockSource(SYSCTL_MAINCLKSRC_IRC);
    Chip_SYSCTL_PowerDown(SYSCTL_SLPWAKE_SYSPLL_PD);
    Chip_Clock_SetupSystemPLL(modes[j].MSEL, modes[j].PSEL);
    Chip_SYSCTL_PowerUp(SYSCTL_SLPWAKE_SYSPLL_PD);
    while (!Chip_Clock_IsSystemPLLLocked()) {};
    Chip_Clock_SetSysClockDiv(modes[j].DIV);
    Chip_Clock_SetMainClockSource(SYSCTL_MAINCLKSRC_PLLOUT);

    SystemCoreClockUpdate();

    clk = SystemCoreClock / 1000;

    // Set new millisecond interval because clock speed changed
    Chip_MRT_SetInterval(LPC_MRT_CH0, clk | MRT_INTVAL_LOAD);

    clk = SystemCoreClock / 1000000;

    Indicator_Display_Dec(clk);

    delay_ms(1000);
  }
  
  Chip_Clock_SetMainClockSource(SYSCTL_MAINCLKSRC_IRC);
  
  while(1){};
}

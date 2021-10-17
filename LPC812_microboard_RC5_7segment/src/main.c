/*
 * main.c
 *
 *  Created on: 2014.01.26
 *      Author: Kestutis Bivainis
 */


#include "chip.h"
#include "board.h"
#include "2_5_digits_indicator.h"

#define IR_PIN 15

extern volatile bool RC5_flag;
extern volatile bool RC5_timeout;
extern uint32_t RC5_System;
extern uint32_t RC5_Command;
extern uint32_t RC5_Toggle;
extern uint32_t RC5_Frame;

extern void sct_fsm_init(void);

void PIN_INT0_IRQHandler(void) {

  Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH0);
}

int32_t main(void) {

  uint32_t cnt = 0;
  uint8_t RC5_System_prev = 0;
  uint8_t RC5_Command_prev = 0;
  CHIP_PMU_MCUPOWER_T mcupower = PMU_MCU_SLEEP;

  //Chip_Clock_SetMainClockSource(SYSCTL_MAINCLKSRC_IRC);
  
  SystemCoreClockUpdate();

  Board_Init();
  
  // init MRT
  Chip_MRT_Init();

  // init SWM
  Chip_SWM_Init();
  //Chip_SWM_DisableFixedPin(SWM_FIXED_SWCLK);//PIO0_3
  //Chip_SWM_DisableFixedPin(SWM_FIXED_SWDIO);//PIO0_2
  Chip_SWM_MovablePinAssign(SWM_CTIN_0_I, IR_PIN);
  Chip_SWM_Deinit();

  // Init indicator
  Init_2_5_Digits_Indicator(4, 0);
  Indicator_Display_Minus();

  // init SCT
  Chip_SCT_Init(LPC_SCT);
  // set prescaler, SCT clock = 1 MHz, clear counter
  LPC_SCT->CTRL_L |= SCT_CTRL_PRE_L(SystemCoreClock/1000000-1) | SCT_CTRL_CLRCTR_L;
  sct_fsm_init();
  NVIC_EnableIRQ(SCT_IRQn);

  // init IR_PIN pin interrupt for wakeup from sleep mode
  Chip_SYSCTL_SetPinInterrupt(0, IR_PIN);
  Chip_PININT_EnableIntLow(LPC_PININT, PININTCH0);
  NVIC_EnableIRQ(PININT0_IRQn);
  Chip_SYSCTL_EnablePINTWakeup(0);

  // set sleep options
  Chip_SYSCTL_SetWakeup(~(SYSCTL_SLPWAKE_IRCOUT_PD | SYSCTL_SLPWAKE_IRC_PD | SYSCTL_SLPWAKE_FLASH_PD));
  Chip_PMU_DisableDeepPowerDown(LPC_PMU);

  while (1) {

    // put chip to sleep
    Chip_PMU_Sleep(LPC_PMU, mcupower);

    // start MRT timer channel 0
    Chip_MRT_SetInterval(LPC_MRT_CH0, SystemCoreClock | MRT_INTVAL_LOAD);
    Chip_MRT_SetMode(LPC_MRT_CH0, MRT_MODE_ONESHOT);
    Chip_MRT_SetEnabled(LPC_MRT_CH0);

    // turn on onboard LED
    Board_LED_Set(0, true);

    // start SCT
    LPC_SCT->CTRL_L &= ~SCT_CTRL_HALT_L;

    // wait for timeout
    while (!RC5_timeout)
    {};

    // stop SCT
    LPC_SCT->CTRL_L |= SCT_CTRL_HALT_L;

    if (RC5_flag) {
      // if frame received, output information on indicator
      if ((RC5_System != RC5_System_prev) || (RC5_Command != RC5_Command_prev)) {
        cnt = 1;
        Indicator_Display_Hex(RC5_Command);
      }
      else {
        cnt++;
      }

      switch (RC5_Command) {
        case 0x50:
          mcupower = PMU_MCU_SLEEP;
        break;
        case 0x56:
          mcupower = PMU_MCU_DEEP_SLEEP;
        break;
        case 0x6B:
          mcupower = PMU_MCU_POWER_DOWN;
        break;
      }

      RC5_System_prev = RC5_System;
      RC5_Command_prev = RC5_Command;
    }
    // turn off onboard LED
    Board_LED_Set(0, false);
    // clear flags
    RC5_flag = false;
    RC5_timeout = false;
  }
}

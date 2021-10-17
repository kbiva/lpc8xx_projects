/*
 * main.c
 *
 *  Created on: 2014.01.26
 *      Author: Kestutis Bivainis
 */


#include "chip.h"
#include "board.h"
#include "spi.h"
#include "nokia6100.h"

#define IR_PIN 3

const char ascii[] = "0123456789ABCDEF";

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

  SystemCoreClockUpdate();

  Board_Init();
  
  // init MRT
  Chip_MRT_Init();

  // init SWM
  Chip_SWM_Init();
  Chip_SWM_DisableFixedPin(SWM_FIXED_SWCLK);//PIO0_3
  Chip_SWM_DisableFixedPin(SWM_FIXED_SWDIO);//PIO0_2
  Chip_SWM_MovablePinAssign(SWM_SPI0_SCK_IO, CLK_PIN);
  Chip_SWM_MovablePinAssign(SWM_SPI0_MOSI_IO, MOSI_PIN);  
  Chip_SWM_MovablePinAssign(SWM_CTIN_0_I, IR_PIN);
  Chip_SWM_Deinit();

  // init SPI0 at SystemCoreClock speed
  Chip_SPI_Init(LPC_SPI0);
  Chip_SPI_ConfigureSPI(LPC_SPI0,SPI_MODE_MASTER |
                                 SPI_CLOCK_CPHA0_CPOL0 |
                                 SPI_DATA_MSB_FIRST |
                                 SPI_SSEL_ACTIVE_LO);
  LPC_SPI0->DIV = 0;
  Chip_SPI_Enable(LPC_SPI0);

  // init LCD reset pin
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,0,RESET_PIN);

  // init LCD
  LCDInit();
  LCDClearScreenBlack();

  // init SCT
  Chip_SCT_Init(LPC_SCT);
  // set prescaler, SCT clock = 1 MHz, clear counter
  LPC_SCT->CTRL_L |= SCT_CTRL_PRE_L(SystemCoreClock/1000000-1) | SCT_CTRL_CLRCTR_L;
  sct_fsm_init();
  NVIC_EnableIRQ(SCT_IRQn);

  // init PIO0_3 pin interrupt for wakeup from sleep mode
  Chip_SYSCTL_SetPinInterrupt(0, IR_PIN);
  Chip_PININT_EnableIntLow(LPC_PININT, PININTCH0);
  NVIC_EnableIRQ(PININT0_IRQn);
  Chip_SYSCTL_EnablePINTWakeup(0);

  // set sleep options
  Chip_SYSCTL_SetWakeup(~(SYSCTL_SLPWAKE_IRCOUT_PD | SYSCTL_SLPWAKE_IRC_PD | SYSCTL_SLPWAKE_FLASH_PD ));
  Chip_PMU_DisableDeepPowerDown(LPC_PMU);

  LCDPutStr("kbiva.wordpress.com", MAX_X / 2 + 50, 10, WHITE, BLACK);
  LCDPutStr("RC5 Decoder", MAX_X / 2 + 35, 35, WHITE, BLACK);
  LCDPutStr("Frame:", MAX_X / 2 + 20, 1, WHITE, BLACK);
  LCDPutStr("System:", MAX_X / 2 + 5 , 1, WHITE, BLACK);
  LCDPutStr("Cmd:", MAX_X / 2 - 10, 1, WHITE, BLACK);
  LCDPutStr("Toggle:", MAX_X / 2 - 25, 1, WHITE, BLACK);
  LCDPutStr("Count:", MAX_X / 2 - 40, 1, WHITE, BLACK);

  while (1) {

    // put chip to sleep
    switch (mcupower) {
      case PMU_MCU_SLEEP:
      default:
        LCDPutStr("SLEEP     ", MAX_X / 2 - 60, 10, WHITE, BLACK);
        break;
      case PMU_MCU_DEEP_SLEEP:
        LCDPutStr("DEEP-SLEEP", MAX_X / 2 - 60, 10, WHITE, BLACK);
        break;
      case PMU_MCU_POWER_DOWN:
        LCDPutStr("POWER-DOWN", MAX_X / 2 - 60, 10, WHITE, BLACK);
        break;
    }
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
      }
      else {
        cnt++;
      }

        for (int32_t i = 3; i >= 0; i--) {
        LCDPutChar(ascii[(RC5_Frame >> (i * 4)) & 0x0F], MAX_X / 2 + 20, 80 + (3 - i) * 7, WHITE, BLACK);
        if (i < 2) {
          if ((RC5_System != RC5_System_prev) || (RC5_Command != RC5_Command_prev)) {
            LCDPutChar(ascii[(RC5_System >> (i * 4)) & 0x0F], MAX_X / 2 + 5, 66 + (3 - i) * 7, WHITE, BLACK);
            LCDPutChar(ascii[(RC5_Command >> (i * 4)) & 0x0F], MAX_X / 2 - 10, 66 + (3 - i) * 7, WHITE, BLACK);
          }
        }
        
        LCDPutChar(ascii[(cnt >> (i * 4)) & 0x0F],MAX_X / 2 - 40, 80 + (3 - i) * 7, WHITE, BLACK);
      }
      LCDPutStr(RC5_Toggle ? "ON ":"OFF", MAX_X / 2 - 25, 80, WHITE, BLACK);
      
      switch (RC5_Command) {
        case 0x50:
          mcupower = PMU_MCU_SLEEP;
        break;
        case 0x55:
          if (RC5_Toggle){
            spi0Transfer(SLEEPOUT);
            spi0Transfer(DISPON);
          }
          else {
            spi0Transfer(DISPOFF);
            spi0Transfer(SLEEPIN);
          }
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

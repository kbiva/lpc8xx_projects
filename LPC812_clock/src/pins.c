/*
 * pins.c
 *
 *      Author: Kestutis Bivainis
 */

#include "chip.h"
#include "pins.h"
#include "i2c.h"

extern volatile uint8_t menuinit;
extern volatile uint8_t inmenu;
extern volatile uint8_t pos;
extern volatile uint8_t button_middle_pressed;
extern volatile uint8_t button_bottom_pressed;
extern volatile uint8_t button_top_pressed;

void PININT1_IRQHandler(void) {
  Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH(1));
  if(!inmenu) {
    inmenu=true;
    menuinit=true;
    pos=0;
  }
  button_middle_pressed=true;
}
void PININT2_IRQHandler(void) {
  Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH(2));
  button_bottom_pressed=true;
}
void PININT3_IRQHandler(void) {
  Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH(3));
  button_top_pressed=true;
}

void PININT4_IRQHandler(void) {
  Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH(4));
}


void Init_Pins(void){

  Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
  Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);

  Chip_SWM_DisableFixedPin(SWM_FIXED_ACMP_I1);
  Chip_SWM_DisableFixedPin(SWM_FIXED_ACMP_I2);
  //Chip_SWM_DisableFixedPin(SWM_FIXED_SWCLK);
  //Chip_SWM_DisableFixedPin(SWM_FIXED_SWDIO);
  Chip_SWM_DisableFixedPin(SWM_FIXED_XTALIN);
  Chip_SWM_DisableFixedPin(SWM_FIXED_XTALOUT);
  //Chip_SWM_DisableFixedPin(SWM_FIXED_RST);
  Chip_SWM_DisableFixedPin(SWM_FIXED_CLKIN);
  Chip_SWM_DisableFixedPin(SWM_FIXED_VDDCMP);
  
  Chip_SYSCTL_SetPinInterrupt(IRQ_BUTTON1,PIN_BUTTON1);
  Chip_SYSCTL_SetPinInterrupt(IRQ_BUTTON2,PIN_BUTTON2);  
  Chip_SYSCTL_SetPinInterrupt(IRQ_BUTTON3,PIN_BUTTON3);
  Chip_SYSCTL_SetPinInterrupt(IRQ_TIME,PIN_PCA2129T_INTERRUPT);

  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 0);
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIO0,PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 0, false);
  
  Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, PIN_BUTTON1);//1
  
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 2);
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIO2,PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 2, false);
  
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 3);
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIO3,PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 3, false);
  
  Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, PIN_PCA2129T_INTERRUPT);
  
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 5);
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIO5,PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 5, false);
  
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 6);
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIO6,PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 6, false);
  
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 7);
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIO7,PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 7, false);
  
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 8);
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIO8,PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 8, false);
  
  Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, PIN_BUTTON2);//9  
  
  Chip_SWM_MovablePinAssign(SWM_I2C_SCL_IO, 10);
  
  Chip_SWM_MovablePinAssign(SWM_I2C_SDA_IO, 11);

#if (I2C_BITRATE >= 400000)
  /* Enable Fast Mode Plus for I2C pins */
  Chip_IOCON_PinSetI2CMode(LPC_IOCON, IOCON_PIO10, PIN_I2CMODE_FASTPLUS);
  Chip_IOCON_PinSetI2CMode(LPC_IOCON, IOCON_PIO11, PIN_I2CMODE_FASTPLUS);
#endif
  
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, PIN_LED1);//13
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIOLED1,PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED1, false);
  
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 14);
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIO14,PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 14, false);
  
  Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, PIN_BUTTON3);//15
  
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, PIN_EEPROM_WP);//16
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_EEPROM_WP, true);

  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, PIN_LED2);//17
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIOLED2,PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);

  Chip_IOCON_PinEnableHysteresis(LPC_IOCON,IOCON_PIOBUTTON1);
  Chip_IOCON_PinEnableHysteresis(LPC_IOCON,IOCON_PIOBUTTON2);
  Chip_IOCON_PinEnableHysteresis(LPC_IOCON,IOCON_PIOBUTTON3);
  
  Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH(1));
  Chip_PININT_EnableIntHigh(LPC_PININT, PININTCH(1));
  Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH(2));
  Chip_PININT_EnableIntHigh(LPC_PININT, PININTCH(2));
  Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH(3));
  Chip_PININT_EnableIntHigh(LPC_PININT, PININTCH(3));
  Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH(4));
  Chip_PININT_EnableIntLow(LPC_PININT, PININTCH(4));

  NVIC_EnableIRQ(PININT1_IRQn);
  NVIC_EnableIRQ(PININT2_IRQn);
  NVIC_EnableIRQ(PININT3_IRQn);
  NVIC_EnableIRQ(PININT4_IRQn);

  Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_IOCON);
  Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
}

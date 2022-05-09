/*
 * pins.c
 *
 *      Author: Kestutis Bivainis
 */

#include "chip.h"
#include "pins.h"
#include "i2c.h"
#include "chip.h"

volatile uint8_t last_encoded = 0;
volatile int16_t counter = 50;
// false - button is pressed, true - button is not pressed
volatile bool button_state = true;
// seconds or minutes interrupt flag
volatile bool clock_interrupt = false;

void PIN_INT1_IRQHandler(void) {

  Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH1);

  uint8_t MSB = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, 0, 1);
  uint8_t LSB = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, 0, 9);

  uint8_t encoded = (uint8_t)((MSB << 1) | LSB);
  uint8_t sum = (uint8_t)((last_encoded << 2) | encoded);

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) counter--;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) counter++;

  last_encoded = encoded;
}

void PIN_INT3_IRQHandler(void) {

  Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH3);

  uint8_t MSB = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, 0, 1);
  uint8_t LSB = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, 0, 9);

  uint8_t encoded = (MSB << 1) | LSB;
  uint8_t sum = (last_encoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) counter--;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) counter++;

  last_encoded = encoded;
}

void PIN_INT2_IRQHandler(void) {

  Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH2);
  button_state = Chip_GPIO_GetPinState(LPC_GPIO_PORT, 0, 15);
}

void PIN_INT4_IRQHandler(void) {

  Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH4);
  clock_interrupt = true;
}

void configure_SWD(bool value) {

  Chip_SWM_Init();
  if (value) {
    Chip_SWM_EnableFixedPin(SWM_FIXED_SWDIO);
    Chip_SWM_EnableFixedPin(SWM_FIXED_SWCLK);
  }
  else {
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);
    Chip_SWM_DisableFixedPin(SWM_FIXED_SWDIO);
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 2);
    Chip_IOCON_PinSetMode(LPC_IOCON, IOCON_PIO2, PIN_MODE_INACTIVE);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 2, false);
    Chip_SWM_DisableFixedPin(SWM_FIXED_SWCLK);
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 3);
    Chip_IOCON_PinSetMode(LPC_IOCON, IOCON_PIO3, PIN_MODE_INACTIVE);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 3, false);
    Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_IOCON);
  }
  Chip_SWM_Deinit();
}


void init_pins(void) {

  Chip_GPIO_Init(LPC_GPIO_PORT);
  Chip_SYSCTL_PeriphReset(RESET_GPIO);

  Chip_SWM_Init();
  Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);

  // 0 unused
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 0);
  Chip_IOCON_PinSetMode(LPC_IOCON, IOCON_PIO0, PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 0, false);

  // 1 encoder A (default pull-up)
  Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, 1);
  Chip_IOCON_PinEnableHysteresis(LPC_IOCON, IOCON_PIO1);

  // will be configured later
  // 2 SWDIO enabled by default
  // 3 SWCLK enabled by default

  // 4 clock interrupt input (default pull-up)
  Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, PIN_PCA2129T_INTERRUPT);

  // 5 external reset

  // 6 unused
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 6);
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIO6, PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 6, false);

  // 7 unused
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 7);
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIO7, PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 7, false);

  // 8 unused
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 8);
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIO8, PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 8, false);

  // 9 encoder B (default pull-up)
  Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, 9);
  Chip_IOCON_PinEnableHysteresis(LPC_IOCON,IOCON_PIO9);

  // 10 I2C SCL
  Chip_SWM_MovablePinAssign(SWM_I2C_SCL_IO, 10);
  Chip_IOCON_PinSetI2CMode(LPC_IOCON, IOCON_PIO10, PIN_I2CMODE_FASTPLUS);

  // 11 I2C SDA
  Chip_SWM_MovablePinAssign(SWM_I2C_SDA_IO, 11);
  Chip_IOCON_PinSetI2CMode(LPC_IOCON, IOCON_PIO11, PIN_I2CMODE_FASTPLUS);

  // 12 ISP
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 12);
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIO12, PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 12, false);

  // 13 LED1
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, PIN_LED1);
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIOLED1, PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED1, false);

  // 14 unused
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 14);
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIO14, PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 14, false);

  // 15 encoder button (default pull-up)
  Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, 15);
  Chip_IOCON_PinEnableHysteresis(LPC_IOCON, IOCON_PIO15);

  // 16 EEPROM write-enable (default pull-up)
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, PIN_EEPROM_WP);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_EEPROM_WP, true);

  // 17 LED2
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, PIN_LED2);
  Chip_IOCON_PinSetMode(LPC_IOCON,IOCON_PIOLED2, PIN_MODE_INACTIVE);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED2, false);

  // encoder A interrupt
  Chip_SYSCTL_SetPinInterrupt(1,1);
  Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH1);
  Chip_PININT_EnableIntLow(LPC_PININT, PININTCH1);
  Chip_PININT_EnableIntHigh(LPC_PININT, PININTCH1);

  // encoder B interrupt
  Chip_SYSCTL_SetPinInterrupt(3,9);
  Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH3);
  Chip_PININT_EnableIntLow(LPC_PININT, PININTCH3);
  Chip_PININT_EnableIntHigh(LPC_PININT, PININTCH3);

  // encoder button
  Chip_SYSCTL_SetPinInterrupt(2,15);
  Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH2);
  Chip_PININT_EnableIntLow(LPC_PININT, PININTCH2);

  // clock interrupt
  Chip_SYSCTL_SetPinInterrupt(IRQ_TIME, PIN_PCA2129T_INTERRUPT);
  Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH4);
  Chip_PININT_EnableIntLow(LPC_PININT, PININTCH4);

  NVIC_EnableIRQ(PIN_INT1_IRQn);
  NVIC_EnableIRQ(PIN_INT2_IRQn);
  NVIC_EnableIRQ(PIN_INT3_IRQn);
  NVIC_EnableIRQ(PIN_INT4_IRQn);

  Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_IOCON);
  Chip_SWM_Deinit();
}

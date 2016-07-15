/*
 *  main.c
 *
 *  LPC812 microboard and one blinking LED.
 *
 *  Author: Kestutis Bivainis
 */

#include "chip.h"

#define PIN_LED 17

extern uint32_t SystemCoreClock;

int main(void) {

  volatile uint32_t i;

  Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, PIN_LED);

  while (1) {

    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED, false);

    for(i=0;i<1000000;i++){};
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, PIN_LED, true);

    for(i=0;i<1000000;i++){};
  }
}

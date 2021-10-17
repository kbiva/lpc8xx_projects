/*
 *  main.c
 *
 *  LPC812 microboard and one blinking LED.
 *
 *  Author: Kestutis Bivainis
 */

#include "chip.h"
#include "board.h"

int main(void) {

  volatile uint32_t i;

  Board_Init();
  
  while (1) {

    Board_LED_Set(0, true);

    for (i = 0; i < 500000; i++) {};
    Board_LED_Set(0, false);

    for (i = 0; i < 500000; i++) {};
  }
}

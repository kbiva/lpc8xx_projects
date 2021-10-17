/*
 *  delay.c
 *
 *  Author: Kestutis Bivainis
 *
 */

#include "delay.h"

extern volatile uint32_t counter;

void delay_ms(uint32_t msec) {

  counter = 0;
  while (counter < msec) {};
}

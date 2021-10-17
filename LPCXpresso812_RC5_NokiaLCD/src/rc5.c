/*
 * rc5.c
 *
 *  Created on: 2014.01.26
 *      Author: Kestutis Bivainis
 */

#include "chip.h"
#include "rc5.h"

uint32_t RC5_System;
uint32_t RC5_Command;
uint32_t RC5_Toggle;
uint32_t RC5_Frame;

volatile bool RC5_flag;
volatile bool RC5_timeout;

static int32_t low_time;                                 // to store and save captured low time
static int32_t high_time;                                // to store and save captured high time
static uint16_t frame;                                   // rc5 frame (max 14 bits)

RC5EVENT rc5event;
RC5STATE rc5state;

const uint8_t trans[] = {0x01,0x91,0x9B,0xFB};

RC5STATE RC5Advance(RC5STATE State, RC5EVENT Event) {

  RC5STATE newState = (trans[State] >> Event) & 0x03;
  if (newState == State) {
    frame = 0;
  } else {
    if (newState == RC5STATE_MID0) {
      frame <<= 1;
      frame |= 1;
    } else if (newState == RC5STATE_MID1) {
      frame <<= 1;
    }
  }
  return newState;
}

void SCT_IRQHandler(void) {

  if (LPC_SCT->EVFLAG & SCT_EVT_0) {                       // Event 0 ? (timeout)
                                                           // to guarantee a 12 msec idle time after last RC5 pulse
    if (frame) {

      low_time  = LPC_SCT->CAP[2].L;                       // capture last low time
      if (checkVal(low_time, IR_RC5_0_PULSE, IR_RC5_TOL_PCT)) {
        RC5Advance(rc5state, RC5EVENT_SHORTSPACE);
      }
      else if(checkVal(low_time, IR_RC5_1_PULSE, IR_RC5_TOL_PCT)) {
             RC5Advance(rc5state, RC5EVENT_LONGSPACE);
           }

      RC5_Frame = frame;                               // save frame
      RC5_Command = frame & 0x003F;                    // save command byte
      if ((frame & 0x1000) == 0) {                        // if protocol is RC5 extended
        RC5_Command |= 0x40;
      }
      RC5_System = (frame & 0x07c0) >> 6;                // save system byte
      RC5_Toggle = (frame & 0x0800) >> 11;               // save toggle

      RC5_flag = true;
    }

    frame = 0;
    Chip_SCT_ClearEventFlag(LPC_SCT, SCT_EVT_0);          // clear event 0 flag
    RC5_timeout = true;
  }
  else {                                                  // Event 1 (CTIN_0 falling edge)

    high_time = LPC_SCT->CAP[1].L;                 // capture high time
    low_time = LPC_SCT->CAP[2].L;                 // and capture low time

    if (frame) {

      if (checkVal(high_time + low_time, IR_RC5_2_WIDTH, IR_RC5_TOL_PCT)) {
        rc5state = RC5Advance(rc5state, RC5EVENT_LONGSPACE);
        rc5state = RC5Advance(rc5state, RC5EVENT_LONGPULSE);
      }
      else if (checkVal(high_time + low_time, IR_RC5_0_WIDTH, IR_RC5_TOL_PCT)) {
         rc5state = RC5Advance(rc5state, RC5EVENT_SHORTSPACE);
         rc5state = RC5Advance(rc5state, RC5EVENT_SHORTPULSE);
      }
      else if (checkVal(high_time + low_time, IR_RC5_1_WIDTH, IR_RC5_TOL_PCT)) {
        if (checkVal(low_time, IR_RC5_0_PULSE, IR_RC5_TOL_PCT)) {
           rc5state = RC5Advance(rc5state, RC5EVENT_SHORTSPACE);
           rc5state = RC5Advance(rc5state, RC5EVENT_LONGPULSE);
        }
         else {
           rc5state = RC5Advance(rc5state, RC5EVENT_LONGSPACE);
           rc5state = RC5Advance(rc5state, RC5EVENT_SHORTPULSE);
         }
      }
     }
     else { // get low time from MRT Channel 0 for the first pulse
        low_time = 1000000 - Chip_MRT_GetTimer(LPC_MRT_CH0) / (SystemCoreClock / 1000000) - high_time;
        Chip_MRT_SetDisabled(LPC_MRT_CH0);

        // first two bits are 1 0
        if (checkVal(high_time + low_time, IR_RC5_0_HEADER_WIDTH, IR_RC5_TOL_PCT) &&
            checkVal(low_time, IR_RC5_0_HEADER_PULSE, IR_RC5_TOL_PCT) ) {
          frame = 0x02;
          rc5state = RC5Advance(RC5STATE_MID1,RC5EVENT_SHORTPULSE);
        }
        // first two bits are 1 0
        else if (checkVal(high_time + low_time, IR_RC5_1_HEADER_WIDTH, IR_RC5_TOL_PCT) &&
                  checkVal(low_time, IR_RC5_1_HEADER_PULSE, IR_RC5_TOL_PCT) ) {
               frame = 0x02;
               rc5state = RC5Advance(RC5STATE_MID1,RC5EVENT_LONGPULSE);
             }
        // first two bits are 1 1
        else if (checkVal(high_time + low_time, IR_RC5_2_HEADER_WIDTH, IR_RC5_TOL_PCT) &&
                checkVal(low_time, IR_RC5_2_HEADER_PULSE, IR_RC5_TOL_PCT) ) {
             frame = 0x01;
             rc5state = RC5Advance(RC5STATE_MID1,RC5EVENT_LONGPULSE);
        }
        // first two bits are 1 1
        else if (checkVal(high_time + low_time, IR_RC5_3_HEADER_WIDTH, IR_RC5_TOL_PCT) &&
                checkVal(low_time, IR_RC5_3_HEADER_PULSE, IR_RC5_TOL_PCT) ) {
             frame = 0x01;
             rc5state = RC5Advance(RC5STATE_MID1,RC5EVENT_SHORTPULSE);
        }

    }
    Chip_SCT_ClearEventFlag(LPC_SCT, SCT_EVT_1);          // clear event 1 flag
  }
}


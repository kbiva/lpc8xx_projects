/*
 * rc5.h
 *
 *  Created on: 2014.01.26
 *      Author: Kestutis Bivainis
 */

#ifndef RC5_H_
#define RC5_H_

// An Efficient Algorithm for Decoding RC5 Remote Control Signals
// http://www.clearwater.com.au/code/rc5
//
typedef enum {
    RC5EVENT_SHORTSPACE = 0,
    RC5EVENT_SHORTPULSE = 2,
    RC5EVENT_LONGSPACE  = 4,
    RC5EVENT_LONGPULSE  = 6,
    RC5EVENT_ERROR      = 8
} RC5EVENT;

typedef enum {
    RC5STATE_START1 = 0,
    RC5STATE_MID1   = 1,
    RC5STATE_MID0   = 2,
    RC5STATE_START0 = 3
} RC5STATE;

#define IR_RC5_TIME_UNIT_US        889

#define IR_RC5_0_HEADER_PULSE      (2*IR_RC5_TIME_UNIT_US)
#define IR_RC5_0_HEADER_WIDTH      (3*IR_RC5_TIME_UNIT_US)

#define IR_RC5_1_HEADER_PULSE      (2*IR_RC5_TIME_UNIT_US)
#define IR_RC5_1_HEADER_WIDTH      (4*IR_RC5_TIME_UNIT_US)

#define IR_RC5_2_HEADER_PULSE      (1*IR_RC5_TIME_UNIT_US)
#define IR_RC5_2_HEADER_WIDTH      (2*IR_RC5_TIME_UNIT_US)

#define IR_RC5_3_HEADER_PULSE      (1*IR_RC5_TIME_UNIT_US)
#define IR_RC5_3_HEADER_WIDTH      (3*IR_RC5_TIME_UNIT_US)

#define IR_RC5_0_WIDTH             (2*IR_RC5_TIME_UNIT_US)
#define IR_RC5_1_WIDTH             (3*IR_RC5_TIME_UNIT_US)
#define IR_RC5_2_WIDTH             (4*IR_RC5_TIME_UNIT_US)

#define IR_RC5_0_PULSE             (IR_RC5_TIME_UNIT_US)
#define IR_RC5_1_PULSE             (2*IR_RC5_TIME_UNIT_US)

#define IR_RC5_TOL_PCT             20

#define checkVal(var,val,tol) ((var)>((val)*(100-(tol))/100) && (var)<((val)*(100+(tol))/100))

RC5STATE RC5Advance(RC5STATE State, RC5EVENT Event);

#endif /* RC5_H_ */

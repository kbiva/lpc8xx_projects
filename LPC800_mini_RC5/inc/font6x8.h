/*
 * font6x8.h
 *
 *  Created on: 2013.09.11
 *      Author: Kestutis Bivainis
 */

#ifndef FONT6X8_H_
#define FONT6X8_H_

// *********************************************************************************
// Font tables for Nokia 6610 LCD Display Driver (PCF8833 Controller)
//
// FONT6x8 - SMALL font (mostly 5x7)
// FONT8x8 - MEDIUM font (8x8 characters, a bit thicker)
// FONT8x16 - LARGE font (8x16 characters, thicker)
//
// Note: ASCII characters 0x00 through 0x1F are not included in these fonts.
// First row of each font contains the number of columns, the
// number of rows and the number of bytes per character.
//
// Author: Jim Parise, James P Lynch July 7, 2007
// *********************************************************************************
const unsigned char FONT6x8[97*8] = {
  0x06,0x08,0x08,0x00,0x00,0x00,0x00,0x00, // columns, rows, num_bytes_per_char
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // space 0x20
  0x20,0x20,0x20,0x20,0x20,0x00,0x20,0x00, // !
  0x50,0x50,0x50,0x00,0x00,0x00,0x00,0x00, // "
  0x50,0x50,0xF8,0x50,0xF8,0x50,0x50,0x00, // #
  0x20,0x78,0xA0,0x70,0x28,0xF0,0x20,0x00, // $
  0xC0,0xC8,0x10,0x20,0x40,0x98,0x18,0x00, // %
  0x40,0xA0,0xA0,0x40,0xA8,0x90,0x68,0x00, // &
  0x30,0x30,0x20,0x40,0x00,0x00,0x00,0x00, // '
  0x10,0x20,0x40,0x40,0x40,0x20,0x10,0x00, // (
  0x40,0x20,0x10,0x10,0x10,0x20,0x40,0x00, // )
  0x00,0x20,0xA8,0x70,0x70,0xA8,0x20,0x00, // *
  0x00,0x20,0x20,0xF8,0x20,0x20,0x00,0x00, // +
  0x00,0x00,0x00,0x00,0x30,0x30,0x20,0x40, // ,
  0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00, // -
  0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00, // .
  0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00, // / (forward slash)
  0x70,0x88,0x88,0xA8,0x88,0x88,0x70,0x00, // 0 0x30
  0x20,0x60,0x20,0x20,0x20,0x20,0x70,0x00, // 1
  0x70,0x88,0x08,0x70,0x80,0x80,0xF8,0x00, // 2
  0xF8,0x08,0x10,0x30,0x08,0x88,0x70,0x00, // 3
  0x10,0x30,0x50,0x90,0xF8,0x10,0x10,0x00, // 4
  0xF8,0x80,0xF0,0x08,0x08,0x88,0x70,0x00, // 5
  0x38,0x40,0x80,0xF0,0x88,0x88,0x70,0x00, // 6
  0xF8,0x08,0x08,0x10,0x20,0x40,0x80,0x00, // 7
  0x70,0x88,0x88,0x70,0x88,0x88,0x70,0x00, // 8
  0x70,0x88,0x88,0x78,0x08,0x10,0xE0,0x00, // 9
  0x00,0x00,0x20,0x00,0x20,0x00,0x00,0x00, // :
  0x00,0x00,0x20,0x00,0x20,0x20,0x40,0x00, // ;
  0x08,0x10,0x20,0x40,0x20,0x10,0x08,0x00, // <
  0x00,0x00,0xF8,0x00,0xF8,0x00,0x00,0x00, // =
  0x40,0x20,0x10,0x08,0x10,0x20,0x40,0x00, // >
  0x70,0x88,0x08,0x30,0x20,0x00,0x20,0x00, // ?
  0x70,0x88,0xA8,0xB8,0xB0,0x80,0x78,0x00, // @ 0x40
  0x20,0x50,0x88,0x88,0xF8,0x88,0x88,0x00, // A
  0xF0,0x88,0x88,0xF0,0x88,0x88,0xF0,0x00, // B
  0x70,0x88,0x80,0x80,0x80,0x88,0x70,0x00, // C
  0xF0,0x88,0x88,0x88,0x88,0x88,0xF0,0x00, // D
  0xF8,0x80,0x80,0xF0,0x80,0x80,0xF8,0x00, // E
  0xF8,0x80,0x80,0xF0,0x80,0x80,0x80,0x00, // F
  0x78,0x88,0x80,0x80,0x98,0x88,0x78,0x00, // G
  0x88,0x88,0x88,0xF8,0x88,0x88,0x88,0x00, // H
  0x70,0x20,0x20,0x20,0x20,0x20,0x70,0x00, // I
  0x38,0x10,0x10,0x10,0x10,0x90,0x60,0x00, // J
  0x88,0x90,0xA0,0xC0,0xA0,0x90,0x88,0x00, // K
  0x80,0x80,0x80,0x80,0x80,0x80,0xF8,0x00, // L
  0x88,0xD8,0xA8,0xA8,0xA8,0x88,0x88,0x00, // M
  0x88,0x88,0xC8,0xA8,0x98,0x88,0x88,0x00, // N
  0x70,0x88,0x88,0x88,0x88,0x88,0x70,0x00, // O
  0xF0,0x88,0x88,0xF0,0x80,0x80,0x80,0x00, // P 0x50
  0x70,0x88,0x88,0x88,0xA8,0x90,0x68,0x00, // Q
  0xF0,0x88,0x88,0xF0,0xA0,0x90,0x88,0x00, // R
  0x70,0x88,0x80,0x70,0x08,0x88,0x70,0x00, // S
  0xF8,0xA8,0x20,0x20,0x20,0x20,0x20,0x00, // T
  0x88,0x88,0x88,0x88,0x88,0x88,0x70,0x00, // U
  0x88,0x88,0x88,0x88,0x88,0x50,0x20,0x00, // V
  0x88,0x88,0x88,0xA8,0xA8,0xA8,0x50,0x00, // W
  0x88,0x88,0x50,0x20,0x50,0x88,0x88,0x00, // X
  0x88,0x88,0x50,0x20,0x20,0x20,0x20,0x00, // Y
  0xF8,0x08,0x10,0x70,0x40,0x80,0xF8,0x00, // Z
  0x78,0x40,0x40,0x40,0x40,0x40,0x78,0x00, // [
  0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00, // \ (back slash)
  0x78,0x08,0x08,0x08,0x08,0x08,0x78,0x00, // ]
  0x20,0x50,0x88,0x00,0x00,0x00,0x00,0x00, // ^
  0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x00, // _
  0x60,0x60,0x20,0x10,0x00,0x00,0x00,0x00, // ` 0x60
  0x00,0x00,0x60,0x10,0x70,0x90,0x78,0x00, // a
  0x80,0x80,0xB0,0xC8,0x88,0xC8,0xB0,0x00, // b
  0x00,0x00,0x70,0x88,0x80,0x88,0x70,0x00, // c
  0x08,0x08,0x68,0x98,0x88,0x98,0x68,0x00, // d
  0x00,0x00,0x70,0x88,0xF8,0x80,0x70,0x00, // e
  0x10,0x28,0x20,0x70,0x20,0x20,0x20,0x00, // f
  0x00,0x00,0x70,0x98,0x98,0x68,0x08,0x70, // g
  0x80,0x80,0xB0,0xC8,0x88,0x88,0x88,0x00, // h
  0x20,0x00,0x60,0x20,0x20,0x20,0x70,0x00, // i
  0x10,0x00,0x10,0x10,0x10,0x90,0x60,0x00, // j
  0x80,0x80,0x90,0xA0,0xC0,0xA0,0x90,0x00, // k
  0x60,0x20,0x20,0x20,0x20,0x20,0x70,0x00, // l
  0x00,0x00,0xD0,0xA8,0xA8,0xA8,0xA8,0x00, // m
  0x00,0x00,0xB0,0xC8,0x88,0x88,0x88,0x00, // n
  0x00,0x00,0x70,0x88,0x88,0x88,0x70,0x00, // o
  0x00,0x00,0xB0,0xC8,0xC8,0xB0,0x80,0x80, // p 0x70
  0x00,0x00,0x68,0x98,0x98,0x68,0x08,0x08, // q
  0x00,0x00,0xB0,0xC8,0x80,0x80,0x80,0x00, // r
  0x00,0x00,0x78,0x80,0x70,0x08,0xF0,0x00, // s
  0x20,0x20,0xF8,0x20,0x20,0x28,0x10,0x00, // t
  0x00,0x00,0x88,0x88,0x88,0x98,0x68,0x00, // u
  0x00,0x00,0x88,0x88,0x88,0x50,0x20,0x00, // v
  0x00,0x00,0x88,0x88,0xA8,0xA8,0x50,0x00, // w
  0x00,0x00,0x88,0x50,0x20,0x50,0x88,0x00, // x
  0x00,0x00,0x88,0x88,0x78,0x08,0x88,0x70, // y
  0x00,0x00,0xF8,0x10,0x20,0x40,0xF8,0x00, // z
  0x10,0x20,0x20,0x40,0x20,0x20,0x10,0x00, // {
  0x20,0x20,0x20,0x00,0x20,0x20,0x20,0x00, // |
  0x40,0x20,0x20,0x10,0x20,0x20,0x40,0x00, // }
  0x40,0xA8,0x10,0x00,0x00,0x00,0x00,0x00, // ~
  0x70,0xD8,0xD8,0x70,0x00,0x00,0x00,0x00, // DEL
};


#endif /* FONT6X8_H_ */

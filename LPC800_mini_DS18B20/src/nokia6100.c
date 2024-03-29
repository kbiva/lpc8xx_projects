/*
 * nokia6100.c
 *
 *  Created on: 2013.09.11
 *      Author: Kestutis Bivainis
 */

#include <stdint.h>
#include "chip.h"
#include "nokia6100.h"
#include "font6x8.h"
#include "spi.h"


void LCDInit(void) {

  volatile uint32_t i;

//  Reset
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, RESET_PIN, false);
  for (i = 0; i < 12000; i++) {};
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, RESET_PIN, true);
  for(i = 0; i < 600000; i++) {};

  // Sleep out (command 0x11)
  spi0Transfer(SLEEPOUT);

#ifdef _NOKIA6100
  // Inversion on (command 0x20)
  spi0Transfer(INVON);
#endif

  // Color Interface Pixel Format (command 0x3A)
  spi0Transfer(COLMOD);
#ifdef _8BITCOLOR
  spi0Transfer(COLOR_MODE_8BIT | 0x100);
  LCDSetup8BitColor();
#elif defined _12BITCOLOR
  spi0Transfer(COLOR_MODE_12BIT | 0x100);
#elif defined _16BITCOLOR
  spi0Transfer(COLOR_MODE_16BIT | 0x100);
#endif

  // Memory access controller (command 0x36)
  // 0xC8 = mirror x and y, reverse rgb    Nokia 6100
  // 0x80 = mirror y                       Nokia 6030
  // 0x80 = mirror y                       Nokia 3100
  spi0Transfer(MADCTL);
  spi0Transfer(MADCTL_DATA | 0x100);
  // Write contrast (command 0x25)
  spi0Transfer(SETCON);
  spi0Transfer(SETCON_DATA | 0x100);

  // Display On (command 0x29)
  spi0Transfer(DISPON);
}

void LCDClearScreenBlack(void) {
  uint32_t i;

  LCDSetWindow(START_X, START_Y, END_X, END_Y);

#ifdef _8BITCOLOR
  for (i = 0; i < ((MAX_X * MAX_Y)); i++) {
    spi0Transfer((BLACK & 0xFF) | 0x100);
  }
#elif defined _12BITCOLOR
  for (i = 0; i < ((MAX_X * MAX_Y) / 2); i++)
  {
    spi0Transfer(((BLACK >> 4)&0xFF)|0x100);
    spi0Transfer((((BLACK & 0x0F) << 4) | ((BLACK >> 8) & 0x0F))|0x100);
    spi0Transfer((BLACK&0xFF)|0x100);
  }
#elif defined _16BITCOLOR
  for (i = 0; i < ((MAX_X * MAX_Y)); i++)
  {
    spi0Transfer((BLACK>>8)|0x100);
    spi0Transfer((BLACK&0xFF)|0x100);
  }
#endif

}

void LCDSetWindow(int32_t x1, int32_t y1,int32_t x2, int32_t y2) {
  // Row address set (command 0x2B)
  spi0Transfer(PASET);
  spi0Transfer(x1 | 0x100);
  spi0Transfer(x2 | 0x100);

  // Column address set (command 0x2A)
  spi0Transfer(CASET);
  spi0Transfer(y1 | 0x100);
  spi0Transfer(y2 | 0x100);

  // WRITE MEMORY
  spi0Transfer(RAMWR);
}

void LCDSetPixel(int32_t x, int32_t y, int32_t color) {

  LCDSetWindow(x, y, x, y);

#ifdef _8BITCOLOR
  spi0Transfer((color & 0xFF) | 0x100);
#elif defined _12BITCOLOR
  spi0Transfer(((color >> 4)&0xFF)|0x100);
  spi0Transfer(((color & 0x0F) << 4)|0x100);
#elif defined _16BITCOLOR
  spi0Transfer(((color >> 8)&0xFF)|0x100);
  spi0Transfer((color&0xFF)|0x100);
#endif
}

void LCDPutChar(char c, int32_t x, int32_t y, int32_t fColor, int32_t bColor) {
  int32_t i, j;
  uint32_t nCols;
  uint32_t nRows;
  uint32_t nBytes;
  uint8_t PixelRow;
  uint8_t Mask;
  uint32_t Word0;
  uint32_t Word1;
  uint8_t *pFont;
  uint8_t *pChar;
  uint8_t *FontTable[] = { (uint8_t*) FONT6x8 };

  // get pointer to the beginning of the selected font table
  pFont = (uint8_t*) FontTable[0];
  // get the nColumns, nRows and nBytes
  nCols = *pFont;
  nRows = *(pFont + 1);
  nBytes = *(pFont + 2);

  // get pointer to the last byte of the desired character
  pChar = pFont + (nBytes * (c - 0x1F)) + nBytes - 1;

  LCDSetWindow(x, y, x + nRows - 1, y + nCols - 1);

  // loop on each row, working backwards from the bottom to the top
  for (i = nRows - 1; i >= 0; i--) {
    // copy pixel row from font table and then decrement row
    PixelRow = *pChar--;

    // loop on each pixel in the row (left to right)
    // Note: we do two pixels each loop
    Mask = 0x80;
    for (j = 0; j < nCols; j += 2) {
      // if pixel bit set, use foreground color; else use the background color
      // now get the pixel color for two successive pixels

      if (PixelRow & Mask)
        Word0 = fColor;
      else
        Word0 = bColor;
      Mask >>= 1;

      if (PixelRow & Mask)
        Word1 = fColor;
      else
        Word1 = bColor;
      Mask >>= 1;

#ifdef _8BITCOLOR
      // use this information to output two data bytes
      spi0Transfer((Word0 & 0xFF) | 0x100);
      spi0Transfer((Word1 & 0xFF) | 0x100);
#elif defined _12BITCOLOR
      // use this information to output three data bytes
      spi0Transfer(((Word0 >> 4)&0xFF)|0x100);
      spi0Transfer(((Word0 & 0x0F) << 4)|((Word1 >> 8) & 0x0F)|0x100);
      spi0Transfer((Word1&0xFF)|0x100);
#elif defined _16BITCOLOR
      // use this information to output four data bytes
      spi0Transfer(((Word0 >> 8)&0xFF)|0x100);
      spi0Transfer((Word0&0xFF)|0x100);
      spi0Transfer(((Word1 >> 8)&0xFF)|0x100);
      spi0Transfer((Word1&0xFF)|0x100);
#endif
    }
  }
}

void LCDPutStr(char *pString, int32_t x, int32_t y, int32_t fColor, int32_t bColor) {
  // loop until null-terminator is seen
  while (*pString) {
    // draw the character
    LCDPutChar(*pString++, x, y, fColor, bColor);

    // advance the y position
    y = y + 6;

    // bail out if y exceeds 131
    if (y > END_Y)
      break;
  }
}

static uint8_t RGB8ColorMap_Nokia6020[48]= {
  0x00,0x04,0x09,0x0D,0x12,0x16,0x1B,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // red
  0x00,0x09,0x12,0x1B,0x24,0x2D,0x36,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // green
  0x00,0x0A,0x15,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // blue
};

void LCDSetup8BitColor(void) {

  uint32_t i;

  spi0Transfer(RGBSET);  // Define Color Table  (command 0x2D)

  for (i = 0; i < 48; i++) {
    spi0Transfer(RGB8ColorMap_Nokia6020[i] | 0x100);
  }
}


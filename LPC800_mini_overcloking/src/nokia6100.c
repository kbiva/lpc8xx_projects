/*
 *  nokia6100.c
 *
 *  Author: Kestutis Bivainis
 *
 */

#include "chip.h"
#include "nokia6100.h"
#include "font6x8.h"
#include "spi.h"
#include "delay.h"

void LCDInit(void) {

//  Reset
  Chip_GPIO_SetPinState(LPC_GPIO_PORT,0,RESET_PIN,false);
  delay_ms(5);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT,0,RESET_PIN,true);
  delay_ms(20);

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

  LCDSetWindow(x,y,x,y);

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

void LCDPutChar(char c, int32_t x, int32_t y) {
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

  LCDSetWindow(x,y,x + nRows - 1,y + nCols - 1);

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
        Word0 = WHITE;
      else
        Word0 = BLACK;
      Mask >>= 1;

      if (PixelRow & Mask)
        Word1 = WHITE;
      else
        Word1 = BLACK;
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

void LCDPutStr(char *pString, int32_t x, int32_t y) {
  // loop until null-terminator is seen
  while (*pString) {
    // draw the character
    LCDPutChar(*pString++, x, y);

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

  for(i=0;i<48;i++) {
    spi0Transfer(RGB8ColorMap_Nokia6020[i] | 0x100);
  }
}


void LCDRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t color) {
  int32_t xmin, xmax, ymin, ymax;
  int32_t i;

  // check if the rectangle is to be filled
    // best way to create a filled rectangle is to define a drawing box
    // and loop two pixels at a time
    // calculate the min and max for x and y directions
    xmin = (x0 <= x1) ? x0 : x1;
    xmax = (x0 > x1) ? x0 : x1;
    ymin = (y0 <= y1) ? y0 : y1;
    ymax = (y0 > y1) ? y0 : y1;

    // specify the controller drawing box according to those limits
    // Row address set (command 0x2B)
    spi0Transfer(PASET);
    spi0Transfer(xmin | 0x100);
    spi0Transfer(xmax | 0x100);

    // Column address set (command 0x2A)
    spi0Transfer(CASET);
    spi0Transfer(ymin | 0x100);
    spi0Transfer(ymax | 0x100);

    // WRITE MEMORY
    spi0Transfer(RAMWR);

#ifdef _8BITCOLOR
    // loop on total number of pixels
    for (i = 0; i < ((xmax - xmin + 1) * (ymax - ymin + 1)); i++) {
      spi0Transfer((color & 0xFF) | 0x100);
    }
#elif defined _12BITCOLOR
    // loop on total number of pixels / 2
    for (i = 0; i < ((((xmax - xmin + 1) * (ymax - ymin + 1)) / 2) + 1); i++)
    {
      // use the color value to output three data bytes covering two pixels
      spi0Transfer(((color >> 4)&0xFF)|0x100);
      spi0Transfer(((color & 0x0F) << 4) | ((color >> 8) & 0x0F)|0x100);
      spi0Transfer((color&0xFF)|0x100);
    }
#elif defined _16BITCOLOR
    // loop on total number of pixels
    for (i = 0; i < ((xmax - xmin + 1) * (ymax - ymin + 1)); i++)
    {
      spi0Transfer(((color >> 8)&0xFF)|0x100);
      spi0Transfer((color&0xFF)|0x100);
    }
#endif
}

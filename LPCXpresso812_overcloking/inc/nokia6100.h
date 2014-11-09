/*
 *  nokia6100.h
 *
 *  Author: Kestutis Bivainis
 *
 */

#ifndef NOKIA6100_H_
#define NOKIA6100_H_

// Nokia 6100, Nokia 3100
//  8bit RGB 3:3:2 (LUT)
// 12bit RGB 4:4:4 (native)
// 16bit RGB 5:6:5 (dithered)

// Nokia 6030, Nokia 6020
//  8bit RGB 3:3:2 (LUT?)
// 12bit RGB 4:4:4 (interpolated?)
// 16bit RGB 5:6:5 (native)

#define _NOKIA6100
//#define _NOKIA3100
//#define _NOKIA6030
//#define _NOKIA6020

#define _8BITCOLOR 8
//#define _12BITCOLOR 12
//#define _16BITCOLOR 16

#define RESET_PIN 15
#define CLK_PIN 12
#define MOSI_PIN 14

#define MAX_X 132
#define MAX_Y 132
#define START_X 0
#define END_X 131
#define START_Y 0
#define END_Y 131

// *************************************************************************************
// LCD Include File for Philips PCF8833 STN RGB- 132x132x3 Driver
//
// Taken from Philips data sheet Feb 14, 2003
// *************************************************************************************
// Philips PCF8833 LCD controller command codes

#define NOP      0x00 // nop
#define SWRESET  0x01 // software reset
#define BSTROFF  0x02 // booster voltage OFF
#define BSTRON   0x03 // booster voltage ON
#define RDDIDIF  0x04 // read display identification
#define RDDST    0x09 // read display status
#define SLEEPIN  0x10 // sleep in
#define SLEEPOUT 0x11 // sleep out
#define PTLON    0x12 // partial display mode
#define NORON    0x13 // display normal mode
#define INVOFF   0x20 // inversion OFF
#define INVON    0x21 // inversion ON
#define DALO     0x22 // all pixel OFF
#define DAL      0x23 // all pixel ON
#define SETCON   0x25 // write contrast
#define DISPOFF  0x28 // display OFF
#define DISPON   0x29 // display ON
#define CASET    0x2A // column address set
#define PASET    0x2B // page address set
#define RAMWR    0x2C // memory write
#define RGBSET   0x2D // colour set
#define PTLAR    0x30 // partial area
#define VSCRDEF  0x33 // vertical scrolling definition
#define TEOFF    0x34 // test mode
#define TEON     0x35 // test mode
#define MADCTL   0x36 // memory access control
#define SEP      0x37 // vertical scrolling start address
#define IDMOFF   0x38 // idle mode OFF
#define IDMON    0x39 // idle mode ON
#define COLMOD   0x3A // interface pixel format
#define SETVOP   0xB0 // set Vop
#define BRSOFF   0xB4 // bottom row swap
#define BRSON    0xB5 // bottom row swap
#define TRSOFF   0xB6 // top row swap
#define TRSON    0xB7 // top row swap
#define FINVOFF  0xB8 // display control
#define DISCTR   0xB9 // display control
#define DOROFF   0xBA // data order
#define DORON    0xBB // data order
#define TCDFE    0xBD // enable/disable DF temperature compensation
#define TCVOPE   0xBF // enable/disable Vop temp comp
#define EC       0xC0 // internal or external oscillator
#define SETMUL   0xC2 // set multiplication factor
#define TCVOPAB  0xC3 // set TCVOP slopes A and B
#define TCVOPCD  0xC4 // set TCVOP slopes c and d
#define TCDF     0xC5 // set divider frequency
#define DF8COLOR 0xC6 // set divider frequency 8-color mode
#define SETBS    0xC7 // set bias system
#define RDTEMP   0xC8 // temperature read back
#define NLI      0xC9 // n-line inversion
#define RDID1    0xDA // read ID1
#define RDID2    0xDB // read ID2
#define RDID3    0xDC // read ID3

#define COLOR_MODE_8BIT  0x02
#define COLOR_MODE_12BIT 0x03
#define COLOR_MODE_16BIT 0x05

// Booleans
#define NOFILL 0
#define FILL 1

#ifdef _8BITCOLOR

#define BLACK   0x00
#define BLUE    0x03
#define GREEN   0x1C
#define CYAN    0x1F
#define BROWN   0xA9
#define RED     0xE0
#define MAGENTA 0xE3
#define PINK    0xEE
#define ORANGE  0xF4
#define YELLOW  0xFC
#define WHITE   0xFF

#elif defined _12BITCOLOR

#define BLACK       0x000
#define NAVY        0x008
#define BLUE        0x00F
#define TEAL        0x088
#define EMERALD     0x0C5
#define GREEN       0x0F0
#define CYAN        0x0FF
#define SLATE       0x244
#define INDIGO      0x408
#define TURQUOISE   0x4ED
#define OLIVE       0x682
#define MAROON      0x800
#define PURPLE      0x808
#define GRAY        0x888
#define SKYBLUE     0x8CE
#define BROWN       0xB22
#define CRIMSON     0xD13
#define ORCHID      0xD7D
#define RED         0xF00
#define MAGENTA     0xF0F
#define ORANGE      0xF40
#define PINK        0xF6A
#define CORAL       0xF75
#define SALMON      0xF87
#define ORANGE2     0xFA0
#define GOLD        0xFD0
#define YELLOW      0xFF0
#define WHITE       0xFFF

#elif defined _16BITCOLOR

#define BLACK         0x0000
#define NAVY        0x0010
#define DARK_BLUE       0x0011
#define MEDIUM_BLUE     0x0019
#define BLUE         0x001F
#define DARK_GREEN       0x0320
#define GREEN_W3C       0x0400
#define TEAL            0x0410
#define DARK_CYAN       0x0451
#define DEEP_SKY_BLUE     0x05FF
#define DARK_TURQUOISE     0x067A
#define MEDIUM_SPRING_GREEN  0x07D3
#define GREEN        0x07E0
#define LIME_W3C      0x07E0
#define SPRING_GREEN     0x07EF
#define AQUA         0x07FF
#define CYAN         0x07FF
#define MIDNIGHT_BLUE     0x18CE
#define DODGER_BLUE     0x1C9F
#define FOREST_GREEN     0x2444
#define LIGHT_SEA_GREEN   0x2595
#define DARK_SLATE_GRAY   0x2A69
#define SEA_GREEN       0x2C4A
#define LIME_GREEN       0x3666
#define MEDIUM_SEA_GREEN   0x3D8E
#define ROYAL_BLUE       0x435C
#define STEEL_BLUE       0x4416
#define TURQUOISE       0x471A
#define INDIGO         0x4810
#define DARK_SLATE_BLUE   0x49F1
#define MEDIUM_TURQUOISE   0x4E99
#define DARK_OLIVE_GREEN   0x5345
#define CADET_BLUE       0x5CF4
#define CORNFLOWER       0x64BD
#define MEDIUM_AQUAMARINE   0x6675
#define SLATE_BLUE       0x6AD9
#define DIM_GRAY       0x6B4D
#define OLIVE_DRAB       0x6C64
#define SLATE_GRAY       0x6AD9
#define LIGHT_SLATE_GRAY   0x7453
#define MAROON_W3C       0x7800
#define PURPLE_W3C       0x780F
#define MEDIUM_SLATE_BLUE   0x7B5D
#define CHARTREUSE       0x7FE0
#define LAWN_GREEN       0x7FE0
#define AQUAMARINE       0x7FFA
#define OLIVE         0x8400
#define GRAY_W3C       0x8410
#define SKY_BLUE       0x867D
#define LIGHT_SKY_BLUE     0x867F
#define DARK_RED       0x8800
#define DARK_MAGENTA     0x8811
#define BLUE_VIOLET     0x895C
#define SADDLE_BROWN     0x8A22
#define DARK_SEA_GREEN     0x8DF1
#define DARK_VIOLET     0x901A
#define MEDIUM_PURPLE     0x939B
#define LIGHT_GREEN     0x9772
#define DARK_ORCHID     0x9999
#define YELLOW_GREEN     0x9E66
#define PALE_GREEN       0x9FD3
#define PURPLE_X11       0xA11E
#define BROWN         0xA145
#define SIENNA         0xA285
#define DARK_GRAY       0xAD55
#define LIGHT_BLUE       0xAEDC
#define PALE_TURQUOISE     0xAF7D
#define GREEN_YELLOW     0xAFE5
#define FIREBRICK       0xB104
#define MAROON_X11       0xB18C
#define LIGHT_STEEL_BLUE   0xB63B
#define POWDER_BLUE     0xB71C
#define MEDIUM_ORCHID     0xBABA
#define DARK_GOLDENROD     0xBC21
#define ROSY_BROWN       0xBC71
#define DARK_KHAKI       0xBDAD
#define GRAY_X11       0xBDF7
#define MEDIUM_VIOLET_RED   0xC0B0
#define SILVER_W3C       0xC618
#define INDIAN_RED       0xCAEB
#define PERU         0xCC27
#define CHOCOLATE      0xD343
#define TAN         0xD5B1
#define LIGHT_GRAY       0xD69A
#define CRIMSON       0xD8A7
#define PALE_VIOLET_RED   0xDB92
#define ORCHID         0xDB9A
#define PLUM         0xDD1B
#define GOLDENROD       0xDD24
#define BURLYWOOD       0xDDD0
#define THISTLE       0xDDFB
#define GAINSBORO      0xDEFB
#define LAVENDER       0xE73F
#define LIGHT_CYAN       0xE7FF
#define VIOLET         0xEC1D
#define DARK_SALMON     0xECAF
#define PALE_GOLDENROD     xEF55
#define LIGHT_CORAL     0xF410
#define SANDY_BROWN     0xF52C
#define WHEAT         0xF6F6
#define KHAKI         0xF731
#define BEIGE         0xF7B9
#define WHITE_SMOKE     0xF7BE
#define ALICE_BLUE       0xF7DF
#define HONEYDEW       0xF7FE
#define AZURE         0xF7FF
#define MINT_CREAM       0xF7FF
#define RED         0xF800
#define FUCHSIA       0xF81F
#define MAGENTA       0xF81F
#define DEEP_PINK       0xF8B2
#define ORANGE_RED       0xFA20
#define TOMATO         0xFB08
#define HOT_PINK       0xFB56
#define ORANGE         0xFBE0
#define CORAL         0xFBEA
#define SALMON         0xFC0E
#define DARK_ORANGE     0xFC60
#define LIGHT_SALMON     0xFD0F
#define LIGHT_PINK       0xFE19
#define PINK         0xFDB8
#define GOLD         0xFEA8
#define PEACH_PUFF       0xFED7
#define NAVAJO_WHITE     0xFEF5
#define MOCCASIN       0xFF36
#define BISQUE         0xFF38
#define MISTY_ROSE       0xFF3C
#define BLANCHED_ALMOND   0xFF59
#define ANTIQUE_WHITE     0xFF5A
#define PAPAYA_WHIP     0xFF7A
#define LINEN         0xFF9C
#define LAVENDER_BLUSH     0xFF9E
#define OLD_LACE       0xFFBC
#define SEAHSELL       0xFFBD
#define LEMON_CHIFFON     0xFFD9
#define LIGHT_GOLDENROD   0xFFDA
#define CORNSILK       0xFFDB
#define FLORAL_WHITE     0xFFDE
#define GHOST_WHITE     0xFFDF
#define SNOW         0xFFDF
#define YELLOW         0xFFE0
#define LIGHT_YELLOW     0xFFFC
#define IVORY         0xFFFE
#define WHITE         0xFFFF

#endif

#ifdef _NOKIA6100
#define MADCTL_DATA 0xC8
#define SETCON_DATA 0x3A

#elif defined _NOKIA3100
#define MADCTL_DATA 0x80
#define SETCON_DATA 0x3C

#elif defined _NOKIA6030
#define MADCTL_DATA 0x08
#define SETCON_DATA 0x3A

#elif defined _NOKIA6020
#define MADCTL_DATA 0x80
#define SETCON_DATA 0x3C

#endif

void LCDInit(void);
void LCDSetup8BitColor(void);
void LCDSetWindow(int32_t x1, int32_t y1,int32_t x2, int32_t y2);
void LCDPutChar(char c, int32_t x, int32_t y);
void LCDPutStr(char *pString, int32_t x, int32_t y);
void LCDSetPixel(int32_t x, int32_t y, int32_t color);
void LCDRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t color);

#endif /* NOKIA6100_H_ */

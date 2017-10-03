/*********************************************************************
 * original:
This is a library for our Monochrome Nokia 5110 LCD Displays

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/338

These displays use SPI to communicate, 4 or 5 pins are required to  
interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/
/* Modified by Kevin Luedemann
 */
#ifndef _NOKIA_5110_H
#define _NOKIA_5110_H

#include <avr/io.h>
#include "SPI.h"
#include "Output.h"
#include <math.h>
#include <avr/pgmspace.h>
#include "fonts.h"

#define BLACK 1
#define WHITE 0

#define LCDWIDTH 84
#define LCDHEIGHT 48

#define PCD8544_POWERDOWN 0x04
#define PCD8544_ENTRYMODE 0x02
#define PCD8544_EXTENDEDINSTRUCTION 0x01

#define PCD8544_DISPLAYBLANK 0x0
#define PCD8544_DISPLAYNORMAL 0x4
#define PCD8544_DISPLAYALLON 0x1
#define PCD8544_DISPLAYINVERTED 0x5

// H = 0
#define PCD8544_FUNCTIONSET 0x20
#define PCD8544_DISPLAYCONTROL 0x08
#define PCD8544_SETYADDR 0x40
#define PCD8544_SETXADDR 0x80

// H = 1
#define PCD8544_SETTEMP 0x04
#define PCD8544_SETBIAS 0x10
#define PCD8544_SETVOP 0x80

//Zeichendefinitionen
#define charsize 6
#define charhighte 8
#define numbersmalsize 16
#define numbersmalhight 16
#define numberbigsize 16
#define numberbighight 24

class nokia_5110{
public:
    nokia_5110(Output *DC_ob, Output *CS_ob, Output *RST_ob, Spi *spi_ob);
    ~nokia_5110();
    uint8_t *pcd8544_buffer;

    void begin(uint8_t contrast = 40, uint8_t bias = 0x04);

    void command(uint8_t c);
    void data(uint8_t c);

    void setContrast(uint8_t val);
    void clearDisplay(void);
    void display();

    void drawPixel(int16_t x, int16_t y, uint16_t color);
    uint8_t getPixel(int8_t x, int8_t y);

    void drawVLine(uint8_t x, uint8_t y, uint8_t length);
    void drawHLine(uint8_t x, uint8_t y, uint8_t length);
    void drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t fill);
    void draw_line(uint8_t x, uint8_t y, uint8_t length, float phi);
    void draw_ASCI(uint8_t sym,uint8_t x, uint8_t y);
    void draw_number16x16(uint8_t buch, uint8_t x, uint8_t y);
    void analog(uint8_t stunde, uint8_t minute, uint8_t sekunde, uint8_t sekanzeige);
    void drawprogress(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t percent);

private:
    Output *DC;
    Output *CS;
    Output *RST;
    Spi *spi;

    void spiWrite(uint8_t c);
};

#endif

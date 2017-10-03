/*********************************************************************
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
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/

#include <stdlib.h>
#include <string.h>
#include "nokia_5110.h"

// the memory buffer for the LCD

nokia_5110::nokia_5110(Output *DC_ob, Output *CS_ob, Output *RST_ob, Spi *spi_ob){
    DC  = DC_ob;
    CS  = CS_ob;
    RST = RST_ob;
    spi = spi_ob;
    RST->off();
    uint8_t buffer[LCDWIDTH * LCDHEIGHT / 8] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFC, 0xFE, 0xFF, 0xFC, 0xE0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8,
    0xF8, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80, 0xC0, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x7F,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC7, 0xC7, 0x87, 0x8F, 0x9F, 0x9F, 0xFF, 0xFF, 0xFF,
    0xC1, 0xC0, 0xE0, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC, 0xFC, 0xFE, 0xFE, 0xFE,
    0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xE0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x80, 0xC0, 0xE0, 0xF1, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x1F, 0x0F, 0x0F, 0x87,
    0xE7, 0xFF, 0xFF, 0xFF, 0x1F, 0x1F, 0x3F, 0xF9, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xFD, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xF0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
    0x7E, 0x3F, 0x3F, 0x0F, 0x1F, 0xFF, 0xFF, 0xFF, 0xFC, 0xF0, 0xE0, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFC, 0xF0, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01,
    0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0F, 0x1F, 0x3F, 0x7F, 0x7F,
    0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x1F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    };
    pcd8544_buffer = (uint8_t *) malloc((LCDWIDTH * LCDHEIGHT / 8)*sizeof(uint8_t));
    for(uint16_t i=0;i<LCDWIDTH * LCDHEIGHT / 8;i++){pcd8544_buffer[i]=buffer[i];}
    begin(60,4);
}

nokia_5110::~nokia_5110(){
    free(pcd8544_buffer);
}

// the most basic function, set a single pixel
void nokia_5110::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
    return;
  // x is which column
  if (color) 
    pcd8544_buffer[x+ (y/8)*LCDWIDTH] |= (1<<y%8);  
  else
    pcd8544_buffer[x+ (y/8)*LCDWIDTH] &= ~(1<<y%8); 
}

// the most basic function, get a single pixel
uint8_t nokia_5110::getPixel(int8_t x, int8_t y) {
  if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
    return 0;

  return (pcd8544_buffer[x+ (y/8)*LCDWIDTH] >> (y%8)) & 0x1;  
}

void nokia_5110::drawVLine(uint8_t x, uint8_t y, uint8_t length){
	for (uint8_t i = 0; i < length; ++i) {
		drawPixel(x,i+y,BLACK);
	}
}
void nokia_5110::drawHLine(uint8_t x, uint8_t y, uint8_t length){
	for (uint8_t i = 0; i < length; ++i) {
		drawPixel(i+x,y,BLACK);
	}
}
void nokia_5110::drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t fill){
	uint8_t length = x2 - x1 + 1;
	uint8_t height = y2 - y1;
	if (!fill) {
		drawHLine(x1,y1,length);
		drawHLine(x1,y2,length);
		drawVLine(x1,y1,height);
		drawVLine(x2,y1,height);
	}
	else {
		for (int x = 0; x < length; ++x) {
			for (int y = 0; y <= height; ++y) {
				drawPixel(x1+x,y+y1,BLACK);
			}
		}
	}
}
void nokia_5110::draw_line(uint8_t x, uint8_t y, uint8_t length, float phi){
	//phi*=M_PI/180.0;
	uint8_t pos_x=0, pos_y=0;
	for (uint8_t i=0;i<length;i++)
	{
		pos_x=(uint8_t) (-i*cos(phi)+x);
		pos_y=(uint8_t) (i*sin(phi)+y);
		drawPixel(pos_x,pos_y,BLACK);
	}
}
void nokia_5110::draw_ASCI(uint8_t sym,uint8_t x, uint8_t y){
	if(sym!=' ' && sym>=33){
		sym-=33;
		uint16_t symbol;
		for(uint8_t i = 0; i < 5; i++){
			symbol = pgm_read_byte(&font5x8[sym][i]);
			symbol = symbol << y%8;
			pcd8544_buffer[x+(y/8)*LCDWIDTH+i] |=(uint8_t) (symbol & 0x00FF);
			if(((symbol&0xFF00)>>8)==BLACK){
				pcd8544_buffer[x+(y/8+1)*LCDWIDTH+i] |= BLACK;
			}
			else {
				pcd8544_buffer[x+(y/8+1)*LCDWIDTH+i] |= (uint8_t) ((symbol & 0xFF00)>>8);
			}
		}
	}
	else{
		for(uint8_t i = 0; i < 7; i++){
			pcd8544_buffer[x+(y/8)*LCDWIDTH+i] |= BLACK;
		}
	}
}
void nokia_5110::draw_number16x16(uint8_t number, uint8_t x, uint8_t y){
	uint16_t symbol;
	if(number>=0 && number<10){
		for(uint8_t i = 0; i < 32;i++){
			symbol = pgm_read_byte(&digits16x16[number][i]);
			symbol = symbol << y%8;
			if(i<16){
				pcd8544_buffer[x+(y/8)*LCDWIDTH+i] |=(uint8_t) (symbol & 0x00FF);
				if(((symbol&0xFF00)>>8)==0){
					pcd8544_buffer[x+(y/8+1)*LCDWIDTH+i] |= 0;
				}
				else {
					pcd8544_buffer[x+(y/8+1)*LCDWIDTH+i] |= (uint8_t) ((symbol & 0xFF00)>>8);
				}
			}
			else {
				pcd8544_buffer[x+(y/8+1)*LCDWIDTH+(i-16)] |= (uint8_t) (symbol & 0x00FF);
				if(((symbol&0xFF00)>>8)==0){
					pcd8544_buffer[x+(y/8+2)*LCDWIDTH+(i-16)] |= 0;
				}
				else {
					pcd8544_buffer[x+(y/8+2)*LCDWIDTH+(i-16)] |= (uint8_t) ((symbol & 0xFF00)>>8);
				}
			}
		}
	}
}
void nokia_5110::analog(uint8_t stunde, uint8_t minute, uint8_t sekunde, uint8_t sekanzeige){
	uint8_t r=LCDHEIGHT/2-charhighte;
	uint8_t Px=LCDWIDTH/2-1;
	uint8_t Py=LCDHEIGHT/2-1;
	uint8_t x=0,y=0;
	float winkel=0;
	
	if (sekanzeige)
	{
		//Berechnen des Sekundenzeigers
		for(uint8_t i=0;i<r;i++){
			winkel=(sekunde*6.0)*M_PI/180.0-M_PI/2.0;
			x=(uint8_t) i*cos(winkel)+Px;
			y=(uint8_t) i*sin(winkel)+Py;

			drawPixel(x,y,BLACK);
		}

		r=r-2;
		//Berechnen des Minutenzeigers
		for(uint8_t i=0;i<r;i++){
			winkel=(minute*6.0)*M_PI/180.0-M_PI/2.0;
			x=(uint8_t) i*cos(winkel)+Px;
			y=(uint8_t) i*sin(winkel)+Py;

			drawPixel(x,y,BLACK);
		}

		r=r-2;
		//Berechnen des Stundenzeigers
		for(uint8_t i=0;i<r;i++){
			winkel=(stunde*30.0+minute/2.0)*M_PI/180.0-M_PI/2.0;
			x= (uint8_t) i*cos(winkel) +Px;
			y= (uint8_t) i*sin(winkel) +Py;

			drawPixel(x,y,BLACK);
		}
	}
	else{
		r=r;
		//Berechnen des Minutenzeigers
		for(uint8_t i=0;i<r;i++){
			winkel=(minute*6.0)*M_PI/180.0-M_PI/2.0;
			x=(uint8_t) i*cos(winkel)+Px;
			y=(uint8_t) i*sin(winkel)+Py;

			drawPixel(x,y,BLACK);
		}

		r=r-5;
		//Berechnen des Stundenzeigers
		for(uint8_t i=0;i<r;i++){
			winkel=(stunde*30.0+minute/2.0)*M_PI/180.0-M_PI/2.0;
			x= (uint8_t) i*cos(winkel) +Px;
			y= (uint8_t) i*sin(winkel) +Py;

			drawPixel(x,y,BLACK);
		}
	}
	

	//Anzeige der Zahlen
    r=LCDHEIGHT/2;
	draw_ASCI('1',Px-charsize,1);
	draw_ASCI('2',Px,1);

	draw_ASCI('3',Px+r-charsize,Py-charhighte/2);

	draw_ASCI('6',Px-charsize/2,Py+r-charhighte);

	draw_ASCI('9',Px-r+charsize/2,Py-charhighte/2);

    r=LCDHEIGHT/2-charhighte*3/4;
	//anzeige des Rings fuer die Restlichen Stunden Zahlen
	//1
	drawPixel(r*cos((   30.0)*M_PI/180.0-M_PI/2.0)+Px,r*sin((   30.0)*M_PI/180.0-M_PI/2.0)+Py,BLACK);
	//2
	drawPixel(r*cos(( 2*30.0)*M_PI/180.0-M_PI/2.0)+Px,r*sin(( 2*30.0)*M_PI/180.0-M_PI/2.0)+Py,BLACK);
	//4
	drawPixel(r*cos(( 4*30.0)*M_PI/180.0-M_PI/2.0)+Px,r*sin(( 4*30.0)*M_PI/180.0-M_PI/2.0)+Py,BLACK);
	//5
	drawPixel(r*cos(( 5*30.0)*M_PI/180.0-M_PI/2.0)+Px,r*sin(( 5*30.0)*M_PI/180.0-M_PI/2.0)+Py,BLACK);
	//7
	drawPixel(r*cos(( 7*30.0)*M_PI/180.0-M_PI/2.0)+Px,r*sin(( 7*30.0)*M_PI/180.0-M_PI/2.0)+Py,BLACK);
	//8
	drawPixel(r*cos(( 8*30.0)*M_PI/180.0-M_PI/2.0)+Px,r*sin(( 8*30.0)*M_PI/180.0-M_PI/2.0)+Py,BLACK);
	//10
	drawPixel(r*cos((10*30.0)*M_PI/180.0-M_PI/2.0)+Px,r*sin((10*30.0)*M_PI/180.0-M_PI/2.0)+Py,BLACK);
	//11
	drawPixel(r*cos((11*30.0)*M_PI/180.0-M_PI/2.0)+Px,r*sin((11*30.0)*M_PI/180.0-M_PI/2.0)+Py,BLACK);
}

void nokia_5110::drawprogress(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t percent){
	uint8_t length = x2 - x1 + 1;
	uint8_t height = y2 - y1;
    if(percent>100){percent=100;}
    uint8_t holdl = (uint8_t)(((float)length/100.)*percent);
    for (int x = 0; x < holdl; ++x) {
        for (int y = 0; y <= height; ++y) {
            drawPixel(x1+x,y+y1,BLACK);
        }
    }

    drawHLine(x1,y1,length);
    drawHLine(x1,y2,length);
    drawVLine(x1,y1,height);
    drawVLine(x2,y1,height);
}

void nokia_5110::begin(uint8_t contrast, uint8_t bias) {
    RST->on();

    // get into the EXTENDED mode!
    command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );

    // LCD bias select (4 is optimal?)
    command(PCD8544_SETBIAS | bias);

    // set VOP
    if (contrast > 0x7f)
    contrast = 0x7f;

    command( PCD8544_SETVOP | contrast); // Experimentally determined


    // normal mode
    command(PCD8544_FUNCTIONSET);

    // Set display to Normal
    command(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);

    // Push out pcd8544_buffer to the Display (will show the AFI logo)
    //DC->on();
    display();
}

inline void nokia_5110::spiWrite(uint8_t d) {
    spi->transfer(d);
}

void nokia_5110::command(uint8_t c) {
    DC->off();
    CS->off();
    spiWrite(c);
    CS->on();
}

void nokia_5110::data(uint8_t c) {
    DC->on();
    CS->off();
    spiWrite(c);
    CS->on();
}

void nokia_5110::setContrast(uint8_t val) {
    if (val > 0x7f) {
        val = 0x7f;
    }
    command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );
    command( PCD8544_SETVOP | val); 
    command(PCD8544_FUNCTIONSET);
}

void nokia_5110::display(void) {
  uint8_t col, maxcol, p;
  
  for(p = 0; p < 6; p++) {
    command(PCD8544_SETYADDR | p);

    // start at the beginning of the row
    col = 0;
    maxcol = LCDWIDTH-1;

    command(PCD8544_SETXADDR | col);

    DC->on();
    CS->off();
    for(; col <= maxcol; col++) {
      spiWrite(pcd8544_buffer[(LCDWIDTH*p)+col]);
    }
    CS->on();
  }
  command(PCD8544_SETYADDR );  // no idea why this is necessary but it is to finish the last byte?
}

// clear everything
void nokia_5110::clearDisplay(void) {
  memset(pcd8544_buffer, 0, LCDWIDTH*LCDHEIGHT/8);
}

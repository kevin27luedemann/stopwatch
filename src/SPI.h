/*
 * Created by Kevin Luedemann
 * 09.09.2017 12:27
 */

#ifndef __SPI_H__
#define __SPI_H__

#include <avr/io.h>
#include <stdlib.h>

class Spi
{
public:
	Spi();
	~Spi();
	void transfer(uint8_t d);
};

#endif

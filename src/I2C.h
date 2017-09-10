/* 
* I2C.h
*
* Created: 02.09.2015 12:06:52
* Author: kevin
*/


#ifndef __I2C_H__
#define __I2C_H__

#define I2C_WRITE	0
#define I2C_READ	1

#include <avr/io.h>
#include <stdlib.h>

class I2c
{
#include <util/twi.h>
//variables
public:
protected:
private:

//functions
public:
	I2c();
	~I2c();
	void init();
	void twi_start();
	void twi_stop();
	void twi_ack();
	void twi_nack();
	void twi_write(uint8_t data);
	uint8_t twi_read(uint8_t ack);
protected:
private:

}; //I2C

#endif //__I2C_H__

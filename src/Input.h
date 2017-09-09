/* 
* Input.h
*
* Created: 17.08.2017
* Author: kevin
*/


#ifndef __INPUT_H__
#define __INPUT_H__

#include <avr/io.h>
#include <stdio.h>

class Input
{
//variables
public:
protected:
private:
	volatile uint8_t *PORT;
	volatile uint8_t *DDR;
	volatile uint8_t *PIN;
	uint8_t pin_num;

//functions
public:
	Input(volatile uint8_t *IDDR, volatile uint8_t *IPORT, volatile uint8_t *IPIN, uint8_t PIN_NUMMER, uint8_t pullup);
	~Input();
	uint8_t ison();
	uint8_t pressed();
protected:
private:
	Input( const Input &c );
	Input& operator=( const Input &c );
	uint8_t is_down;

}; //Input

#endif //__INPUT_H

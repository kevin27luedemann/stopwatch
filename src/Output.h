/* 
* Output.h
*
* Created: 09.09.2015 16:01:30
* Author: kevin
*/


#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include <avr/io.h>
#include <stdio.h>

class Output
{
//variables
public:
protected:
private:
	volatile uint8_t *PORT;
	volatile uint8_t *DDR;
	uint8_t Pin;

//functions
public:
	Output(volatile uint8_t *ODDR, volatile uint8_t *OPORT, uint8_t PIN_NUMMER);
	~Output();
	void on();
	void off();
	void toggle();
	uint8_t ison();
protected:
private:
	Output( const Output &c );
	Output& operator=( const Output &c );

}; //Output

#endif //__OUTPUT_H__

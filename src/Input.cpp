/* 
* Input.cpp
*
* Created: 09.09.2015 16:01:30
* Author: kevin
*/


#include "Input.h"
#ifndef F_CPU
#define F_CPU 8000000
#endif
#include <util/delay.h>

// default constructor
Input::Input(volatile uint8_t *IDDR, volatile uint8_t *IPORT, volatile uint8_t *IPIN, uint8_t PIN_NUMMER, uint8_t pullup)
{
    DDR     = IDDR;
    PORT    = IPORT;
    PIN     = IPIN;
    pin_num = PIN_NUMMER;
    if(pullup){*PORT |= (1<<pin_num);}
    is_down = 0;
} //Input

// default destructor
Input::~Input()
{
    *PORT &= ~(1<<pin_num);
} //~Input

uint8_t Input::ison(){
	uint8_t returnparam=0;
    if (!(*PIN&(1<<pin_num)))
    {
        returnparam = 1;
    }
	return returnparam;
}

uint8_t Input::pressed(){
	uint8_t returnparam=0;
    if (!(*PIN&(1<<pin_num)))
    {
        is_down = 1;
    }
    else if ((*PIN&(1<<pin_num)) && is_down==1){returnparam = 1; is_down = 0;}
	return returnparam;
}


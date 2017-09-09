/* 
* Output.cpp
*
* Created: 09.09.2015 16:01:30
* Author: kevin
*/


#include "Output.h"

// default constructor
Output::Output(volatile uint8_t *ODDR, volatile uint8_t *OPORT, uint8_t PIN_NUMMER)
{
	PORT    = OPORT;
    DDR     = ODDR;
	Pin     = PIN_NUMMER;
	*DDR    |= (1<<Pin);
} //Output

// default destructor
Output::~Output()
{
	*DDR    &= ~(1<<Pin);
} //~Output

void Output::on(){
    *PORT |= (1<<Pin);
}

uint8_t Output::ison(){
	uint8_t returnparam=0;
    if ((*PORT&(1<<Pin)))
    {
        returnparam = 1;
    }
	return returnparam;
}

void Output::off(){
    *PORT &= ~(1<<Pin);
}

void Output::toggle(){
    if ((*PORT&(1<<Pin)))
    {
        off();
    }
    else{
        on();
    }
}

//this example runs with 2 shift registers

#define F_CPU 8000000
#define BAUD 9600

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "Input.h"
#include "Output.h"
#include "com.h"
#include "SPI.h"
#include "nokia_5110.h"

Output LED(&DDRB,&PORTB,0);
Output CS(&DDRB,&PORTB,2);
Output DC(&DDRD,&PORTD,5);
Output RST(&DDRD,&PORTD,3);
Spi spi;
nokia_5110 nok(&DC,&CS,&RST,&spi);

Input taster(&DDRD,&PORTD,&PIND,6,true);

#define DISP_UPDATE 0
uint8_t flag_reg;

ISR(TIMER1_COMPA_vect){
    flag_reg |= (1<<DISP_UPDATE);
}

ISR(USART_RX_vect){
    uint8_t temp = uart_getc();
}

int main(void) {

    ACSR = (1<<ACD);
    //uart_init();
    flag_reg = 0;

    //seconds timer
    TIMSK1 = (1 << OCIE1A);
    OCR1A  = 31250;
    TCCR1B = (1 << WGM12) | (1 << CS12); //CTC Mode, Presc 256
    sei();


    //nok.display();
	while(true) 
    {
        if((flag_reg&(1<<DISP_UPDATE))){LED.toggle();flag_reg&=~(1<<DISP_UPDATE);}
        set_sleep_mode(SLEEP_MODE_IDLE);
        //set_sleep_mode(SLEEP_MODE_PWR_SAVE);
        sleep_mode();
	}

}


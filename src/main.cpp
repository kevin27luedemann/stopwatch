/*
 * Main Program
 *
 * created by Kevin Luedemann
 * 02.10.2017
 */

#define F_CPU 3686400
#define F_CPU_KHZ 3686.4
#define R1 46.5
#define R2 9.96
#define TEILER R2/(R1+R2)
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
#include "I2C.h"
#include "ds3231.h"
//define new and delete operator
void * operator new(size_t size)
{
	return malloc(size);
}
void operator delete(void * ptr)
{
	free(ptr);
}


ds3231 rtc;

Output CS(&DDRB,&PORTB,2);
Output DC(&DDRB,&PORTB,1);
Output RST(&DDRD,&PORTD,7);
Output BL(&DDRD,&PORTD,6);
Input RTSW(&DDRD,&PORTD,&PIND,5,true);
Input RTDT(&DDRD,&PORTD,&PIND,4,true);
Input RTCLK(&DDRD,&PORTD,&PIND,3,true);
Input SQM(&DDRB,&PORTB,&PINB,0,true);
Input taster(&DDRC,&PORTC,&PINC,3,true);
Input STR(&DDRC,&PORTC,&PINC,2,true);
Spi spi;
nokia_5110 nok(&DC,&CS,&RST,&spi);
Input STW(&DDRD,&PORTD,&PIND,2,true);

uint16_t flag_reg;
#define DISP_UPDATE     0
#define BACKLIGHT       1
#define INCREMENT       2
#define DECREMENT       3
#define BTN_PRESSED     4
#define STOPWATCH       5
#define STWRESET        6
#define TIME_INC        7
#define CLOCK_TICK      8
#define CLORUNNING      9

uint8_t brightnes;
float batt;
uint8_t position;

void init();
void update_disp();
void update_disp2();
void nachti();
void blpwm(uint8_t on);
float get_voltage();

#define numberofpages 5
#include "Monitor.h"
#include "INT_kernals.h"

int main(void) {
    init();

    monitor* mon[numberofpages+1] = {
                new watch(&nok,&rtc),
                new stop_watch(&nok,&rtc),
                new counter(&nok,&rtc),
                new brightnes_settings(&nok,&rtc),
                new blank(&nok,&rtc),
                new menue(&nok,&rtc)
                };

    mon[position]->draw();

	while(true) 
    {
        if(flag_reg&(1<<INCREMENT))  {mon[position]->inc();flag_reg&=~(1<<INCREMENT);}
        if(flag_reg&(1<<DECREMENT))  {mon[position]->dec();flag_reg&=~(1<<DECREMENT);}
        if(flag_reg&(1<<BTN_PRESSED)){mon[position]->btn();flag_reg&=~(1<<BTN_PRESSED);}

        if(flag_reg&(1<<STOPWATCH)){mon[position]->STWbtn();flag_reg&=~(1<<STOPWATCH);}
        if(flag_reg&(1<<STWRESET)){mon[position]->STRbtn();flag_reg&=~(1<<STWRESET);}

        if((flag_reg&(1<<TIME_INC))){mon[position]->timer();flag_reg&=~(1<<TIME_INC);}
        if(flag_reg&(1<<CLOCK_TICK)){rtc.get();flag_reg&=~(1<<CLOCK_TICK);}

        if((flag_reg&(1<<DISP_UPDATE))){mon[position]->draw();flag_reg&=~(1<<DISP_UPDATE);}
        nachti();
	}
    return 0;
}

void init(){
    ACSR = (1<<ACD);
    //uart_init();
    flag_reg = 0;

    //seconds timer
    TIMSK1 = (1 << OCIE1A);
    OCR1A  = 57599;
    TCCR1B = (1 << WGM12) | (1<<CS11) | (1<<CS10); //CTC Mode

    //Pin change interrupts
    PCMSK0 |= (1<<PCINT0 );
    PCMSK1 |= (1<<PCINT11) | (1<<PCINT10);
    PCMSK2 |= (1<<PCINT21);
    PCICR  |= (1<<PCIE1) | (1<<PCIE2) | (1<<PCIE0);

    //interupt detection
    EICRA |= (1<<ISC11);// | (1<<ISC10);
    EICRA |= (1<<ISC01);// | (1<<ISC00);
    EIMSK |= (1<<INT1) | (1<<INT0);

    //fast PWM for BL
    //OCR0A  = 128;
    brightnes=100;
    blpwm(false);
    OCR0A = (uint8_t)((float)brightnes*2.55);
    //blpwm(true);
    //flag_reg |= (1<<BACKLIGHT);
    sei();
    //position = numberofpages;
    position = 0;
}

void blpwm(uint8_t on){
    if(on){
        TCCR0A = (1<<COM0A1) | (1<<WGM01) | (1<<WGM00);
        TCCR0B = (1<<CS02);
    }
    else{
        TCCR0B = 0;
        TCCR0A = 0;
        BL.off();
    }
}

void nachti(){
    if(flag_reg&(1<<BACKLIGHT) || flag_reg&(1<<CLORUNNING)){
        set_sleep_mode(SLEEP_MODE_IDLE);
    }
    else{
        //set_sleep_mode(SLEEP_MODE_PWR_SAVE);
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        //set_sleep_mode(SLEEP_MODE_IDLE);
    }
    sleep_mode();
}

float get_voltage(){
    ADMUX = (1<<REFS1) | (1<<REFS0);
    ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADSC);
    while(ADCSRA&(1<<ADSC));
    ADCSRA |= (1<<ADSC);
    while(ADCSRA&(1<<ADSC));
    uint16_t val = (uint16_t)ADCL;
    val += (ADCH<<8);

    float volt = (float)val/1024.;
    ADCSRA = 0;
    return volt*1.1;
}

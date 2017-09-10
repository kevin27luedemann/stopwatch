//this example runs with 2 shift registers

#define F_CPU 3686400
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
Output DC(&DDRB,&PORTB,1);
Output RST(&DDRD,&PORTD,7);
Output BL(&DDRD,&PORTD,6); //later use as PWM
Input RTSW(&DDRD,&PORTD,&PIND,5,true);
Input RTDT(&DDRD,&PORTD,&PIND,4,false);
Input RTCLK(&DDRD,&PORTD,&PIND,3,false);
Spi spi;
nokia_5110 nok(&DC,&CS,&RST,&spi);

Input taster(&DDRD,&PORTD,&PIND,2,true);

#define DISP_UPDATE 0
#define INCREMENT 1
#define DECREMENT 2
#define BACKLIGHT 3
uint8_t flag_reg;

uint8_t sekunden;

ISR(TIMER1_COMPA_vect){
    flag_reg |= (1<<DISP_UPDATE);
}

ISR(USART_RX_vect){
    uint8_t temp = uart_getc();
}

ISR(INT1_vect){
    if(RTDT.ison()){
        //if(sekunden<100){sekunden++;}
        flag_reg |= (1<<INCREMENT);
    }
    else{
        //if(sekunden<=100 && sekunden>0){sekunden--;}
        flag_reg |= (1<<DECREMENT);
    }
    flag_reg |= (1<<DISP_UPDATE);
}

ISR(INT0_vect){
    if(flag_reg&(1<<BACKLIGHT)){flag_reg&=~(1<<BACKLIGHT);}
    else{flag_reg|=(1<<BACKLIGHT);}
    flag_reg |= (1<<DISP_UPDATE);
}

void update_disp();
void nachti();
void blpwm(uint8_t on);

int main(void) {

    ACSR = (1<<ACD);
    //uart_init();
    flag_reg = 0;

    //seconds timer
    TIMSK1 = (1 << OCIE1A);
    OCR1A  = 14400;
    TCCR1B = (1 << WGM12) | (1 << CS12); //CTC Mode, Presc 256

    //interupt detection
    EICRA |= (1<<ISC11);// | (1<<ISC10);
    EICRA |= (1<<ISC01) | (1<<ISC00);
    EIMSK |= (1<<INT1) | (1<<INT0);

    //fast PWM for BL
    OCR0A  = 128;
    sekunden=50;
    sei();

    update_disp();

	while(true) 
    {
        if(RTSW.ison()){LED.on();}
        else{LED.off();}

        if((flag_reg&(1<<BACKLIGHT))&&!(TCCR0B&(1<<CS02))){blpwm(1);}
        else if(!(flag_reg&(1<<BACKLIGHT))&&(TCCR0B&(1<<CS02))){blpwm(0);}

        if(flag_reg&(1<<INCREMENT)){if(sekunden<100){sekunden++;}flag_reg&=~(1<<INCREMENT);}
        else if(flag_reg&(1<<DECREMENT)){if(sekunden<=100&&sekunden>0){sekunden--;}flag_reg&=~(1<<DECREMENT);}

        if((flag_reg&(1<<DISP_UPDATE))){update_disp();flag_reg&=~(1<<DISP_UPDATE);}

        nachti();
	}

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
    if(flag_reg&(1<<BACKLIGHT)){
        set_sleep_mode(SLEEP_MODE_IDLE);
    }
    else{
        set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    }
    sleep_mode();
}

void update_disp(){
    if(flag_reg&(1<<BACKLIGHT)){
        if(sekunden>100){sekunden=100;}
        OCR0A = (uint8_t)((float)sekunden*2.55);
    }
    nok.clearDisplay();
    //nok.analog(10,25,sekunden,true);
    if(sekunden<0){
        nok.draw_ASCI('-',0,0);
        nok.draw_ASCI((-sekunden/100)%10+'0',1*charsize,0);
        nok.draw_ASCI((-sekunden/10)%10+'0' ,2*charsize,0);
        nok.draw_ASCI( -sekunden%10+'0'     ,3*charsize,0);
    }
    else{
        nok.draw_ASCI('+',0,0);
        nok.draw_ASCI((sekunden/100)%10+'0',1*charsize,0);
        nok.draw_ASCI((sekunden/10)%10+'0' ,2*charsize,0);
        nok.draw_ASCI( sekunden%10+'0'     ,3*charsize,0);
    }
    nok.drawprogress(charsize*2,charhighte*2,LCDWIDTH-charsize,charhighte*3,sekunden);
    nok.display();
}

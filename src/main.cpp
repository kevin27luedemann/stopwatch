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
#include "I2C.h"
#include "ds3231.h"

ds3231 rtc;

//Output LED(&DDRB,&PORTB,0);
Output CS(&DDRB,&PORTB,2);
Output DC(&DDRB,&PORTB,1);
Output RST(&DDRD,&PORTD,7);
Output BL(&DDRD,&PORTD,6); //later use as PWM
Input RTSW(&DDRD,&PORTD,&PIND,5,true);
Input RTDT(&DDRD,&PORTD,&PIND,4,true);
Input RTCLK(&DDRD,&PORTD,&PIND,3,true);
Input SQM(&DDRB,&PORTB,&PINB,0,true);
Input STW(&DDRC,&PORTC,&PINC,3,true);
Spi spi;
nokia_5110 nok(&DC,&CS,&RST,&spi);

Input taster(&DDRD,&PORTD,&PIND,2,true);

#define DISP_UPDATE 0
#define INCREMENT 1
#define DECREMENT 2
#define BACKLIGHT 3
#define STOPWATCH 4
uint8_t flag_reg;

uint8_t brightnes;
uint32_t milis;

ISR(TIMER1_COMPA_vect){
    flag_reg |= (1<<DISP_UPDATE);
    milis+=10;
}

ISR(USART_RX_vect){
    uint8_t temp = uart_getc();
}

ISR(INT1_vect){
    if(RTDT.ison()){
        //if(brightnes<100){brightnes++;}
        flag_reg |= (1<<INCREMENT);
    }
    else{
        //if(brightnes<=100 && brightnes>0){brightnes--;}
        flag_reg |= (1<<DECREMENT);
    }
    flag_reg |= (1<<DISP_UPDATE);
}

ISR(INT0_vect){
    if(flag_reg&(1<<BACKLIGHT)){flag_reg&=~(1<<BACKLIGHT);}
    else{flag_reg|=(1<<BACKLIGHT);}
    flag_reg |= (1<<DISP_UPDATE);
}

ISR(PCINT0_vect){
    flag_reg |= (1<<DISP_UPDATE);
}

ISR(PCINT2_vect){
    milis = 0;
}

ISR(PCINT1_vect){
    if(STW.ison()){
        flag_reg |= (1<<STOPWATCH);
        //TCCR1B |= (1<<CS10) | (1<<CS11);
        TCCR1B |= (1<<CS10);
    }
    else{
        flag_reg &= ~(1<<STOPWATCH);
        TCNT1   = 0;
        //TCCR1B &= ~((1<<CS10) | (1<<CS11));
        TCCR1B &= ~((1<<CS10));
        TIFR1 |= ((1<<OCF1A) | (1<<TOV1));
    }
}

void update_disp();
void nachti();
void blpwm(uint8_t on);

int main(void) {

    ACSR = (1<<ACD);
    //uart_init();
    flag_reg = 0;
    milis = 0;

    //seconds timer
    TIMSK1 = (1 << OCIE1A);
    OCR1A  = 36864;
    TCCR1B = (1 << WGM12); //CTC Mode
    PCMSK0 |= (1<<PCINT0 );
    PCMSK1 |= (1<<PCINT11);
    PCMSK2 |= (1<<PCINT21);
    PCICR  |= (1<<PCIE1) | (1<<PCIE2) | (1<<PCIE0);

    //interupt detection
    EICRA |= (1<<ISC11);// | (1<<ISC10);
    EICRA |= (1<<ISC01) | (1<<ISC00);
    EIMSK |= (1<<INT1) | (1<<INT0);

    //fast PWM for BL
    OCR0A  = 128;
    brightnes=50;
    sei();

    update_disp();

	while(true) 
    {

        if((flag_reg&(1<<BACKLIGHT))&&!(TCCR0B&(1<<CS02))){blpwm(1);}
        else if(!(flag_reg&(1<<BACKLIGHT))&&(TCCR0B&(1<<CS02))){blpwm(0);}

        if(flag_reg&(1<<INCREMENT)){if(brightnes<99){brightnes+=2;}flag_reg&=~(1<<INCREMENT);}
        else if(flag_reg&(1<<DECREMENT)){if(brightnes<=100&&brightnes>1){brightnes-=2;}flag_reg&=~(1<<DECREMENT);}

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
    if(flag_reg&(1<<BACKLIGHT) || flag_reg&(1<<STOPWATCH)){
        set_sleep_mode(SLEEP_MODE_IDLE);
    }
    else{
        set_sleep_mode(SLEEP_MODE_PWR_SAVE);
        //set_sleep_mode(SLEEP_MODE_IDLE);
    }
    sleep_mode();
}

void update_disp(){
    if(flag_reg&(1<<BACKLIGHT)){
        if(brightnes>100){brightnes=100;}
        OCR0A = (uint8_t)((float)brightnes*2.55);
    }
    nok.clearDisplay();
    rtc.get();
    nok.draw_ASCI('0'+rtc.t.hour/10%10,0*charsize,0);
    nok.draw_ASCI('0'+rtc.t.hour%10   ,1*charsize,0);
    nok.draw_ASCI(':'                 ,2*charsize,0);
    nok.draw_ASCI('0'+rtc.t.min/10%10 ,3*charsize,0);
    nok.draw_ASCI('0'+rtc.t.min%10    ,4*charsize,0);
    nok.draw_ASCI(':'                 ,5*charsize,0);
    nok.draw_ASCI('0'+rtc.t.sec/10%10 ,6*charsize,0);
    nok.draw_ASCI('0'+rtc.t.sec%10    ,7*charsize,0);

    nok.draw_ASCI('0'+rtc.t.mday/10%10   ,LCDWIDTH-8*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI('0'+rtc.t.mday%10      ,LCDWIDTH-7*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI(':'                    ,LCDWIDTH-6*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI('0'+rtc.t.mon/10%10    ,LCDWIDTH-5*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI('0'+rtc.t.mon%10       ,LCDWIDTH-4*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI(':'                    ,LCDWIDTH-3*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI('0'+rtc.t.year_s/10%10 ,LCDWIDTH-2*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI('0'+rtc.t.year_s%10    ,LCDWIDTH-1*charsize,LCDHEIGHT-charhighte);
    nok.drawprogress(0,charhighte,LCDWIDTH-1,charhighte*2,brightnes);

    uint8_t sek = (milis/1000);
    uint8_t min = sek/60;
            sek = sek%60;

    nok.draw_ASCI('0'+(min/10        )%10,0*charsize,3*charhighte);
    nok.draw_ASCI('0'+(min           )%10,1*charsize,3*charhighte);
    nok.draw_ASCI(':'                    ,2*charsize,3*charhighte);
    nok.draw_ASCI('0'+(sek/10       )%10,3*charsize,3*charhighte);
    nok.draw_ASCI('0'+(sek          )%10,4*charsize,3*charhighte);
    nok.draw_ASCI('.'                   ,5*charsize,3*charhighte);
    nok.draw_ASCI('0'+(milis/100    )%10,6*charsize,3*charhighte);
    nok.draw_ASCI('0'+(milis/10     )%10,7*charsize,3*charhighte);
    nok.display();
}

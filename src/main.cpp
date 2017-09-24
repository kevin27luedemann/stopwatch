//this example runs with 2 shift registers

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

ds3231 rtc;

Output CS(&DDRB,&PORTB,2);
Output DC(&DDRB,&PORTB,1);
Output RST(&DDRD,&PORTD,7);
Output BL(&DDRD,&PORTD,6);
Input RTSW(&DDRD,&PORTD,&PIND,5,true);
Input RTDT(&DDRD,&PORTD,&PIND,4,true);
Input RTCLK(&DDRD,&PORTD,&PIND,3,true);
Input SQM(&DDRB,&PORTB,&PINB,0,true);
Input STW(&DDRC,&PORTC,&PINC,3,true);
Input STR(&DDRC,&PORTC,&PINC,2,true);
Spi spi;
nokia_5110 nok(&DC,&CS,&RST,&spi);
Input taster(&DDRD,&PORTD,&PIND,2,true);

uint16_t flag_reg;
#define DISP_UPDATE     0
#define DISP_UPDATE2    1
#define BACKLIGHT       2
#define INCREMENT       3
#define DECREMENT       4
#define TIME_INC        5
#define STOPWATCH       6

uint8_t brightnes;
uint8_t seconds, minutes;
uint16_t millise;

void init();
void update_disp();
void update_disp2();
void nachti();
void blpwm(uint8_t on);
float get_voltage();

#include "INT_kernals.h"

int main(void) {

    init();

	while(true) 
    {

        if(flag_reg&(1<<INCREMENT)){if(brightnes<99){brightnes+=2;}flag_reg&=~(1<<INCREMENT);}
        else if(flag_reg&(1<<DECREMENT)){if(brightnes<=100&&brightnes>1){brightnes-=2;}flag_reg&=~(1<<DECREMENT);}

        if((flag_reg&(1<<TIME_INC))){
            seconds++;
            if(seconds>=60){minutes++;seconds=0;}
            flag_reg &= ~(1<<TIME_INC);
        }

        if((flag_reg&(1<<DISP_UPDATE))){update_disp();flag_reg&=~(1<<DISP_UPDATE);}
        if((flag_reg&(1<<DISP_UPDATE2))){update_disp2();flag_reg&=~(1<<DISP_UPDATE2);}

        nachti();
	}
    return 0;
}

void init(){
    ACSR = (1<<ACD);
    //uart_init();
    flag_reg = 0;
    seconds = 0;
    minutes = 0;
    millise = 0;

    //seconds timer
    TIMSK1 = (1 << OCIE1A);
    OCR1A  = 57599;
    TCCR1B = (1 << WGM12) | (1<<CS11) | (1<<CS10); //CTC Mode
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
    sei();
    update_disp();
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
        //set_sleep_mode(SLEEP_MODE_PWR_SAVE);
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
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
    float batt = get_voltage();
    batt /= TEILER;
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
    nok.draw_ASCI('.'                    ,LCDWIDTH-6*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI('0'+rtc.t.mon/10%10    ,LCDWIDTH-5*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI('0'+rtc.t.mon%10       ,LCDWIDTH-4*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI('.'                    ,LCDWIDTH-3*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI('0'+rtc.t.year_s/10%10 ,LCDWIDTH-2*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI('0'+rtc.t.year_s%10    ,LCDWIDTH-1*charsize,LCDHEIGHT-charhighte);

    nok.draw_number16x16((minutes/10)%10,0*numberbigsize,2*charhighte-charhighte/2);
    nok.draw_number16x16((minutes   )%10,1*numberbigsize,2*charhighte-charhighte/2);

    nok.draw_ASCI('.'                    ,2*numberbigsize+charsize/4,2*charhighte-charhighte/4*3);
    nok.draw_ASCI('.'                    ,2*numberbigsize+charsize/4,3*charhighte-charhighte/4*3);

    nok.draw_number16x16((seconds/10)%10,2*numberbigsize+charsize,2*charhighte-charhighte/2);
    nok.draw_number16x16((seconds   )%10,3*numberbigsize+charsize,2*charhighte-charhighte/2);

    nok.draw_ASCI('.'                    ,LCDWIDTH-4*charsize,4*charhighte-charhighte/2);
    nok.draw_ASCI('0'+(millise/100   )%10,LCDWIDTH-3*charsize,4*charhighte-charhighte/2);
    nok.draw_ASCI('0'+(millise/10    )%10,LCDWIDTH-2*charsize,4*charhighte-charhighte/2);
    nok.draw_ASCI('0'+(millise       )%10,LCDWIDTH-1*charsize,4*charhighte-charhighte/2);


    nok.draw_ASCI('0'+((uint8_t)(batt))%10      ,LCDWIDTH-4*charsize,0);
    nok.draw_ASCI('.'                           ,LCDWIDTH-3*charsize,0);
    nok.draw_ASCI('0'+((uint8_t)(batt*10))%10   ,LCDWIDTH-2*charsize,0);
    nok.draw_ASCI('V'                           ,LCDWIDTH-1*charsize,0);
    nok.display();
}

void update_disp2(){
    if(flag_reg&(1<<BACKLIGHT)){
        if(brightnes>100){brightnes=100;}
        OCR0A = (uint8_t)((float)brightnes*2.55);
    }
    nok.clearDisplay();
    float batt = get_voltage();
    batt /= TEILER;
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
    nok.draw_ASCI('.'                    ,LCDWIDTH-6*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI('0'+rtc.t.mon/10%10    ,LCDWIDTH-5*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI('0'+rtc.t.mon%10       ,LCDWIDTH-4*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI('.'                    ,LCDWIDTH-3*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI('0'+rtc.t.year_s/10%10 ,LCDWIDTH-2*charsize,LCDHEIGHT-charhighte);
    nok.draw_ASCI('0'+rtc.t.year_s%10    ,LCDWIDTH-1*charsize,LCDHEIGHT-charhighte);

    if(brightnes>=100){
        nok.draw_number16x16((brightnes/100)%10, 1*numberbigsize,1*charhighte);
    }
    nok.draw_number16x16((brightnes/10)%10, 2*numberbigsize,1*charhighte);
    nok.draw_number16x16((brightnes)%10, 3*numberbigsize,1*charhighte);

    nok.drawprogress(0,charhighte*3,LCDWIDTH-1,charhighte*5-1,brightnes);

    nok.draw_ASCI('0'+((uint8_t)(batt))%10      ,LCDWIDTH-4*charsize,0);
    nok.draw_ASCI('.'                           ,LCDWIDTH-3*charsize,0);
    nok.draw_ASCI('0'+((uint8_t)(batt*10))%10   ,LCDWIDTH-2*charsize,0);
    nok.draw_ASCI('V'                           ,LCDWIDTH-1*charsize,0);
    nok.display();
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

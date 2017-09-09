//this example runs with 2 shift registers

#define F_CPU 8000000
#define BAUD 9600

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include "seven_seg.h"
#include "Input.h"
#include "Output.h"
//#include "l74hc595b.h"
#include "DHT22.h"
DHT22 dht22(0, PD2, &DDRD, &PORTD, INT0, ISC00, ISC01, INTF0, &EIMSK, &EICRA, &EIFR, CS21, &OCR2A, &TCNT2, &TCCR2B);
#include "com.h"
//#include "tm1637.h"

//l74hc595b shift_reg(&DDRB,&PORTB,1,2,3,1);
//seven_seg seg(4,&DDRC,&PORTC,0,&shift_reg);
Input taster(&DDRD,&PORTD,&PIND,6,true);
Input ebene0(&DDRC,&PORTC,&PINC,5,true);
Output LED(&DDRB,&PORTB,0);
#define WEATHER_FLAG 0
#define DISP_UPDATE 1
#define MANUAL_UPDATE 2
#define ALTERNATE 3
uint8_t flag_reg;

void send_weather();
void update_disp_wae();

ISR(TIMER2_COMPA_vect){
    dht22.ISR_TIMER_TOUTINE();
}

ISR(INT0_vect){
   dht22.ISR_INT_ROUTINE();
}

/*
ISR(TIMER1_COMPA_vect){
    if(!(flag_reg&(1<<MANUAL_UPDATE))){
        flag_reg |= (1<<DISP_UPDATE);
    }
    if(flag_reg&(1<<ALTERNATE)){
        flag_reg &= ~(1<<ALTERNATE);
    }
    else{
        flag_reg |= (1<<ALTERNATE);
    }
}
*/

ISR(USART_RX_vect){
    uint8_t temp = uart_getc();
    static uint8_t count = 0;
    if(temp=='d'){
        uart_putc(taster.ison()+'0');
        uart_putc(ebene0.ison()+'0');
    }
    else if(temp=='w'){
        dht22.DHT22_StartReading();
        flag_reg |= (1<<MANUAL_UPDATE);
        flag_reg |= (1<<WEATHER_FLAG);
    }
    else if(temp=='m'){
        flag_reg |= (1<<MANUAL_UPDATE);
    }
    else if(temp=='a'){
        flag_reg &= ~(1<<MANUAL_UPDATE);
    }
	/*
    else{
        if(count==4){
            //tm1637_dp = temp-'0';
            count = 0;
        }
        else{
            //seg.set_number(count,temp-'0');
            tm1637_setzif(count,temp-'0');
            count++;
        }
    }*/
    
}

int main(void) {

    uart_init();
    //tm1637_init();
    //tm1637_dp = 0;
    flag_reg = 0;

    //init timer fuer segment
    //TIMSK0 |= (1<<TOIE0);
    //TCCR0B |= (1<<CS02);// | (1<<CS00);
    //init timer fuer dht22
    //intialize timer0 for DHT22 Sensors
    TCCR2A = (1 << WGM21);
    TIMSK2 = (1 << OCIE2A);
    //seconds timer
    //TIMSK1 = (1 << OCIE1A);
    //OCR1A  = 31250;
    //TCCR1B = (1 << WGM12) | (1 << CS12); //CTC Mode, Presc 256
    sei();

    /*
    seg.on();
    seg.dot_pos = 1;
    seg.set_number(0,0);
    seg.set_number(1,0);
    seg.set_number(2,0);
    seg.set_number(3,0);
    */

    uint16_t counter = 0;
	while(true) 
    {
        if(taster.pressed()){
            counter++;
            if(counter>1999){counter=0;}
            //seg.set_number(0,counter/1000%10);
            //seg.set_number(1,counter/100%10);
            //seg.set_number(2,counter/10%10);
            //seg.set_number(3,counter%10);
        }
        if(taster.ison() || (flag_reg&(1<<ALTERNATE))){LED.on();}
        else if(ebene0.ison() || (flag_reg&(1<<ALTERNATE))){LED.on();}
        else{LED.off();}
        if((flag_reg&(1<<WEATHER_FLAG))){send_weather();flag_reg&=~(1<<WEATHER_FLAG);}
        //if((flag_reg&(1<<DISP_UPDATE))){update_disp_wae();flag_reg&=~(1<<DISP_UPDATE);}
	}

}

/*
void update_disp_wae(){
    uint8_t status = 0;
    status = dht22.DHT22_StartReading();
    while(status != dht22.DHT_DATA_READY && status != dht22.DHT_ERROR_NOT_RESPOND && status != dht22.DHT_ERROR_CHECKSUM){
        status = dht22.DHT22_CheckStatus();
    }
    if(flag_reg&(1<<ALTERNATE)){
        tm1637_setzif(0,dht22.humidity_integral/10%10);
        tm1637_setzif(1,dht22.humidity_integral%10);
        tm1637_setzif(2,dht22.humidity_decimal);
        tm1637_setzif(3,0);
    }
    else{
        tm1637_setzif(0,dht22.temperature_integral/10%10);
        tm1637_setzif(1,dht22.temperature_integral%10);
        tm1637_setzif(2,dht22.temperature_decimal);
        tm1637_setzif(3,0);
    }
    tm1637_dp = 1;
}*/

void send_weather(){
   uint8_t status = 0;
   while(status != dht22.DHT_DATA_READY && status != dht22.DHT_ERROR_NOT_RESPOND && status != dht22.DHT_ERROR_CHECKSUM){
   status = dht22.DHT22_CheckStatus();
   }
   if(status == dht22.DHT_ERROR_NOT_RESPOND){
        uart_puts("no_respond!\n");
   }
   else{
       if(dht22.temperature_integral<0){
          uart_putc('-');
          uart_putc('0'+(-1)*dht22.temperature_integral/10);
       }
       else{
          uart_putc('+');
          uart_putc('0'+dht22.temperature_integral/10);
       } 
       uart_putc('0'+dht22.temperature_integral%10);
       uart_putc('.');
       uart_putc('0'+dht22.temperature_decimal);
       uart_putc('\t');
       uart_putc('0'+dht22.humidity_integral/100%10);
       uart_putc('0'+dht22.humidity_integral/10%10);
       uart_putc('0'+dht22.humidity_integral%10);
       uart_putc('.');
       uart_putc('0'+dht22.humidity_decimal);
       uart_putc('\n');
    }
}

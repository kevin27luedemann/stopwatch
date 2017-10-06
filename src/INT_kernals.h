ISR(TIMER1_COMPA_vect){
    if(flag_reg&(1<<STOPWATCH)){
        flag_reg |= (1<<DISP_UPDATE);
        flag_reg |= (1<<TIME_INC);
    }
}

ISR(USART_RX_vect){
    uint8_t temp = uart_getc();
}

ISR(INT0_vect){
    if(STW.ison() && !(flag_reg&(1<<STOPWATCH))){
        flag_reg |= (1<<STOPWATCH);
        TCNT1   = 0;
    }
    else if(STW.ison() && (flag_reg&(1<<STOPWATCH))){
        flag_reg &= ~(1<<STOPWATCH);
        uint32_t temp1 = TCNT1;
        temp1 += 1;
        temp1 *= 64;
        millise += uint16_t (temp1/F_CPU_KHZ);
        if(millise>=1000){
            seconds++;
            millise-=1000;
            if(seconds>=60){
                minutes++;
                seconds=0;
            }
       }
    }
    flag_reg |= (1<<DISP_UPDATE);
}

ISR(INT1_vect){
    if(RTDT.ison()){
        flag_reg |= (1<<INCREMENT);
    }
    else{
        flag_reg |= (1<<DECREMENT);
    }
    flag_reg |= (1<<DISP_UPDATE);
}

ISR(PCINT0_vect){
    flag_reg |= (1<<CLOCK_TICK) | (1<<DISP_UPDATE);
}

ISR(PCINT1_vect){
    if(taster.ison() && (flag_reg&(1<<BACKLIGHT))){
        blpwm(false);
	flag_reg &= ~(1<<BACKLIGHT);
    }
    else if(taster.ison() && !(flag_reg&(1<<BACKLIGHT))){
        blpwm(true);
        flag_reg |= (1<<BACKLIGHT);
    }
    if(STR.ison() && !(flag_reg&(1<<STOPWATCH))){
        seconds = 0;
        minutes = 0;
        millise = 0;
    }
    flag_reg |= (1<<DISP_UPDATE);
}

ISR(PCINT2_vect){
    /*
    if(RTSW.ison() && (flag_reg&(1<<BACKLIGHT))){flag_reg&=~(1<<BACKLIGHT);blpwm(false);}
    else if(RTSW.ison() && !(flag_reg&(1<<BACKLIGHT))){flag_reg|=(1<<BACKLIGHT);blpwm(true);}
    */
    if(RTSW.ison()){flag_reg |= (1<<DISP_UPDATE) | (1<<BTN_PRESSED);}
}

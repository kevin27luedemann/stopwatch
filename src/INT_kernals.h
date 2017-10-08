ISR(TIMER1_COMPA_vect){
    if(flag_reg&(1<<CLORUNNING)){
        flag_reg |= (1<<DISP_UPDATE);
        flag_reg |= (1<<TIME_INC);
    }
}

ISR(USART_RX_vect){
    uint8_t temp = uart_getc();
}

ISR(INT0_vect){
    //in order to get fast response, the calculation of millisec are done in ISR
    /*
    if(flag_reg&(1<<CLORUNNING)){
        uint32_t temp1 = TCNT1;
        temp1 += 1;
        temp1 *= 64;
        stpwcounter.msec += uint16_t (temp1/F_CPU_KHZ);
        if(stpwcounter.msec>=1000){
            stpwcounter.inc();
            stpwcounter.msec-=1000;
       }
	   flag_reg &= ~(1<<CLORUNNING);
       flag_reg |=  (1<<DISP_UPDATE);
    }
    else{
        flag_reg |= (1<<DISP_UPDATE) | (1<<STOPWATCH);
    }
    */
    flag_reg |= (1<<DISP_UPDATE) | (1<<STOPWATCH);
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
    //global Backlight settings
    //should be left as they are
    if(taster.ison() && (flag_reg&(1<<BACKLIGHT))){
        blpwm(false);
	    flag_reg &= ~(1<<BACKLIGHT);
    }
    else if(taster.ison() && !(flag_reg&(1<<BACKLIGHT))){
        blpwm(true);
        flag_reg |= (1<<BACKLIGHT);
    }

    if(STR.ison()){
        flag_reg |= (1<<STWRESET);
    }
    flag_reg |= (1<<DISP_UPDATE);
}

ISR(PCINT2_vect){
    if(RTSW.ison()){flag_reg |= (1<<DISP_UPDATE) | (1<<BTN_PRESSED);}
}

/*
 * Monitor.h
 *
 * Created: 13.12.2015 13:21:12
 *  Author: Lüdemann
 */ 


#ifndef MONITOR_H_
#define MONITOR_H_
#include <stdio.h>
#include <stdlib.h>
#include "nokia_5110.h"

#ifndef numberofpages
#define numberofpages 4
#endif

class monitor
{
private:
	
protected:
	nokia_5110 *no;
	ds3231 *rt;
	int8_t posy;
	uint8_t maxentries;
public:
	monitor(nokia_5110 *display, ds3231 *clock)
	{
		no = display;
		rt = clock;
		posy=0;
		maxentries=1;
	}

    virtual void btn(){
		position = numberofpages;
    }

    virtual void inc(){
        posy++;
        if(posy>=maxentries){posy = 0;}
    }
	
    virtual void dec(){
        posy--;
        if(posy<0){posy=maxentries-1;}
    }

	virtual void STWbtn(){

	}

	virtual void STRbtn(){

	}

    virtual void timer(){

    }

    virtual void clock_tick(){

    }
	
	//draw header
	virtual void header(){
		no->draw_ASCI('0'+rt->t.hour/10%10,0*charsize,0);
		no->draw_ASCI('0'+rt->t.hour%10   ,1*charsize,0);
		no->draw_ASCI(':'                 ,2*charsize,0);
		no->draw_ASCI('0'+rt->t.min/10%10 ,3*charsize,0);
		no->draw_ASCI('0'+rt->t.min%10    ,4*charsize,0);
		no->draw_ASCI(':'                 ,5*charsize,0);
		no->draw_ASCI('0'+rt->t.sec/10%10 ,6*charsize,0);
		no->draw_ASCI('0'+rt->t.sec%10    ,7*charsize,0);
	
		no->draw_ASCI('0'+uint16_t(batt)%10      ,LCDWIDTH-5*charsize,0);
		no->draw_ASCI('.'                        ,LCDWIDTH-4*charsize,0);
		no->draw_ASCI('0'+uint32_t(batt*10.0)%10 ,LCDWIDTH-3*charsize,0);
		no->draw_ASCI('0'+uint32_t(batt*100.0)%10,LCDWIDTH-2*charsize,0);
		no->draw_ASCI('V'                        ,LCDWIDTH-1*charsize,0);
	}
	
	//draw footer
	virtual void footer(){
		no->draw_ASCI('0'+rt->t.mday/10%10   ,LCDWIDTH-8*charsize,LCDHEIGHT-charhighte);
		no->draw_ASCI('0'+rt->t.mday%10      ,LCDWIDTH-7*charsize,LCDHEIGHT-charhighte);
		no->draw_ASCI('.'                    ,LCDWIDTH-6*charsize,LCDHEIGHT-charhighte);
		no->draw_ASCI('0'+rt->t.mon/10%10    ,LCDWIDTH-5*charsize,LCDHEIGHT-charhighte);
		no->draw_ASCI('0'+rt->t.mon%10       ,LCDWIDTH-4*charsize,LCDHEIGHT-charhighte);
		no->draw_ASCI('.'                    ,LCDWIDTH-3*charsize,LCDHEIGHT-charhighte);
		no->draw_ASCI('0'+rt->t.year_s/10%10 ,LCDWIDTH-2*charsize,LCDHEIGHT-charhighte);
		no->draw_ASCI('0'+rt->t.year_s%10    ,LCDWIDTH-1*charsize,LCDHEIGHT-charhighte);
	}
	
	//drawfunktion
	virtual void draw(){
    	no->clearDisplay();
	}

	//send the frame to the display
	void send(){
    	no->display();
	}
};

class stop_clock:public monitor
{
	private:
	public:
	stts last_round;
	stts last_split;
	stop_clock(nokia_5110 *disp, ds3231 *rt):monitor(disp,rt)
	{
		maxentries = 2;
		last_round.init();
		last_split.init();
	}

	void STRbtn(){
		if(!(flag_reg&(1<<CLOCKWATCH))){
			stpwcounter.init();
			last_round.init();
			last_split.init();
		}
		else{
			last_round.init();
			last_round.sec = stpwcounter.sec  - last_split.sec;
			if(last_round.sec < 0){
				last_round.min--;
				last_round.sec += 60;
			}
			last_round.min += stpwcounter.min  - last_split.min;
			if(last_round.min < 0){
				last_round.hour--;
				last_round.min += 60;
			}
			last_round.hour += stpwcounter.hour - last_split.hour;
			
			last_split.msec = stpwcounter.msec;
			last_split.sec  = stpwcounter.sec;
			last_split.min  = stpwcounter.min;
			last_split.hour = stpwcounter.hour;
		}
	}

	void STWbtn(){
		if(!(flag_reg&(1<<CLOCKWATCH))){
			flag_reg |= (1<<CLOCKWATCH);
		}
		else{
			flag_reg &= ~(1<<CLOCKWATCH);
			last_round.init();
			last_round.sec = stpwcounter.sec  - last_split.sec;
			if(last_round.sec < 0){
				last_round.min--;
				last_round.sec += 60;
			}
			last_round.min += stpwcounter.min  - last_split.min;
			if(last_round.min < 0){
				last_round.hour--;
				last_round.min += 60;
			}
			last_round.hour += stpwcounter.hour - last_split.hour;
			
		}
	}

	//anzeige vorbereiten
	void draw()
	{
		monitor::draw();
		header();
		footer();
		switch (posy)
		{
			case 0:
				no->draw_ASCI('0'+(stpwcounter.hour/10  )%10  ,0*charsize,1*charhighte+charhighte/2);
				no->draw_ASCI('0'+(stpwcounter.hour     )%10  ,1*charsize,1*charhighte+charhighte/2);
				no->draw_ASCI(':'                             ,2*charsize,1*charhighte+charhighte/2);
				no->draw_number16x16((stpwcounter.min/10)%10  ,LCDWIDTH-4*numberbigsize-charsize+1 	,3*charhighte-charhighte/2);
				no->draw_number16x16((stpwcounter.min   )%10  ,LCDWIDTH-3*numberbigsize-charsize+1	,3*charhighte-charhighte/2);
				no->draw_ASCI('.'                    		  ,LCDWIDTH-2*numberbigsize-charsize*3/4-1,3*charhighte-charhighte/4*3);
				no->draw_ASCI('.'                    		  ,LCDWIDTH-2*numberbigsize-charsize*3/4-1,3*charhighte+charhighte/4);
				no->draw_number16x16((stpwcounter.sec/10)%10  ,LCDWIDTH-2*numberbigsize				,3*charhighte-charhighte/2);
				no->draw_number16x16((stpwcounter.sec   )%10  ,LCDWIDTH-1*numberbigsize				,3*charhighte-charhighte/2);
				break;
			case 1:
				no->draw_ASCI('0'+(stpwcounter.hour/10  )%10,3*charsize,1.5*charhighte);
				no->draw_ASCI('0'+(stpwcounter.hour     )%10,4*charsize,1.5*charhighte);
				no->draw_ASCI(':'                           ,5*charsize,1.5*charhighte);
				no->draw_ASCI('0'+(stpwcounter.min/10)%10   ,6*charsize,1.5*charhighte);
				no->draw_ASCI('0'+(stpwcounter.min   )%10   ,7*charsize,1.5*charhighte);
				no->draw_ASCI(':'                    		,8*charsize,1.5*charhighte);
				no->draw_ASCI('0'+(stpwcounter.sec/10)%10   ,9*charsize,1.5*charhighte);
				no->draw_ASCI('0'+(stpwcounter.sec   )%10   ,10*charsize,1.5*charhighte);

				no->draw_ASCI('0'+(last_split.hour/10  )%10,3*charsize,2.5*charhighte);
				no->draw_ASCI('0'+(last_split.hour     )%10,4*charsize,2.5*charhighte);
				no->draw_ASCI(':'                          ,5*charsize,2.5*charhighte);
				no->draw_ASCI('0'+(last_split.min/10)%10   ,6*charsize,2.5*charhighte);
				no->draw_ASCI('0'+(last_split.min   )%10   ,7*charsize,2.5*charhighte);
				no->draw_ASCI(':'                    	   ,8*charsize,2.5*charhighte);
				no->draw_ASCI('0'+(last_split.sec/10)%10   ,9*charsize,2.5*charhighte);
				no->draw_ASCI('0'+(last_split.sec   )%10   ,10*charsize,2.5*charhighte);

				no->draw_ASCI('0'+(last_round.hour/10  )%10,3*charsize,3.5*charhighte);
				no->draw_ASCI('0'+(last_round.hour     )%10,4*charsize,3.5*charhighte);
				no->draw_ASCI(':'                          ,5*charsize,3.5*charhighte);
				no->draw_ASCI('0'+(last_round.min/10)%10   ,6*charsize,3.5*charhighte);
				no->draw_ASCI('0'+(last_round.min   )%10   ,7*charsize,3.5*charhighte);
				no->draw_ASCI(':'                    	   ,8*charsize,3.5*charhighte);
				no->draw_ASCI('0'+(last_round.sec/10)%10   ,9*charsize,3.5*charhighte);
				no->draw_ASCI('0'+(last_round.sec   )%10   ,10*charsize,3.5*charhighte);
				break;
			default:
				break;
		}
		send();
	}
};

class stop_watch:public monitor
{
	private:
    void cl_timer(uint8_t on){
        if(on){
            TIMSK1 = (1 << OCIE1A);
            OCR1A  = 57599;
            TCCR1B = (1 << WGM12) | (1<<CS11) | (1<<CS10); //CTC Mode
        	TCNT1   = 0;
        }
        else{
            TCCR1B &= ~((1 << WGM12) | (1<<CS11) | (1<<CS10));
        }
    }
	public:
	stts last_round;
	stts last_split;
	stop_watch(nokia_5110 *disp, ds3231 *rt):monitor(disp,rt)
	{
		maxentries = 2;
		last_round.init();
		last_split.init();
	}

	void STRbtn(){
		if(!(flag_reg&(1<<CLORUNNING))){
			stpwcounter.init();
			last_split.init();
			last_round.init();
		}
		else{
			uint32_t temp1 = TCNT1;
			temp1 += 1;
			temp1 *= 64;
			stpwcounter.msec += uint16_t (temp1/F_CPU_KHZ);
			if(stpwcounter.msec>=1000){
				stpwcounter.inc();
				stpwcounter.msec-=1000;
		   	}

			last_round.init();
			last_round.msec = stpwcounter.msec - last_split.msec;
			if(last_round.msec < 0){
				last_round.sec--;
				last_round.msec+=1000;
			}
			last_round.sec += stpwcounter.sec  - last_split.sec;
			if(last_round.sec < 0){
				last_round.min--;
				last_round.sec += 60;
			}
			last_round.min += stpwcounter.min  - last_split.min;
			if(last_round.min < 0){
				last_round.hour--;
				last_round.min += 60;
			}
			last_round.hour += stpwcounter.hour - last_split.hour;
			
			last_split.msec = stpwcounter.msec;
			last_split.sec  = stpwcounter.sec;
			last_split.min  = stpwcounter.min;
			last_split.hour = stpwcounter.hour;
		}
	}

	void STWbtn(){
		if(!(flag_reg&(1<<CLORUNNING))){
			flag_reg|=(1<<CLORUNNING);
            cl_timer(true);
		}
		else{
			uint32_t temp1 = TCNT1;
			temp1 += 1;
			temp1 *= 64;
			stpwcounter.msec += uint16_t (temp1/F_CPU_KHZ);
			if(stpwcounter.msec>=1000){
				stpwcounter.inc();
				stpwcounter.msec-=1000;
		   	}
			flag_reg &= ~(1<<CLORUNNING);
            cl_timer(false);
			last_round.init();
			last_round.msec = stpwcounter.msec - last_split.msec;
			if(last_round.msec < 0){
				last_round.sec--;
				last_round.msec+=1000;
			}
			last_round.sec += stpwcounter.sec  - last_split.sec;
			if(last_round.sec < 0){
				last_round.min--;
				last_round.sec += 60;
			}
			last_round.min += stpwcounter.min  - last_split.min;
			if(last_round.min < 0){
				last_round.hour--;
				last_round.min += 60;
			}
			last_round.hour += stpwcounter.hour - last_split.hour;
			
		}
	}

	//anzeige vorbereiten
	void draw()
	{
		monitor::draw();
		header();
		footer();
		switch (posy)
		{
			case 0:
				no->draw_ASCI('0'+(stpwcounter.hour/10  )%10  ,0*charsize 							   ,1*charhighte+charhighte/2);
				no->draw_ASCI('0'+(stpwcounter.hour     )%10  ,1*charsize							   ,1*charhighte+charhighte/2);
				no->draw_ASCI(':'                             ,2*charsize							   ,1*charhighte+charhighte/2);
				no->draw_number16x16((stpwcounter.min/10)%10  ,LCDWIDTH-4*numberbigsize-charsize+1 	   ,3*charhighte-charhighte/2);
				no->draw_number16x16((stpwcounter.min   )%10  ,LCDWIDTH-3*numberbigsize-charsize+1	   ,3*charhighte-charhighte/2);
				no->draw_ASCI('.'                    		  ,LCDWIDTH-2*numberbigsize-charsize*3/4-1 ,3*charhighte-charhighte/4*3);
				no->draw_ASCI('.'                    		  ,LCDWIDTH-2*numberbigsize-charsize*3/4-1 ,3*charhighte+charhighte/4);
				no->draw_number16x16((stpwcounter.sec/10)%10  ,LCDWIDTH-2*numberbigsize				   ,3*charhighte-charhighte/2);
				no->draw_number16x16((stpwcounter.sec   )%10  ,LCDWIDTH-1*numberbigsize				   ,3*charhighte-charhighte/2);
				no->draw_ASCI('.'                             ,LCDWIDTH-4*charsize					   ,1*charhighte+charhighte/2);
				no->draw_ASCI('0'+(stpwcounter.msec/100 )%10  ,LCDWIDTH-3*charsize 					   ,1*charhighte+charhighte/2);
				no->draw_ASCI('0'+(stpwcounter.msec/10  )%10  ,LCDWIDTH-2*charsize					   ,1*charhighte+charhighte/2);
				no->draw_ASCI('0'+(stpwcounter.msec     )%10  ,LCDWIDTH-1*charsize					   ,1*charhighte+charhighte/2);
				break;
			case 1:
				no->draw_ASCI('0'+(stpwcounter.hour/10  )%10,1*charsize,1.5*charhighte);
				no->draw_ASCI('0'+(stpwcounter.hour     )%10,2*charsize,1.5*charhighte);
				no->draw_ASCI(':'                           ,3*charsize,1.5*charhighte);
				no->draw_ASCI('0'+(stpwcounter.min/10)%10   ,4*charsize,1.5*charhighte);
				no->draw_ASCI('0'+(stpwcounter.min   )%10   ,5*charsize,1.5*charhighte);
				no->draw_ASCI(':'                    		,6*charsize,1.5*charhighte);
				no->draw_ASCI('0'+(stpwcounter.sec/10)%10   ,7*charsize,1.5*charhighte);
				no->draw_ASCI('0'+(stpwcounter.sec   )%10   ,8*charsize,1.5*charhighte);
				no->draw_ASCI(':'                    	    ,9*charsize,1.5*charhighte);
				no->draw_ASCI('0'+(stpwcounter.msec/100)%10 ,10*charsize,1.5*charhighte);
				no->draw_ASCI('0'+(stpwcounter.msec/10 )%10 ,11*charsize,1.5*charhighte);
				no->draw_ASCI('0'+(stpwcounter.msec    )%10 ,12*charsize,1.5*charhighte);

				no->draw_ASCI('0'+(last_split.hour/10  )%10,1*charsize,2.5*charhighte);
				no->draw_ASCI('0'+(last_split.hour     )%10,2*charsize,2.5*charhighte);
				no->draw_ASCI(':'                          ,3*charsize,2.5*charhighte);
				no->draw_ASCI('0'+(last_split.min/10)%10   ,4*charsize,2.5*charhighte);
				no->draw_ASCI('0'+(last_split.min   )%10   ,5*charsize,2.5*charhighte);
				no->draw_ASCI(':'                    	   ,6*charsize,2.5*charhighte);
				no->draw_ASCI('0'+(last_split.sec/10)%10   ,7*charsize,2.5*charhighte);
				no->draw_ASCI('0'+(last_split.sec   )%10   ,8*charsize,2.5*charhighte);
				no->draw_ASCI(':'                    	   ,9*charsize,2.5*charhighte);
				no->draw_ASCI('0'+(last_split.msec/100)%10 ,10*charsize,2.5*charhighte);
				no->draw_ASCI('0'+(last_split.msec/10 )%10 ,11*charsize,2.5*charhighte);
				no->draw_ASCI('0'+(last_split.msec    )%10 ,12*charsize,2.5*charhighte);

				no->draw_ASCI('0'+(last_round.hour/10  )%10,1*charsize,3.5*charhighte);
				no->draw_ASCI('0'+(last_round.hour     )%10,2*charsize,3.5*charhighte);
				no->draw_ASCI(':'                          ,3*charsize,3.5*charhighte);
				no->draw_ASCI('0'+(last_round.min/10)%10   ,4*charsize,3.5*charhighte);
				no->draw_ASCI('0'+(last_round.min   )%10   ,5*charsize,3.5*charhighte);
				no->draw_ASCI(':'                    	   ,6*charsize,3.5*charhighte);
				no->draw_ASCI('0'+(last_round.sec/10)%10   ,7*charsize,3.5*charhighte);
				no->draw_ASCI('0'+(last_round.sec   )%10   ,8*charsize,3.5*charhighte);
				no->draw_ASCI(':'                    	   ,9*charsize,3.5*charhighte);
				no->draw_ASCI('0'+(last_round.msec/100)%10 ,10*charsize,3.5*charhighte);
				no->draw_ASCI('0'+(last_round.msec/10 )%10 ,11*charsize,3.5*charhighte);
				no->draw_ASCI('0'+(last_round.msec    )%10 ,12*charsize,3.5*charhighte);
				break;
			default:
				break;
		}
		send();
	}
};

class tacho:public monitor
{
	private:
    void cl_timer(uint8_t on){
        if(on){
            TIMSK1 = (1 << OCIE1A);
            OCR1A  = 57599;
            TCCR1B = (1 << WGM12) | (1<<CS11) | (1<<CS10); //CTC Mode
        	TCNT1   = 0;
        }
        else{
            TCCR1B &= ~((1 << WGM12) | (1<<CS11) | (1<<CS10));
        }
    }
	inline float ct_sec(){
		return (float)holder.sec+((float)holder.msec)/1000.+(float)holder.min*60.+(float)holder.hour*3600.;
	}
	public:
	stts holder;
	float diameter;
	float speed_kmh;
	float speed_ms;
	tacho(nokia_5110 *disp, ds3231 *rt):monitor(disp,rt)
	{
		maxentries 	= 1;
		diameter 	= 28.0*2.54/100.0;
		speed_kmh   = 0.;
        speed_ms    = 0.;
		holder.init();
	}

	void STRbtn(){
		if(!(flag_reg&(1<<CLORUNNING))){
			stpwcounter.init();
			holder.init();
		}
		else{
			flag_reg &= ~(1<<CLORUNNING);
            cl_timer(false);
		}
	}

	void STWbtn(){
		if(!(flag_reg&(1<<CLORUNNING))){
			flag_reg|=(1<<CLORUNNING);
            cl_timer(true);
		}
		else{
			uint32_t temp1 = TCNT1;
			temp1 += 1;
			temp1 *= 64;
			stpwcounter.msec += uint16_t (temp1/F_CPU_KHZ);
			if(stpwcounter.msec>=1000){
				stpwcounter.inc();
				stpwcounter.msec-=1000;
		   	}
			holder = stpwcounter;
			stpwcounter.init();
		}
	}

	//anzeige vorbereiten
	void draw()
	{
		monitor::draw();
		header();
		footer();
		speed_ms    = diameter/ct_sec();
        speed_kmh   = speed_ms*3.6;

		no->draw_number16x16(((uint8_t)speed_kmh/100  )%10  ,0*numberbigsize            ,1*charhighte+charhighte/2);
		no->draw_number16x16(((uint8_t)speed_kmh/10   )%10  ,1*numberbigsize            ,1*charhighte+charhighte/2);
		no->draw_number16x16(((uint8_t)speed_kmh      )%10  ,2*numberbigsize            ,1*charhighte+charhighte/2);
        no->draw_ASCI('.'                 		            ,3*numberbigsize            ,3*charhighte-charhighte/4*3);
		no->draw_number16x16(((uint8_t)(speed_kmh*10.))%10  ,3*numberbigsize+charsize/2 ,1*charhighte+charhighte/2);
        no->draw_ASCI('k'                 		            ,4*numberbigsize+3*charsize/4 ,1*charhighte+charhighte/2);
        no->draw_ASCI('m'                 		            ,4*numberbigsize+7*charsize/4 ,1*charhighte+charhighte/2);
        no->draw_ASCI('h'                 		            ,4*numberbigsize+3*charsize/4 ,2*charhighte+charhighte/2);

		no->draw_ASCI('0'+((uint8_t)speed_ms/10  )%10 ,0*charsize ,3*charhighte+charhighte/2);
		no->draw_ASCI('0'+((uint8_t)speed_ms     )%10 ,1*charsize ,3*charhighte+charhighte/2);
        no->draw_ASCI('.'                 		      ,2*charsize ,3*charhighte+charhighte/2);
		no->draw_ASCI('0'+((uint8_t)(speed_ms*10))%10 ,3*charsize ,3*charhighte+charhighte/2);
        no->draw_ASCI('m'                 		      ,4*charsize+charsize/4 ,3*charhighte+charhighte/2);
        no->draw_ASCI('/'                 		      ,5*charsize+charsize/4 ,3*charhighte+charhighte/2);
        no->draw_ASCI('s'                 		      ,6*charsize+charsize/4 ,3*charhighte+charhighte/2);
		send();
	}
};

const char basketball_names[][11] PROGMEM = {
    "  Snickers",
    " Superstar",
    "    Anhang",
    "     Bonus"};
class counter:public monitor
{
	private:
	public:
	int16_t *c;
	uint8_t dele;
    uint8_t basketball;
	counter(nokia_5110 *disp, ds3231 *rt):monitor(disp,rt)
	{
		maxentries = 4+2;
		dele = 0;
        basketball = 0;
		c = new int16_t[maxentries-2];
		for(uint8_t i=0;i<maxentries-2;i++){c[i]=0;}
	}

	void STRbtn(){
		if(posy == maxentries-1){
			dele++;
			if(dele==2){dele=0;}
		}
        else if(posy == maxentries-2){
        }
		else{
			c[posy]--;
		}
	}

	void STWbtn(){
		if(posy == maxentries-1 && dele >= 1){
			dele = 0;	
			for(uint8_t i=0;i<maxentries-1;i++){c[i]=0;}
			posy = 0;
		}
        else if(posy == maxentries-2){
            basketball += 1;
            basketball %= 2;
        }
		else{
			c[posy]++;
		}
	}

	//anzeige vorbereiten
	void draw()
	{
		monitor::draw();
		header();
		footer();
		if(posy == maxentries-1){
			if(dele == 0){
				no->draw_ASCI('D',1*charsize,2*charhighte);
				no->draw_ASCI('e',2*charsize,2*charhighte);
				no->draw_ASCI('l',3*charsize,2*charhighte);
				no->draw_ASCI('e',4*charsize,2*charhighte);
				no->draw_ASCI('t',5*charsize,2*charhighte);
				no->draw_ASCI('e',6*charsize,2*charhighte);
				no->draw_ASCI(' ',7*charsize,2*charhighte);
				no->draw_ASCI('A',8*charsize,2*charhighte);
				no->draw_ASCI('l',9*charsize,2*charhighte);
				no->draw_ASCI('l',10*charsize,2*charhighte);
				no->draw_ASCI('?',11*charsize,2*charhighte);

				no->draw_ASCI('H',1*charsize,3*charhighte);
				no->draw_ASCI('i',2*charsize,3*charhighte);
				no->draw_ASCI('t',3*charsize,3*charhighte);
				no->draw_ASCI(' ',4*charsize,3*charhighte);
				no->draw_ASCI('S',5*charsize,3*charhighte);
				no->draw_ASCI('T',6*charsize,3*charhighte);
				no->draw_ASCI('R',7*charsize,3*charhighte);
				no->draw_ASCI('!',8*charsize,3*charhighte);
			}
			else{
				no->draw_ASCI('S',1*charsize,2*charhighte);
				no->draw_ASCI('u',2*charsize,2*charhighte);
				no->draw_ASCI('r',3*charsize,2*charhighte);
				no->draw_ASCI('e',4*charsize,2*charhighte);
				no->draw_ASCI('?',5*charsize,2*charhighte);

				no->draw_ASCI('H',1*charsize,3*charhighte);
				no->draw_ASCI('i',2*charsize,3*charhighte);
				no->draw_ASCI('t',3*charsize,3*charhighte);
				no->draw_ASCI(' ',4*charsize,3*charhighte);
				no->draw_ASCI('S',5*charsize,3*charhighte);
				no->draw_ASCI('T',6*charsize,3*charhighte);
				no->draw_ASCI('W',7*charsize,3*charhighte);
				no->draw_ASCI('!',8*charsize,3*charhighte);
			}
		}
        else if(posy == maxentries -2){

            no->draw_ASCI('B', 1*charsize,2*charhighte);
            no->draw_ASCI('a', 2*charsize,2*charhighte);
            no->draw_ASCI('s', 3*charsize,2*charhighte);
            no->draw_ASCI('k', 4*charsize,2*charhighte);
            no->draw_ASCI('e', 5*charsize,2*charhighte);
            no->draw_ASCI('t', 6*charsize,2*charhighte);
            no->draw_ASCI('b', 7*charsize,2*charhighte);
            no->draw_ASCI('a', 8*charsize,2*charhighte);
            no->draw_ASCI('l', 9*charsize,2*charhighte);
            no->draw_ASCI('l',10*charsize,2*charhighte);
            if(basketball){
                no->draw_ASCI('t', 1*charsize,3*charhighte);
                no->draw_ASCI('r', 2*charsize,3*charhighte);
                no->draw_ASCI('u', 3*charsize,3*charhighte);
                no->draw_ASCI('e', 4*charsize,3*charhighte);
            }
            else{
                no->draw_ASCI('f', 1*charsize,3*charhighte);
                no->draw_ASCI('a', 2*charsize,3*charhighte);
                no->draw_ASCI('l', 3*charsize,3*charhighte);
                no->draw_ASCI('s', 4*charsize,3*charhighte);
                no->draw_ASCI('e', 5*charsize,3*charhighte);
            }

        }
		else{
            if(basketball){
                for(uint8_t i=0;i<11;i++){
                    if(i<10){
                        no->draw_ASCI(pgm_read_byte(&basketball_names[posy][i]),i*charsize+0*charsize,1*charhighte);
                    }
                    else{
                        no->draw_ASCI(':',i*charsize+0*charsize,1*charhighte);
                    }
                }
            }
            else{
                no->draw_ASCI('0'+posy,0*charsize,1*charhighte);
                no->draw_ASCI(':'     ,1*charsize,1*charhighte);
            }
			if(c[posy] < 0){
				no->draw_ASCI('-'     ,0.5*charsize,2.5*charhighte);
			}
			no->draw_number16x16(abs(c[posy]/10000)%10,0*numberbigsize+1.5*charsize,2*charhighte);
			no->draw_number16x16(abs(c[posy]/100  )%10,1*numberbigsize+1.5*charsize,2*charhighte);
			no->draw_number16x16(abs(c[posy]/10   )%10,2*numberbigsize+1.5*charsize,2*charhighte);
			no->draw_number16x16(abs(c[posy]      )%10,3*numberbigsize+1.5*charsize,2*charhighte);
		}

		send();
	}
};

class watch:public monitor
{
	private:
	public:
	watch(nokia_5110 *disp, ds3231 *rt):monitor(disp,rt)
	{
	}

	void STWbtn(){
		}
	
	//anzeige vorbereiten
	void draw()
	{
		monitor::draw();
		header();
		footer();
		no->draw_number16x16((rt->t.hour/10)%10,0*numberbigsize+charsize,2*charhighte);
		no->draw_number16x16((rt->t.hour   )%10,1*numberbigsize+charsize,2*charhighte);

		no->draw_ASCI('.'                    ,2*numberbigsize+charsize/4+charsize,2*charhighte-charhighte/4*1);
		no->draw_ASCI('.'                    ,2*numberbigsize+charsize/4+charsize,3*charhighte-charhighte/4*1);

		no->draw_number16x16((rt->t.min/10)%10,2*numberbigsize+charsize+charsize,2*charhighte);
		no->draw_number16x16((rt->t.min   )%10,3*numberbigsize+charsize+charsize,2*charhighte);

		send();
	}
};

class brightnes_settings:public monitor
{
private:
	uint8_t crement;
public:
	brightnes_settings(nokia_5110 *disp, ds3231 *rt):monitor(disp,rt){
		crement = 5;
	}

    void inc(){
        if(brightnes<=100-crement){brightnes+=crement;}
        if(brightnes>100){brightnes=100;}
        OCR0A = (uint8_t)((float)brightnes*2.55);
    }
    
    void dec(){
        if(brightnes<=100&&brightnes>=crement){brightnes-=crement;}
		if(brightnes>=200){brightnes=0;}
        OCR0A = (uint8_t)((float)brightnes*2.55);
    }

	void draw()
	{
		monitor::draw();
		header();
		footer();
		if(brightnes>=100){
			no->draw_number16x16((brightnes/100)%10, 1*numberbigsize,1*charhighte);
		}
		no->draw_number16x16((brightnes/10)%10, 2*numberbigsize,1*charhighte);
		no->draw_number16x16((brightnes)%10, 3*numberbigsize,1*charhighte);

		no->drawprogress(0,charhighte*3,LCDWIDTH-1,charhighte*5-2,brightnes);
		send();
	}

};

const char menue_entries[][11] PROGMEM = {
    "Watch     ",
    "Stop watch",
    "Stop clock",
	"Tacho     ",
    "Counter   ",
    "Backlight ",
    "Blank     "};
class menue: public monitor
{
public:
	menue(nokia_5110 *disp, ds3231 *rt):monitor(disp,rt){
        maxentries = numberofpages; 
    }

    void btn(){
		position = posy;
    }

	void draw()
	{
		monitor::draw();
		header();
		footer();
		int8_t places[3];
        for(uint8_t i=0; i<3; i++){
			places[i] = posy+(i-1);
            if(places[i]>=maxentries){
                places[i] -= maxentries;
            }
            else if(places[i]<0){
                places[i] += maxentries;
            }
        }
		for(uint8_t i=0;i<10;i++){
        no->draw_ASCI(pgm_read_byte(&menue_entries[(uint8_t)places[0]][i]),i*charsize+1*charsize,1*charhighte+charhighte/4);}
		for(uint8_t i=0;i<10;i++){
        no->draw_ASCI(pgm_read_byte(&menue_entries[(uint8_t)places[1]][i]),i*charsize+2*charsize,2*charhighte+charhighte/2);}
		for(uint8_t i=0;i<10;i++){
        no->draw_ASCI(pgm_read_byte(&menue_entries[(uint8_t)places[2]][i]),i*charsize+1*charsize,4*charhighte-charhighte/4);}
		no->draw_ASCI('>',0*charsize,(2)*charhighte+charhighte/2);

		send();
	}
};

#endif /* MONITOR_H_ */

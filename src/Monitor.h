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
	
		no->draw_ASCI('0'+((uint8_t)(batt))%10      ,LCDWIDTH-4*charsize,0);
		no->draw_ASCI('.'                           ,LCDWIDTH-3*charsize,0);
		no->draw_ASCI('0'+((uint8_t)(batt*10))%10   ,LCDWIDTH-2*charsize,0);
		no->draw_ASCI('V'                           ,LCDWIDTH-1*charsize,0);
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
        batt = get_voltage();
        batt /= TEILER;
	}

	//send the frame to the display
	void send(){
    	no->display();
	}
};

class stop_watch:public monitor
{
	private:
	public:
    ts stpwcounter;
	stop_watch(nokia_5110 *disp, ds3231 *rt):monitor(disp,rt)
	{
		maxentries = 3;
        stpwcounter.init();
	}

	void STRbtn(){
		if(!(flag_reg&(1<<CLORUNNING))){
			stpwcounter.init();
		}
	}

	void STWbtn(){
		if(!(flag_reg&(1<<CLORUNNING))){
			flag_reg|=(1<<CLORUNNING);
        	TCNT1   = 0;
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
		}
	}

    void timer(){
		if((flag_reg&(1<<CLORUNNING))){
            stpwcounter.inc();
        }
    }

	//anzeige vorbereiten
	void draw()
	{
		monitor::draw();
		header();
		footer();
		uint16_t minu = stpwcounter.min;
		minu += stpwcounter.hour*60;
		switch (posy)
		{
			case 0:
				if(minu>=1000){
					no->draw_ASCI('1'                    	  ,0,4*charhighte-charhighte/2);
				}
				no->draw_number16x16((minu/100)%10  		  ,0*numberbigsize,2*charhighte-charhighte/2);
				no->draw_number16x16((minu/10)%10  			  ,1*numberbigsize,2*charhighte-charhighte/2);
				no->draw_number16x16((minu   )%10  			  ,2*numberbigsize,2*charhighte-charhighte/2);
				no->draw_ASCI('.'                    		  ,3*numberbigsize+charsize/4-1,2*charhighte-charhighte/4*3);
				no->draw_ASCI('.'                    		  ,3*numberbigsize+charsize/4-1,3*charhighte-charhighte/4*3);
				no->draw_number16x16((stpwcounter.sec/10)%10  ,3*numberbigsize+charsize-1,2*charhighte-charhighte/2);
				no->draw_number16x16((stpwcounter.sec   )%10  ,4*numberbigsize+charsize-1,2*charhighte-charhighte/2);
				no->draw_ASCI('.'                    		  ,LCDWIDTH-4*charsize,4*charhighte-charhighte/2);
				no->draw_ASCI('0'+(stpwcounter.msec/100   )%10,LCDWIDTH-3*charsize,4*charhighte-charhighte/2);
				no->draw_ASCI('0'+(stpwcounter.msec/10    )%10,LCDWIDTH-2*charsize,4*charhighte-charhighte/2);
				no->draw_ASCI('0'+(stpwcounter.msec       )%10,LCDWIDTH-1*charsize,4*charhighte-charhighte/2);
				break;
			case 1:
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
			case 2:
				no->draw_ASCI('0'+(stpwcounter.yday/100 )%10  ,0*charsize,1*charhighte+charhighte/2);
				no->draw_ASCI('0'+(stpwcounter.yday/10  )%10  ,1*charsize,1*charhighte+charhighte/2);
				no->draw_ASCI('0'+(stpwcounter.yday     )%10  ,2*charsize,1*charhighte+charhighte/2);
				no->draw_ASCI('_'                             ,3*charsize,1*charhighte+charhighte/2);
				no->draw_ASCI('0'+(stpwcounter.hour/10  )%10  ,4*charsize,1*charhighte+charhighte/2);
				no->draw_ASCI('0'+(stpwcounter.hour     )%10  ,5*charsize,1*charhighte+charhighte/2);
				no->draw_ASCI(':'                             ,6*charsize,1*charhighte+charhighte/2);
				no->draw_number16x16((stpwcounter.min/10)%10  ,LCDWIDTH-4*numberbigsize-charsize+1 	,3*charhighte-charhighte/2);
				no->draw_number16x16((stpwcounter.min   )%10  ,LCDWIDTH-3*numberbigsize-charsize+1	,3*charhighte-charhighte/2);
				no->draw_ASCI('.'                    		  ,LCDWIDTH-2*numberbigsize-charsize*3/4-1,3*charhighte-charhighte/4*3);
				no->draw_ASCI('.'                    		  ,LCDWIDTH-2*numberbigsize-charsize*3/4-1,3*charhighte+charhighte/4);
				no->draw_number16x16((stpwcounter.sec/10)%10  ,LCDWIDTH-2*numberbigsize				,3*charhighte-charhighte/2);
				no->draw_number16x16((stpwcounter.sec   )%10  ,LCDWIDTH-1*numberbigsize				,3*charhighte-charhighte/2);
				break;
			default:
				break;
		}
		send();
	}
};

class counter:public monitor
{
	private:
	public:
    uint32_t count;
	counter(nokia_5110 *disp, ds3231 *rt):monitor(disp,rt)
	{
        count = 0;
	}

	void STRbtn(){
        count = 0;
	}

	void STWbtn(){
        count++;
        if(count>=10000){count=0;}
	}

	//anzeige vorbereiten
	void draw()
	{
		monitor::draw();
		header();
		footer();
        no->draw_number16x16((count/10000)%10   ,0*numberbigsize+1.5*charsize,2*charhighte);
        no->draw_number16x16((count/100)%10     ,1*numberbigsize+1.5*charsize,2*charhighte);
        no->draw_number16x16((count/10)%10      ,2*numberbigsize+1.5*charsize,2*charhighte);
        no->draw_number16x16((count   )%10  	,3*numberbigsize+1.5*charsize,2*charhighte);

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

class blank:public monitor
{
public:
	blank(nokia_5110 *disp, ds3231 *rt):monitor(disp,rt){
    }

	void draw()
	{
		monitor::draw();
		send();
	}
};

#endif /* MONITOR_H_ */

﻿/*
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
public:
	uint8_t posy;
	uint8_t posx;
	uint8_t maxentries;
	uint8_t maxentriesx;
	monitor(nokia_5110 *display, ds3231 *clock)
	{
		no = display;
		rt = clock;
		posy=0;
		posx=0;
		maxentries=1;
		maxentriesx = 1;
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
	stop_watch(nokia_5110 *disp, ds3231 *rt):monitor(disp,rt)
	{
	}
	

	//anzeige vorbereiten
	void draw()
	{
		monitor::draw();
		header();
		footer();
		no->draw_number16x16((minutes/10)%10,0*numberbigsize,2*charhighte-charhighte/2);
		no->draw_number16x16((minutes   )%10,1*numberbigsize,2*charhighte-charhighte/2);

		no->draw_ASCI('.'                    ,2*numberbigsize+charsize/4,2*charhighte-charhighte/4*3);
		no->draw_ASCI('.'                    ,2*numberbigsize+charsize/4,3*charhighte-charhighte/4*3);

		no->draw_number16x16((seconds/10)%10,2*numberbigsize+charsize,2*charhighte-charhighte/2);
		no->draw_number16x16((seconds   )%10,3*numberbigsize+charsize,2*charhighte-charhighte/2);

		no->draw_ASCI('.'                    ,LCDWIDTH-4*charsize,4*charhighte-charhighte/2);
		no->draw_ASCI('0'+(millise/100   )%10,LCDWIDTH-3*charsize,4*charhighte-charhighte/2);
		no->draw_ASCI('0'+(millise/10    )%10,LCDWIDTH-2*charsize,4*charhighte-charhighte/2);
		no->draw_ASCI('0'+(millise       )%10,LCDWIDTH-1*charsize,4*charhighte-charhighte/2);
		send();
	}
};

class brightnes_settings:public monitor
{
public:
	brightnes_settings(nokia_5110 *disp, ds3231 *rt):monitor(disp,rt){}

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

		no->drawprogress(0,charhighte*3,LCDWIDTH-1,charhighte*5-1,brightnes);
		send();
	}

};

#endif /* MONITOR_H_ */

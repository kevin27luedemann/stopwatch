/*
 * RTC DS3231 Library with time struct
 *
 * Created by Kevin Luedemann
 * 10.09.2017 16:41
 */

#ifndef __DS3231_H__
#define __DS3231_H__

//#define DS3231_I2C_ADDR             0x68
#define DS3231_I2C_ADDR             0xD0
#define DS3231_TIME_CAL_ADDR        0x00
#define DS3231_ALARM1_ADDR          0x07
#define DS3231_ALARM2_ADDR          0x0B
#define DS3231_CONTROL_ADDR         0x0E
#define DS3231_STATUS_ADDR          0x0F
#define DS3231_AGING_OFFSET_ADDR    0x10
#define DS3231_TEMPERATURE_ADDR     0x11

#define DS3231_1224	0x40

#define DS3231_A1IE     0x1
#define DS3231_A2IE     0x2
#define DS3231_INTCN    0x4
#define DS3231_RS1	0x8
#define DS3231_RS2	0x10
#define DS3231_CONV	0x20
#define DS3231_BBSQW    0x40
#define DS3231_EOSC	0x80

#define DS3231_A1F      0x1
#define DS3231_A2F      0x2
#define DS3231_OSF      0x80

#include <avr/io.h>
#include "I2C.h"

struct ts {
	int8_t sec;
	int8_t min;
	int8_t hour;
	int8_t mday;
	int8_t mon;
	int16_t year;
	int8_t wday;
	int16_t yday;
	int8_t isdst;
	int8_t year_s;

	inline void init(){
		sec 	= 0;
		min 	= 0;
		hour 	= 0;
		mday 	= 0;
		mon 	= 0;
		year 	= 0;
		wday 	= 0;
		yday    = 0;
		isdst   = 0;
		year_s  = 0;
	}
};

struct stts {
	uint16_t msec;
	int8_t sec;
	int8_t min;
	int8_t hour;

	inline void inc(){
		sec++;
		if(sec>=60){
			sec=0;
			min++;
			if(min>=60){
				min=0;
				hour++;
				if(hour>=24){
					hour=0;
				}
			}
		}
	}
	inline void dec(){
		sec--;
		if(sec<=0){
			sec=60;
			min--;
			if(min<=0){
				min=60;
				hour--;
			}
		}
	}
 	
	inline void init(){
		msec 	= 0;
		sec 	= 0;
		min 	= 0;
		hour 	= 0;
	}
};

class ds3231
{
public:
	ds3231();
	~ds3231();
	void get();
	void set();
	void activate_sqm();
	void deactivate_sqm();
	ts t;

private:
	I2c i2c;
	uint8_t dectobcd(const uint8_t val);
	uint8_t bcdtodec(const uint8_t val);

};

#endif

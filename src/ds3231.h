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

#define DS3231_A1IE     0x1
#define DS3231_A2IE     0x2
#define DS3231_INTCN    0x4
#define DS3231_RS1	0x8
#define DS3231_RS2	0x10
#define DS3231_BBSQW    0x80

#define DS3231_A1F      0x1
#define DS3231_A2F      0x2
#define DS3231_OSF      0x80

#include <avr/io.h>
#include "I2C.h"

struct ts {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t mday;
    uint8_t mon;
    int16_t year;
    uint8_t wday;
    uint8_t yday;
    uint8_t isdst;
    uint8_t year_s;
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

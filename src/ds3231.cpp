/*
 * RTC DS3231 Library with time struct
 *
 * Created by Kevin Luedemann
 * 10.09.2017 16:41
 */

#include "ds3231.h"

ds3231::ds3231(){
    t.sec   = 0;
    t.min   = 43;
    t.hour  = 13;
    t.mday  = 15;
    t.mon   = 9;
    t.year  = 2017;
    t.wday  = 5;
    t.yday  = 0;
    t.isdst = 0;
    t.year_s= 0;
    get();
}

ds3231::~ds3231(){

}

void ds3231::get(){
    int8_t TimeDate[7];        //second,minute,hour,dow,day,month,year
    //uint8_t n[7];
    int8_t century = 0;
    int8_t i, n;
    int16_t year_full;

    i2c.twi_start();
    i2c.twi_write(DS3231_I2C_ADDR | I2C_WRITE);
    i2c.twi_write(DS3231_TIME_CAL_ADDR);

    i2c.twi_start();
    i2c.twi_write(DS3231_I2C_ADDR | I2C_READ);
    for (i = 0; i <= 6; i++) {
        if(i<6){
            n = i2c.twi_read(1);
        }
        else{
            n = i2c.twi_read(0);
        }
        if (i == 5) {
            TimeDate[i] = bcdtodec(n & 0x1F);
            century = (n & 0x80) >> 7;
        } else
            TimeDate[i] = bcdtodec(n);
    }

    if (century == 1) {
        year_full = 2000 + TimeDate[6];
    } else {
        year_full = 1900 + TimeDate[6];
    }
    i2c.twi_stop();

    t.sec       = TimeDate[0];
    t.min       = TimeDate[1];
    t.hour      = TimeDate[2];
    t.mday      = TimeDate[4];
    t.mon       = TimeDate[5];
    t.year      = year_full;
    t.wday      = TimeDate[3];
    t.year_s    = TimeDate[6];
}

void ds3231::set(){
    int8_t i, century;

    if (t.year >= 2000) {
        century = 0x80;
        t.year_s = t.year - 2000;
    } else {
        century = 0;
        t.year_s = t.year - 1900;
    }

    int8_t TimeDate[7] = { t.sec, t.min, t.hour, t.wday, t.mday, t.mon, t.year_s };

    i2c.twi_start();
    i2c.twi_write(DS3231_I2C_ADDR);
    i2c.twi_write(DS3231_TIME_CAL_ADDR);
    for (i = 0; i <= 6; i++) {
        TimeDate[i] = dectobcd(TimeDate[i]);
        if (i == 5){
            TimeDate[i] += century;
        }
        i2c.twi_write(TimeDate[i]);
    }
    i2c.twi_stop();

}

void ds3231::activate_sqm(){
    uint8_t reg;
    i2c.twi_start();
    i2c.twi_write(DS3231_I2C_ADDR | I2C_WRITE);
    i2c.twi_write(DS3231_CONTROL_ADDR);

    i2c.twi_start();
    i2c.twi_write(DS3231_I2C_ADDR | I2C_READ);
    reg = i2c.twi_read(0);
    i2c.twi_stop();

    i2c.twi_start();
    i2c.twi_write(DS3231_I2C_ADDR | I2C_WRITE);
    i2c.twi_write(DS3231_CONTROL_ADDR);
    reg |=  (DS3231_BBSQW);
    reg &= ~(DS3231_RS1);
    reg &= ~(DS3231_RS2);
    reg &= ~(DS3231_INTCN);
    i2c.twi_write(reg);
    i2c.twi_stop();
}
void ds3231::deactivate_sqm(){
    uint8_t reg;
    i2c.twi_start();
    i2c.twi_write(DS3231_I2C_ADDR | I2C_WRITE);
    i2c.twi_write(DS3231_CONTROL_ADDR);

    i2c.twi_start();
    i2c.twi_write(DS3231_I2C_ADDR | I2C_READ);
    reg = i2c.twi_read(0);
    i2c.twi_stop();

    i2c.twi_start();
    i2c.twi_write(DS3231_I2C_ADDR | I2C_WRITE);
    i2c.twi_write(DS3231_CONTROL_ADDR);
    i2c.twi_write(reg&(~DS3231_BBSQW));
    i2c.twi_stop();
}

inline uint8_t ds3231::dectobcd(const uint8_t val)
{
    return ((val / 10 * 16) + (val % 10));
}
inline uint8_t ds3231::bcdtodec(const uint8_t val)
{
    return ((val / 16 * 10) + (val % 16));
}

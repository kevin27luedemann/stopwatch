/*
 * Created by Kevin Luedemann
 * 09.09.2017 12:27
 */

#include "SPI.h"

Spi::Spi(){
    DDRB |= (1<<DDB3) | (1<<DDB5);
    SPCR = (1<<SPE) | (1<<MSTR);
    SPSR = (1<<SPI2X);
}

Spi::~Spi(){
    SPCR = 0;
    SPSR = 0;

}

void Spi::transfer(uint8_t d){
    SPDR = d;
    asm volatile("nop");
    while (!(SPSR & (1<<SPIF)));
}

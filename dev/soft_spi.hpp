/*
 * Atmega328 Soft SPI (bit-bang) library
 * Relies on Ublink Atmega328 register and peripherals framework
 *
 * Documentation available in the provided MD file.
 *
 * (c) 2026 Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

#ifndef AVRSOFTSPI_H
#define AVRSOFTSPI_H

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr_gpio.h>
//#include <macros.h>

class SoftSpi {
private:
    DigitalPin mosi, miso, sck, cs;

public:
    SoftSpi(DigitalPin mosi, DigitalPin miso, DigitalPin sck, DigitalPin cs) : 
            mosi(mosi), miso(miso), sck(sck), cs(cs) {}

    inline uint8_t transfer(uint8_t dat){
        uint8_t rx = 0;
        for (uint8_t i = 0; i < 8; i++) {
            mosi = dat >> 7;
            sck.high();
            rx = (rx << 1) | miso.read();
            sck.low();
            dat <<= 1;
        }
        return rx;
    }

}



#endif // AVRSOFTSPI_H
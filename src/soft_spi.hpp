/*
 * Atmega328 Soft SPI (bit-bang) library
 * Relies on Ublink Atmega328 register and peripherals framework
 *
 * Documentation available in the provided MD file spi.md.
 *
 * (c) 2026 Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

/* approx speeds
 * Fast (PORT)	        ~8–10	~1.6–2.0 MHz    ~DIV8
 * Slow (DigitalPin)	~40–80	~0.2–0.4 MHz    ~DIV32-DIV64
 */

#ifndef AVRSOFTSPI_H
#define AVRSOFTSPI_H

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr_gpio.h>
//#include <macros.h>

#define SS_PIN_REG(port) (*(port - 2))
#define SS_DDR_REG(port) (*(port - 1))

// the following macro may be redefined by the user to make a slower SoltSpi
#ifndef SOFTSPI_DELAY
#define SOFTSPI_DELAY() do {} while(0)
#endif // SOFTSPI_DELAY


class SoftSpi {
private:
    DigitalPin& mosi, miso, sck, cs;

public:
    SoftSpi(DigitalPin& mosi, DigitalPin& miso, DigitalPin& sck, DigitalPin& cs) : 
            mosi(mosi), miso(miso), sck(sck), cs(cs) {}

// interface (all except speed produce errors, speed is kept for compatibility, but does nothing)

	inline SoftSpi& LSBfirst() = delete;
	inline SoftSpi& MSBfirst() = delete;
	inline SoftSpi& polarity(uint8_t pol) = delete;
	inline SoftSpi& phase(uint8_t ph) = delete;
	inline SoftSpi& clock(uint8_t dvd) {return *this;}
	inline SoftSpi& speed2x() {return *this;}
	inline SoftSpi& IRQenable() = delete;
	inline SoftSpi& IRQdisable() = delete;

    inline void enable() {
        mosi.mode(OUTPUT);
        sck.mode(OUTPUT);
        cs.mode(OUTPUT);
        miso.mode(INPUT);
		end(); // SS high to make sure slave sees the beginning of communications
	}

	inline void disable() {mosi.mode(INPUT);sck.mode(INPUT);end();}
    
// continuous mode

	inline SoftSpi& begin() {cs.low(); return *this;}
	inline SoftSpi& begin(DigitalPin cs_pin) {cs = cs_pin; cs.low(); return *this;}
	inline void end() {cs.high();}
	inline void latch() {cs.high(); cs.low();}

    // send and receive single byte
    inline uint8_t transfer(uint8_t dat){
        uint8_t rx = 0;
        for (uint8_t i = 0; i < 8; i++) {
            mosi.write(dat & 0x80);
            SOFTSPI_DELAY();
            sck.high();
			SOFTSPI_DELAY();
            rx = (rx << 1) | miso.read();            
            sck.low();
			SOFTSPI_DELAY();
            dat <<= 1;
        }
        return rx;
    }

    // send and receive an array of bytes of length len. Chainable
	inline SoftSpi& transfer(const uint8_t* tx, uint8_t* rx, uint16_t len) {
		for (uint16_t i = 0; i < len; ++i) { 
			uint8_t r = transfer(tx ? tx[i] : 0xFF); // use dummy if no tx
			if (rx) rx[i] = r;
		}
		return *this;
	}

    // send single byte, chainable, doesn't touch CS
	inline SoftSpi& write(uint8_t dat){
        uint8_t _ = transfer(dat);
        (void)_;
	    return *this;
	}

	// send an array of bytes, chainable, doesn't touch CS
	inline SoftSpi& write(const uint8_t* dat, uint16_t len){
		transfer(dat, nullptr, len);
		return *this;
	}

	inline uint8_t read(){
		return transfer(0xFF);		
    }

	inline SoftSpi& read(uint8_t& received){
		received = transfer(0xFF);	
		return *this;	
    }

    // big read
	inline SoftSpi& read(uint8_t* dat, uint16_t len){
        transfer(nullptr, dat, len);
		return *this;
	}

// transactions mode

	inline uint8_t single(uint8_t dat){
		uint8_t res;
		begin();
		res = transfer(dat);
		end();
		return res;
	}

	inline void writeStream(const uint8_t* dat, uint16_t len){
		begin();
		transfer(dat, nullptr, len);
		end();
	}
	
	inline void readStream(uint8_t* dat, uint16_t len){
		begin();
		read(dat, len);
		end();
	}
	
	inline void transferStream(const uint8_t* arr_out, uint8_t* arr_in, uint16_t len){
		begin();
		transfer(arr_out, arr_in, len);
		end();
	}

	inline void seqTransfer(const uint8_t* arr_out, uint16_t num_out, uint8_t* arr_in, uint16_t num_in){
		begin();
		transfer(arr_out, nullptr, num_out);
		transfer(nullptr, arr_in, num_in);
		end();
	}
};

// aliaces for compatibility with older code
using SoftSpiSlave  	= SoftSpi;

#endif // AVRSOFTSPI_H
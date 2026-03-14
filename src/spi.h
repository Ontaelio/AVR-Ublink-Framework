/*
 * Atmega328 8-bit SPI library
 * Part of Ublink Atmega328 register and peripherals framework
 *
 * Documentation available in the provided MD file.
 *
 * (c) 2021-... Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

#ifndef AVRSPI_H
#define AVRSPI_H

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr_gpio.h>
#include <macros.h>


class SpiSlave{
private:
    uint8_t cfg = 0; //(1 << MSTR);
    digitalPin ssPin;
public:
	SpiSlave(digitalPin gpin = digitalPin(PORTB, 2, OUTPUT)): ssPin(gpin) {}

// interface

	inline SpiSlave& LSBfirst() {cfg |= (1 << DORD); return *this;}
	inline SpiSlave& MSBfirst() {cfg &= ~(1 << DORD); return *this;}
	inline SpiSlave& polarity(uint8_t pol) {
		cfg = (cfg & ~(1 << CPOL)) | ((pol ? 1 : 0) << CPOL); return *this;}
	inline SpiSlave& phase(uint8_t ph) {
		cfg = (cfg & ~(1 << CPHA)) | ((ph ? 1 : 0) << CPHA); return *this;}
	inline SpiSlave& clock(uint8_t dvd) {
		cfg = ((cfg & ~(0x03)) | dvd); return *this;}
	inline SpiSlave& speed2x() {cfg |= (1 << MSTR); return *this;} // 2x saved in MSTR bit for cfg

	inline SpiSlave& IRQenable() {cfg |= (1 << SPIE); SPCR |= (1 << SPIE); return *this;}
	inline SpiSlave& IRQdisable() {cfg &= ~(1 << SPIE); SPCR &= ~(1 << SPIE); return *this;}

	inline void enable() {
		DDRB |= ((1 << PB5) | (1 << PB3) | (1 << PB2)); // SCK, MOSI, SS = output (even if not used!)
    	DDRB &=  ~(1 << PB4); // MISO = input
		end(); // SS high to make sure slave sees the beginning of communications
		if (cfg & 0x10) SPSR |= 0x01; // set 2x from cfg
		else SPSR &= ~0x01; // disable 2x		
		SPCR = (1 << SPE) | (1 << MSTR) | (cfg & 0xEF); 
	}

	inline void enable(uint8_t conf) {cfg = conf; enable();}
	inline void disable() {end(); SPCR &= ~(1 << SPE);}

// continuous mode

	inline SpiSlave& begin() {ssPin.low(); return *this;}
	inline void end() {ssPin.high();}
	inline void latch() {ssPin.high(); ssPin.low();}

    // send and receive single byte
	inline uint8_t transfer(uint8_t dat){
		SPDR = dat;
        while (!(SPSR & (1<<SPIF))) {}
		return SPDR;		
    }

	// send and receive an array of bytes of length len. Chainable
	inline SpiSlave& transfer(const uint8_t* tx, uint8_t* rx, uint8_t len) {
		for (uint8_t i = 0; i < len; ++i) {
			uint8_t r = transfer(tx ? tx[i] : 0xFF); // use dummy if no tx
			if (rx) rx[i] = r;
		}
		return *this;
	}

	// same for bigger arrays
	inline SpiSlave& transfer16(const uint8_t* tx, uint8_t* rx, uint16_t len) {
		for (uint16_t i = 0; i < len; ++i) {
			uint8_t r = transfer(tx ? tx[i] : 0xFF); // use dummy if no tx
			if (rx) rx[i] = r;
		}
		return *this;
	}

	// send single byte, chainable, doesn't touch CS
	inline SpiSlave& write(uint8_t dat){
	    SPDR = dat; //send a byte
	    while (!(SPSR & (1<<SPIF))) {} //wait until it's sent
		volatile uint8_t _ = SPDR; //must access SPDR to clear flag
		(void)_; //avoid compiler warnings and make sure SPDR was read
        return *this;
	}

	// send an array of bytes, chainable, doesn't touch CS
	inline SpiSlave& write(const uint8_t* dat, uint8_t len){
		transfer(dat, nullptr, len);
		return *this;
	}

	// same for bigger arrays
	inline SpiSlave& write16(const uint8_t* dat, uint16_t len){
		transfer16(dat, nullptr, len);
		return *this;
	}
	
	inline uint8_t read(){
		SPDR = 0xFF;
        while (!(SPSR & (1<<SPIF))) {}
		return SPDR;		
    }

	inline SpiSlave& read(uint8_t* dat, uint8_t len){
		transfer(nullptr, dat, len);
		return *this;
	}

	inline SpiSlave& read16(uint8_t* dat, uint16_t len){
		transfer16(nullptr, dat, len);
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
		transfer(nullptr, dat, len);
		end();
	}
	
	inline void transferStream(const uint8_t* arr_out, uint8_t* arr_in, uint16_t len){
		begin();
		transfer(arr_out, arr_in, len);
		end();
	}

	inline void seqTransfer(const uint8_t* arr_out, uint16_t num_out, uint8_t* arr_in, uint16_t num_in){
		begin();
		transfer16(arr_out, nullptr, num_out);
		transfer16(nullptr, arr_in, num_in);
		end();
	}

// ISR mode

	SpiSlave& operator= (const uint8_t& dat) {SPDR = dat; return *this;}
	operator uint16_t() {return SPDR;}

// legacy / deprecated

	[[deprecated("not needed, use enable(conf)")]]
	inline void init(uint8_t conf) {cfg = conf;}

	[[deprecated("use begin()")]]
	inline void ss_low() {ssPin.low();}

	[[deprecated("use end()")]]
	inline void ss_high() {ssPin.high();}
		
	[[deprecated("use write()")]]
	void writeData(uint8_t dat)	{write(dat);}	
	
	[[deprecated("use read()")]]
	uint16_t readData(){return read();}
};


#endif // AVRSPI_H
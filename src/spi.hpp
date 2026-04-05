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


class Spi{
private:
    uint8_t cfg = 0; //(1 << MSTR);
    DigitalPin ssPin;	

public:
	Spi() : ssPin(PORTB, 2, OUTPUT) {}
    Spi(DigitalPin gpin) : ssPin(gpin) {}
	//Spi(digitalPin gpin = digitalPin(PORTB, 2, OUTPUT)): ssPin(gpin) {}

// interface

	inline Spi& LSBfirst() {cfg |= (1 << DORD); return *this;}
	inline Spi& MSBfirst() {cfg &= ~(1 << DORD); return *this;}
	inline Spi& polarity(uint8_t pol) {
		cfg = (cfg & ~(1 << CPOL)) | ((pol ? 1 : 0) << CPOL); return *this;}
	inline Spi& phase(uint8_t ph) {
		cfg = (cfg & ~(1 << CPHA)) | ((ph ? 1 : 0) << CPHA); return *this;}
	inline Spi& clock(uint8_t dvd) {
		cfg = ((cfg & ~(0x03)) | dvd); return *this;}
	inline Spi& speed2x() {cfg |= (1 << MSTR); return *this;} // 2x saved in MSTR bit for cfg

	inline Spi& IRQenable() {cfg |= (1 << SPIE); SPCR |= (1 << SPIE); return *this;}
	inline Spi& IRQdisable() {cfg &= ~(1 << SPIE); SPCR &= ~(1 << SPIE); return *this;}

	inline void enable() {
		DDRB |= ((1 << PB5) | (1 << PB3) | (1 << PB2)); // SCK, MOSI, SS = output (even if not used!)
    	DDRB &=  ~(1 << PB4); // MISO = input
		end(); // SS high to make sure slave sees the beginning of communications
		if (cfg & 0x10) SPSR |= 0x01; // set 2x from cfg
		else SPSR &= ~0x01; // disable 2x		
		SPCR = (1 << SPE) | (1 << MSTR) | (cfg & 0xEF); 
		clear(); // clear the flag just in case
	}

	inline void enable(uint8_t conf) {cfg = conf; enable();}
	inline void disable() {end(); SPCR &= ~(1 << SPE);}

// fluent interface (method chaining)

	inline Spi& begin() {ssPin.low(); return *this;}
	inline Spi& begin(DigitalPin cs_pin) {ssPin = cs_pin; ssPin.low(); return *this;}
	inline void end() {ssPin.high();}
	inline Spi& latch() {ssPin.high(); ssPin.low(); return *this;}

    // send and receive single byte
	inline uint8_t transfer(uint8_t dat){
		SPDR = dat;
        while (!(SPSR & (1<<SPIF))) {}
		return SPDR;		
    }

	// send and receive an array of bytes of length len. Chainable, pipelined
	inline Spi& transfer(const uint8_t* tx, uint8_t* rx, uint16_t len) {
		if (!len) return *this;
		SPDR = tx[0];
		for (uint16_t i = 1; i < len; ++i) {
			while (!(SPSR & (1<<SPIF)));
			uint8_t r = SPDR;
			SPDR = tx[i];
			rx[i-1] = r;
		}
		while (!(SPSR & (1<<SPIF)));
		rx[len-1] = SPDR;		
		return *this;
	}

	// send single byte, chainable, doesn't touch CS
	inline Spi& write(uint8_t dat){
	    SPDR = dat; //send a byte
	    while (!(SPSR & (1<<SPIF))) {} //wait until it's sent
		(void)SPDR;
		//volatile uint8_t _ = SPDR; //must access SPDR to clear flag
		//(void)_; //avoid compiler warnings and make sure SPDR was read
        return *this;
	}

	// send an array of bytes, chainable
	inline Spi& write(const uint8_t* tx, uint16_t len){
		if (!len) return *this;
		SPDR = tx[0];
		for (uint16_t i = 1; i < len; ++i) {
			while (!(SPSR & (1<<SPIF)));
			(void)SPDR;   
			SPDR = tx[i];
		}
		while (!(SPSR & (1<<SPIF)));
		(void)SPDR;
		return *this;
	}

	inline uint8_t read(){
		SPDR = 0xFF;
        while (!(SPSR & (1<<SPIF))) {}
		return SPDR;		
    }

	inline Spi& read(uint8_t& received){
		SPDR = 0xFF;
        while (!(SPSR & (1<<SPIF))) {}
		received = SPDR;	
		return *this;	
    }

	// big read, pipelined
	inline Spi& read(uint8_t* dat, uint16_t len){
		uint8_t* p = dat;
		if (!len) return *this;
		SPDR = 0xFF;
		while (--len){
			while (!(SPSR & (1<<SPIF)));
			uint8_t r = SPDR;   
			SPDR = 0xFF;
			*p++ = r;			
		}
		while (!(SPSR & (1<<SPIF)));
		*p = SPDR;
		return *this;
	}

// transaction interface

	inline uint8_t single(uint8_t dat){
		uint8_t res;
		begin();
		res = transfer(dat);
		end();
		return res;
	}

	inline void writeStream(const uint8_t* dat, uint16_t len){
		begin();
		write(dat, len);
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
		write(arr_out, num_out);
		read(arr_in, num_in);
		end();
	}

// ISR mode

	inline Spi& operator= (const uint8_t& dat) {SPDR = dat; return *this;}
	inline operator uint8_t() {void(SPSR); return SPDR;}
	inline void clear() {(void)SPSR; (void)SPDR;}		

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

class SpiNode{
private:
    uint8_t cfg = 0; 

public:
	SpiNode() {}
    
// interface

	inline SpiNode& LSBfirst() {cfg |= (1 << DORD); return *this;}
	inline SpiNode& MSBfirst() {cfg &= ~(1 << DORD); return *this;}
	inline SpiNode& polarity(uint8_t pol) {
		cfg = (cfg & ~(1 << CPOL)) | ((pol ? 1 : 0) << CPOL); return *this;}
	inline SpiNode& phase(uint8_t ph) {
		cfg = (cfg & ~(1 << CPHA)) | ((ph ? 1 : 0) << CPHA); return *this;}

	inline SpiNode& IRQenable() {cfg |= (1 << SPIE); SPCR |= (1 << SPIE); return *this;}
	inline SpiNode& IRQdisable() {cfg &= ~(1 << SPIE); SPCR &= ~(1 << SPIE); return *this;}

	inline void enable(uint8_t firstByte = 255) {
		DDRB &= ~((1 << PB5) | (1 << PB3) | (1 << PB2)); // SCK, MOSI, SS = input
    	DDRB |=  (1 << PB4); // MISO = output
		SPCR = (1 << SPE) | (cfg & 0xEF); 
		clear(); // clear the flag just in case
		SPDR = firstByte;
	}

	inline void disable() {SPCR &= ~(1 << SPE);}

// ISR mode

	inline SpiNode& operator= (const uint8_t& dat) {SPDR = dat; return *this;}
	inline operator uint8_t() {void(SPSR); return SPDR;}
	inline void clear() {(void)SPSR; (void)SPDR;}	
	
	inline uint8_t transfer(uint8_t nextByte = 255){
		uint8_t received = SPDR;
		SPDR = nextByte;
		return received;
	}

// polling mode

	uint8_t await(uint8_t nextByte = 255){
		while (!(SPSR & (1<<SPIF))) {}
		uint8_t received = SPDR;
		SPDR = nextByte;
		return received;
	}
};

// aliaces for compatibility with older code
using SpiSlave  	= Spi;
using SpiDevice     = Spi;
using SpiMaster		= SpiNode;

#endif // AVRSPI_H
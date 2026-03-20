/*
 * Atmega328 USART-SPI library
 * Part of Ublink Atmega328 register and peripherals framework
 *
 * Documentation available in the provided MD file.
 *
 * (c) 2026 Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

#ifndef AVRUSARTSPI_H
#define AVRUSARTSPI_H

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr_gpio.h>
#include <macros.h>


class UsartSpiSlave{
private:
    uint8_t cfg = 0; //(1 << MSTR);
    uint16_t divider = 1;
    digitalPin ssPin;
public:
	UsartSpiSlave(digitalPin gpin): ssPin(gpin) {}

// interface

	inline UsartSpiSlave& LSBfirst() {cfg |= (1 << DORD); return *this;}
	inline UsartSpiSlave& MSBfirst() {cfg &= ~(1 << DORD); return *this;}
	inline UsartSpiSlave& polarity(uint8_t pol) {
		cfg = (cfg & ~(1 << CPOL)) | ((pol ? 1 : 0) << CPOL); return *this;}
	inline UsartSpiSlave& phase(uint8_t ph) {
		cfg = (cfg & ~(1 << CPHA)) | ((ph ? 1 : 0) << CPHA); return *this;}
	inline UsartSpiSlave& clock(uint16_t dvd) {
		divider = dvd; return *this;}
    //[[deprecated("No 2x in USART SPI, method ignored")]]
	inline UsartSpiSlave& speed2x() {return *this;} // 2x not present in USART SPI

	inline UsartSpiSlave& IRQenable() {cfg |= (1 << SPIE); SPCR |= (1 << SPIE); return *this;}
	inline UsartSpiSlave& IRQdisable() {cfg &= ~(1 << SPIE); SPCR &= ~(1 << SPIE); return *this;}

	inline void enable() {
		DDRD |= ((1 << PD4) | (1 << PD1)); // SCK, MOSI)
    	DDRD &=  ~(1 << PD0); // MISO = input
		end(); // SS high to make sure slave sees the beginning of communications
        UCSR0C = ((cfg >> 3) & (1<<UDORD0)); //LSB/MSB, clearing the rest
        UCSR0C = (UCSR0C & ~(1<<UCPOL0)) | ((cfg >> 3) & (1<<UCPOL0)); //polarity
        UCSR0C = (UCSR0C & ~(1<<UCPHA0)) | ((cfg >> 1) & (1<<UCPHA0)); //phase
        UBRR0L = (divider & 0xFF); //clock lower bits
        UBRR0H = (divider & 0xF00) >> 8; // clock high bits
        UCSR0B = (cfg & (1<<SPIE)); //IRQ on RX, same bit position, clear the rest
		UCSR0C |= (1<<UMSEL01) | (1<<UMSEL00); // select Master SPI mode
        UCSR0B = (1<<RXEN0) | (1<<TXEN0); // enable RX and TX
        volatile uint8_t _ = UDR0; // make sure we clear the byte there to avoid errors
        (void)_;
	}

	inline void enable(uint8_t conf) {cfg = conf; enable();}
	inline void disable() {end(); UCSR0B &= ~((1<<RXEN0) | (1<<TXEN0));}

// continuous mode

	inline UsartSpiSlave& begin() {ssPin.low(); return *this;}
	inline void end() {ssPin.high();}
	inline void latch() {ssPin.high(); ssPin.low();}

    // send and receive single byte
	inline uint8_t transfer(uint8_t dat){
		UDR0 = dat;                          
        while (!(UCSR0A & (1<<RXC0)));
        return UDR0; 		
    }

	// send and receive an array of bytes of length len. Chainable
	inline UsartSpiSlave& transfer(const uint8_t* tx, uint8_t* rx, uint16_t len) {
		if (!len) return *this;
		UDR0 = tx[0];
		for (uint16_t i = 1; i < len; ++i) {
			while (!(UCSR0A & (1<<RXC0)));
			uint8_t r = UDR0;
			UDR0 = tx[i];
			rx[i-1] = r;
		}
		while (!(UCSR0A & (1<<RXC0)));
		rx[len-1] = UDR0;
		return *this;
	}

	// send single byte, chainable, doesn't touch CS
	inline UsartSpiSlave& write(uint8_t dat){
	    UDR0 = dat; //send a byte
	    while (!(UCSR0A & (1<<RXC0))) {} //wait until it's sent
		(void)UDR0;
		//volatile uint8_t _ = UDR0; //must read UDR0 to clear RXC0 NOT NEEDED
		//(void)_; //avoid compiler warnings and make sure UDR was read
        return *this;
	}

	// send an array of bytes, chainable, doesn't touch CS
	inline UsartSpiSlave& write(const uint8_t* tx, uint16_t len){
		if (!len) return *this;
		UDR0 = tx[0];
		for (uint16_t i = 1; i < len; ++i) {
			while (!(UCSR0A & (1<<RXC0)));
			(void)UDR0;
			UDR0 = tx[i];
		}
		while (!(UCSR0A & (1<<RXC0)));
		(void)UDR0;
		return *this;
	}

	inline uint8_t read(){
		UDR0 = 0xFF;                          
        while (!(UCSR0A & (1<<RXC0)));
        return UDR0;		
    }

	// big reads are pipelined, unlike transfer
	inline UsartSpiSlave& read(uint8_t* rx, uint16_t len){
		if (!len) return *this;
		uint8_t *p = rx;

		// prefill
		while (!(UCSR0A & (1<<UDRE0)));
		UDR0 = 0xFF;

		if (--len){
			while (!(UCSR0A & (1<<UDRE0)));
			UDR0 = 0xFF;
			while (--len) {
				while (!(UCSR0A & (1<<RXC0)));
				*p++ = UDR0;
				UDR0 = 0xFF;
			}
			while (!(UCSR0A & (1<<RXC0)));
			*p++ = UDR0;
			}

		while (!(UCSR0A & (1<<RXC0)));
		*p++ = UDR0;
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

	UsartSpiSlave& operator= (const uint8_t& dat) {UDR0 = dat; return *this;}
	operator uint8_t() {return UDR0;}

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


#endif // AVRUSARTSPI_H
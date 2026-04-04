/*
 * Atmega328 Pin library header file
 * Part of Ublink Atmega328 register and peripherals framework
 *
 * Documentation available in the provided MD file.
 *
 * (c) 2021-... Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

#ifndef ATMEGA328_PINS_H_
#define ATMEGA328_PINS_H_

#include <stdint.h>
#include <avr/io.h>
#include <macros.h>

/* ===================================================
 * Digital Pins
 * =================================================== */

class DigitalPin{
private:
	volatile uint8_t* pinx;
	volatile uint8_t* portx;
	uint8_t mask;

public:
	DigitalPin(volatile uint8_t& prt, uint8_t pn, uint8_t mode = INPUT);
	DigitalPin(uint8_t arduinoPin, uint8_t mode = INPUT);
	inline void high()  {*portx |= mask;}
    inline void low()   {*portx &= ~mask;}
    inline void set()   {high();}
    inline void reset() {low();}
    
    inline void write(uint8_t val) {if (val) *portx |= mask; else *portx &= ~mask;}
    inline uint8_t read() const {return (*pinx & mask) != 0;}
	inline void invert() {*pinx = mask;} // toggle via PINx

	void mode(uint8_t md);
	
	void pinChangeIRQ(uint8_t c = 1);
	void externalIRQ(uint8_t c);

	operator uint8_t() {return read();}
	DigitalPin& operator= (const uint8_t& a) {write(a); return *this;}
};

// legacy compatibility
using digitalPin = DigitalPin;

/* ===================================================
 * Analog Pins
 * =================================================== */

class AnalogPin
{
private:
	uint8_t pinnum;
public:
	AnalogPin(uint8_t pn);
	void init(uint8_t ps = ADCPS128);
	uint16_t read();
	void start();
	void stop();
	uint16_t check();
	
	void AREF(uint8_t m = 0);
	void digitalEnable();
	
	operator uint16_t() {return check();}
};

// legacy compatibility
using analogPin = AnalogPin;

/* ===================================================
 * PWM Pins
 * =================================================== */

class PwmPin
{
private:
	volatile uint8_t* ocr;
	volatile uint8_t* tccra;
	uint8_t pinnum;
public:
	PwmPin(uint8_t pn);
	void init();
	void write(uint8_t val) {*ocr = val;}
	
	operator uint8_t() {return (*ocr);}
	PwmPin& operator= (const uint8_t& a) {write(a); return *this;}
	PwmPin& operator++ () {write ((*ocr) + 1); return *this;} // prefix
	uint8_t operator++ (int) {uint8_t old = *ocr; write (old + 1); return old;} // postfix
	PwmPin& operator-- () {write ((*ocr) - 1); return *this;}
	uint8_t operator-- (int) {uint8_t old = *ocr; write (old - 1); return old;}
	PwmPin& operator+= (const uint16_t& a) {write ((*ocr) + a); return *this;}
	PwmPin& operator-= (const uint16_t& a) {write ((*ocr) - a); return *this;}
};

// Inverted PWM pin, old version for convenience (cathode connected to pin)
class PwmPinInv
{
private:
	volatile uint8_t* ocr;
	volatile uint8_t* tccra;
	uint8_t pinnum;
public:
	PwmPinInv(uint8_t pn);
	void init();
	void write(uint8_t val) {*ocr = val;}
	
	operator uint8_t() {return (*ocr);}
	PwmPinInv& operator= (const uint8_t& a) {write(a); return *this;}
	PwmPinInv& operator++ () {write ((*ocr) + 1); return *this;} // prefix
	uint8_t operator++ (int) {uint8_t old = *ocr; write (old + 1); return old;} // postfix
	PwmPinInv& operator-- () {write ((*ocr) - 1); return *this;}
	uint8_t operator-- (int) {uint8_t old = *ocr; write (old - 1); return old;}
	PwmPinInv& operator+= (const uint16_t& a) {write ((*ocr) + a); return *this;}
	PwmPinInv& operator-= (const uint16_t& a) {write ((*ocr) - a); return *this;}
};

// legacy compatibility
using pwmPin = PwmPin;
using pwmPinInv = PwmPinInv;


/* ===================================================
 * Digital Pins Standalone Functions
 * =================================================== */

/* pin change interrupts */
inline void pinChangeBenable() {PCICR |= (1 << PCIE0);}
inline void pinChangeCenable() {PCICR |= (1 << PCIE1);}
inline void pinChangeDenable() {PCICR |= (1 << PCIE2);}
inline void pinChangeBdisable() {PCICR &= ~(1 << PCIE0);}
inline void pinChangeCdisable() {PCICR &= ~(1 << PCIE1);}
inline void pinChangeDdisable() {PCICR &= ~(1 << PCIE2);}

/* pin change masks */
inline void pinChangeBmask(uint8_t mask) {PCMSK0 = mask;}
inline void pinChangeCmask(uint8_t mask) {PCMSK1 = mask;}
inline void pinChangeDmask(uint8_t mask) {PCMSK2 = mask;}

/* pin change flags clear */
inline void pinChangeBclear() {PCIFR |= (1 << PCIF0);}
inline void pinChangeCclear() {PCIFR |= (1 << PCIF1);}
inline void pinChangeDclear() {PCIFR |= (1 << PCIF2);}

/* enable external interrupts INT0 and INT1 */
inline void int0enable() {EIFR  |=  (1 << INTF0); EIMSK |=  (1 << INT0);}
inline void int1enable() {EIFR  |=  (1 << INTF1); EIMSK |=  (1 << INT1);;}
inline void int0disable() {EIMSK &= ~(1 << INT0);}
inline void int1disable() {EIMSK &= ~(1 << INT1);}

/* external interrupt triggers, pins D2 (INT0) and D3 (INT1) */
inline void int0setup(uint8_t val) {EICRA = (EICRA & ~0x03) | val;} //clear and set	
inline void int1setup(uint8_t val) {EICRA = (EICRA & ~0x0C) | (val << 2);} //clear and set

/* external interrupts flags clear*/
inline void int0clear() {EIFR |= (1 << INTF0);}
inline void int1clear() {EIFR |= (1 << INTF1);}

/* built-in pull-up disable*/
inline void pullupDisable() {MCUCR |= 1<<PUD;}
inline void pullupEnable() {MCUCR &= ~(1<<PUD);}

#endif // ATMEGA328_PINS_H_
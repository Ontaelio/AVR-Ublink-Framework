/*
 * Atmega328 Pin functions header file
 * Part of Atmega328 register and peripherals library
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

class digitalPin
{
private:
  volatile uint8_t* pinx;
  volatile uint8_t* portx;
  uint8_t pinnum;

public:
  //digitalPin(uint8_t* prt, uint8_t pn, uint8_t mode);
  digitalPin(volatile uint8_t& prt, uint8_t pn, uint8_t mode);
  void high();
  void set();
  void low();
  void reset();
  void mode(uint8_t md);
  uint8_t read();
  void write(uint8_t val);
  void invert();
  
  void pinChangeIRQ(uint8_t c = 1);
  void externalIRQ(uint8_t c);

  operator uint8_t() {return read();}
  digitalPin& operator= (const uint8_t& a) {write(a); return *this;}
};

/* ===================================================
 * Analog Pins
 * =================================================== */

class analogPin
{
private:
	uint8_t pinnum;
public:
	analogPin(uint8_t pn);
	void init(uint8_t ps = ADCPS128);
	uint16_t read();
	void start();
	void stop();
	uint16_t check();
	
	void AREF(uint8_t m = 0);
	void digitalEnable();
	
	operator uint16_t() {return check();}
};

/* ===================================================
 * PWM Pins
 * =================================================== */

class pwmPin
{
private:
	volatile uint8_t* ocr;
	volatile uint8_t* tccra;
	uint8_t pinnum;
public:
	pwmPin(uint8_t pn);
	void init();
	void write(uint8_t val);
	
	operator uint8_t() {return ~(*ocr);}
	pwmPin& operator= (const uint8_t& a) {write(a); return *this;}
	pwmPin& operator++ () {write (~(*ocr) + 1); return *this;} // prefix
	pwmPin operator++ (int) {write (~(*ocr) + 1); return *this;} // postfix
	pwmPin& operator-- () {write (~(*ocr) - 1); return *this;}
	pwmPin operator-- (int) {write (~(*ocr) - 1); return *this;}
	pwmPin& operator+= (const uint16_t& a) {write (~(*ocr) + a); return *this;}
	pwmPin& operator-= (const uint16_t& a) {write (~(*ocr) - a); return *this;}
};

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

/* enable external interrupts INT0 and INT1 */
inline void int0enable() {EIMSK |= 1;}
inline void int1enable() {EIMSK |= 2;}
inline void int0disable() {EIMSK &= ~1;}
inline void int1disable() {EIMSK &= ~2;}

/* external interrupt triggers, pins D2 (INT0) and D3 (INT1) */
inline void int0setup(uint8_t val) {EICRA = (EICRA & ~0x03) | val;} //clear and set	
inline void int1setup(uint8_t val) {EICRA = (EICRA & ~0x0C) | (val << 2);} //clear and set

/* built-in pull-up disable*/
inline void pullupDisable() {MCUCR |= 1<<PUD;}
inline void pullupEnable() {MCUCR &= ~(1<<PUD);}

#endif // ATMEGA328_PINS_H_
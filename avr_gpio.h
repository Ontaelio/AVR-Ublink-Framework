/*
 * Atmega328 Pin functions header file
 * Part of Atmega328 register and peripherals library
 *
 * Dpcumentation available in the provided MD file.
 *
 * (c) 2021 Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

#ifndef ATMEGA328_PINS_H_
#define ATMEGA328_PINS_H_

#include <stdint.h>
#include <avr/io.h>

#ifndef INPUT
#define INPUT 0x0
#endif

#ifndef OUTPUT
#define OUTPUT 0x1
#endif

#ifndef INPUT_PULLUP
#define INPUT_PULLUP 0x2
#endif

#define PIN_LOW		0x4
#define PIN_CHANGE	0x5
#define PIN_RISING	0x6
#define PIN_FALLING	0x7

class digitalPin
{
private:
  volatile uint8_t* pinx;
  volatile uint8_t* portx;
  uint8_t pinnum;

public:
  digitalPin(uint8_t* prt, uint8_t pn, uint8_t mode);
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

#define ADCPS128	0x7
#define ADCPS64		0x6
#define ADCPS32		0x5
#define ADCPS16		0x4
#define ADCPS8		0x3
#define ADCPS4		0x2
#define ADCPS2		0x1

#define ADC_AREF	0x0
#define ADC_AVCC	0x1
#define ADC_INT1V	0x3

class analogPin
{
private:
	uint8_t pinnum;
public:
	analogPin(uint8_t pn, uint8_t ps = ADCPS128);
	void init(uint8_t ps = ADCPS128);
	uint16_t read();
	void start();
	void stop();
	uint16_t check();
	
	void AREF(uint8_t m = 0);
	
	operator uint16_t() {return check();}
};

#define PIN6	2
#define PIND6	2
#define PIN5	3
#define PIND5	3

#define PIN9	4
#define PINB1	4
#define PIN10	5
#define PINB2	5

#define PIN11	8
#define PINB3	8
#define PIN3	9
#define PIND3	9

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
	//void enable();
	//void disable();
};

/* digital pin standalone functions */

inline void pinChangeBenable() {PCICR |= (1 << PCIE0);}
inline void pinChangeCenable() {PCICR |= (1 << PCIE1);}
inline void pinChangeDenable() {PCICR |= (1 << PCIE2);}
inline void pinChangeBdisable() {PCICR &= ~(1 << PCIE0);}
inline void pinChangeCdisable() {PCICR &= ~(1 << PCIE1);}
inline void pinChangeDdisable() {PCICR &= ~(1 << PCIE2);}


#endif // AVR_GPIO_H_
/*
 * Atmega328 GPIO functions header file
 * Part of Atmega328 register and peripherals library
 *
 * Dpcumentation available in the provided MD file.
 *
 * (c) 2021 Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

#ifndef AVR_GPIO_H_
#define AVR_GPIO_H_

#include <stdint.h>

class digitalPin
{
private:
  uint8_t* pinx;
  uint8_t* portx;
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

  operator uint8_t() {return read();}
  digitalPin& operator= (const uint8_t& a) {write(a); return *this;}
};

#endif // AVR_GPIO_H_
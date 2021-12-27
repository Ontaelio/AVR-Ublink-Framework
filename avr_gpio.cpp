/*
 * Atmega328 GPIO functions file
 * Part of Atmega328 register and peripherals library
 *
 * Dpcumentation available in the provided MD file.
 *
 * (c) 2021 Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */
 
#include <avr_gpio.h>

  digitalPin::digitalPin(uint8_t* prt, uint8_t pn, uint8_t mode)
    {pinnum = pn;
     portx = prt;
     *portx |= ((mode >> 1) & 1) << pinnum;
     uint8_t* adr = portx - 1;
     *adr |= (mode & 1) << pinnum;
     --adr;
     pinx = adr;
    }
  
  void digitalPin::high()
    {
      *portx |= 1 << pinnum;
    }

  void digitalPin::set()
    {
      *portx |= 1 << pinnum;
    }
  
  void digitalPin::low()
    {
      *portx &= ~(1 << pinnum);
    }

  void digitalPin::reset()
    {
      *portx &= ~(1 << pinnum);
    }
 
  void digitalPin::mode(uint8_t md)
    {
     *portx &= ~(1 << pinnum);
     *portx |= ((md >> 1) & 1) << pinnum;
     uint8_t* adr = pinx + 1;
     *adr &= ~(1 << pinnum);
     *adr |= (md & 1) << pinnum;
    }

  uint8_t digitalPin::read()
    {
     uint8_t val = (*pinx >> pinnum) & 1;
     return val;
    }

  void digitalPin::write(uint8_t val)
    {
  if (val) high(); else low();
    }

  void digitalPin::invert()
    {
      *pinx = 1 << pinnum;
    }

//  operator uint8_t() {return read();}
//  digitalPin& operator= (const uint8_t& a) {write(a); return *this;}


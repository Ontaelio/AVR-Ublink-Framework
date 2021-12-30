/*
 * Atmega328 Pin functions file
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
	
  void digitalPin::pinChangeIRQ(uint8_t c)
    {
		if (c)
		{
			if (portx == &PORTB) {PCICR |= 1 << PCIE0; PCMSK0 |= 1 << pinnum;}
			else if (portx == &PORTC) {PCICR |= 1 << PCIE1; PCMSK1 |= 1 << pinnum;}
			else if (portx == &PORTD) {PCICR |= 1 << PCIE2; PCMSK2 |= 1 << pinnum;}
		}
		else
		{
			if (portx == &PORTB) PCMSK0 &= ~(1 << pinnum);
			else if (portx == &PORTC) PCMSK1 &= ~(1 << pinnum);
			else if (portx == &PORTD) PCMSK2 &= ~(1 << pinnum);
		}
	}
	  
  void digitalPin::externalIRQ(uint8_t c)
	{
		if ((portx == &PORTD) && (pinnum == 2)) //INT0
			if (c)
			{
				EICRA &= ~3; // clear
				EICRA |= c & 3; // set
				EIMSK |= 1; // enable
			}
			else EIMSK &= ~1; // disable
		
		if ((portx == &PORTD) && (pinnum == 3)) 
			if (c)
			{
				EICRA &= ~12; // clear
				EICRA |= (c & 3) << 2; // set
				EIMSK |= 2; // enable
			}
			else EIMSK &= ~2; // disable
	}

//  operator uint8_t() {return read();}
//  digitalPin& operator= (const uint8_t& a) {write(a); return *this;}

  analogPin::analogPin(uint8_t pn, uint8_t ps)
	{
		pinnum = pn;
		init(ps);
	}		
		
  void analogPin::init(uint8_t ps)
	{
		ADCSRA &= ~0x7; // clear prescaler
		ADCSRA |= ps; // set prescaler
		ADMUX &= ~0xF; // clear channel selection
		ADMUX |= (1<<REFS0) | pinnum; // assign channel
		ADCSRA |= 1<<ADEN; // enable ADC
		//read();
	}
	
  uint16_t analogPin::read()
	{
		ADMUX &= ~0xF; // clear channel selection
		ADMUX |= pinnum; // assign channel
		ADCSRA |= 1<<ADSC; // start conversion
		while (ADCSRA & (1<<ADSC)) {} // wait until conversion is over
		return ( ADCL | (ADCH<<8) );
	}
		
  void analogPin::start()
	{
		ADMUX &= ~0xF; // clear channel selection
		ADMUX |= pinnum; // assign channel
		ADCSRA |= (1<<ADATE) | (1<<ADSC); // set auto trigger and start conversion
	}
	
  void analogPin::stop()
	{
		ADCSRA &= ~(1<<ADATE); // clear auto trigger
	}
	
  uint16_t analogPin::check()
	{
		return ( ADCL | (ADCH<<8) );
	}

  void analogPin::AREF(uint8_t m)
    {
		ADCSRA &= ~(1<<ADEN); // disable ADC
		ADMUX &= ~(1<<REFS0); // clear bit if set
		ADMUX |= m << REFS0; // set bit
		ADCSRA |= 1<<ADEN; // enable ADC
	}
	
	
/* PWM basic stuff */
  pwmPin::pwmPin(uint8_t pn)
   {
		if (pn&2) // timer 0
		{
			tccra = &TCCR0A;
			if (pn&1) ocr = &OCR0B; else ocr = &OCR0A;
		}
		
		if (pn&4) // timer 1
		{
			tccra = &TCCR1A;
			if (pn&1) ocr = &OCR1BL; else ocr = &OCR1AL;
		}
		
		if (pn&8) // timer 2
		{
			tccra = &TCCR2A;
			if (pn&1) ocr = &OCR2B; else ocr = &OCR2A;
		}
		pinnum = pn;
		init();
   }   

   void pwmPin::init()
  {
	  switch (pinnum)
	  {
		  case 2: DDRD |= 1<<6; break;
		  case 3: DDRD |= 1<<5; break;
		  case 4: DDRB |= 1<<1; OCR1A = 0; break;
		  case 5: DDRB |= 1<<2; OCR1B = 0; break;
		  case 8: DDRB |= 1<<3; break;
		  case 9: DDRD |= 1<<3; break;
		  
		  default: break;
	  }
	  if (pinnum&8) *(tccra+1) = (1<<CS22); else *(tccra+1) = (1<<CS01) | (1<<CS00); // other stuff for timer 2
	  if (pinnum&4) {*tccra = 1<<WGM10; *(tccra+1) |= 1<<WGM12;}
	  else  *tccra = (1<<WGM01) | (1<<WGM00);
	  if (pinnum&1) *tccra |= (1<<COM0B1); else *tccra |= (1<<COM0A1);
	  
  }
  
  void pwmPin::write(uint8_t val)
  {
	  *ocr = ~val;
  }
	  
  //void enable();
  //void disable();
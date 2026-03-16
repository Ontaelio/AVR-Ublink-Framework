/*
 * Atmega328 Pin functions file
 * Part of Ublink Atmega328 register and peripherals framework
 *
 * Documentation available in the provided MD file.
 *
 * (c) 2021-... Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */
 
#include <avr_gpio.h>

DigitalPin::DigitalPin(volatile uint8_t& prt, uint8_t pn, uint8_t mode){
	mask = 1 << pn;
    portx = (uint8_t*)&prt;
    volatile uint8_t* ddr = portx - 1;
    pinx = ddr - 1;
    *ddr  |= (mode & 1) * mask; 
	*portx |= ((mode >> 1) & 1) * mask; 
	}

void DigitalPin::mode(uint8_t md)
    {
     *portx &= ~(mask);
     *portx |= ((md >> 1) & 1) * mask;
     volatile uint8_t* ddr = pinx + 1;
     *ddr &= ~(mask);
     *ddr |= (md & 1) * mask;
    }
  
void DigitalPin::pinChangeIRQ(uint8_t c) //PCINT
    {
		if (c)
		{
			if (portx == &PORTB) {PCICR |= 1 << PCIE0; PCMSK0 |= mask;}
			else if (portx == &PORTC) {PCICR |= 1 << PCIE1; PCMSK1 |= mask;}
			else if (portx == &PORTD) {PCICR |= 1 << PCIE2; PCMSK2 |= mask;}
		}
		else
		{
			if (portx == &PORTB) PCMSK0 &= ~(mask);
			else if (portx == &PORTC) PCMSK1 &= ~(mask);
			else if (portx == &PORTD) PCMSK2 &= ~(mask);
		}
	}
	  
  void DigitalPin::externalIRQ(uint8_t c) //two pins with INT
	{
		if ((portx == &PORTD) && (mask == (1 << 2))){ //INT0
			EIFR |= (1 << INTF0);
			EICRA = (EICRA & ~0x03) | (c & 3); // clear and set
			EIMSK = (EIMSK & ~1) | (c >> 2); // enable / disable
		}
			
		if ((portx == &PORTD) && (mask == (1 << 3))){ //INT1
			EIFR |= (1 << INTF1);
			EICRA = (EICRA & ~0x0C) | (c << 2); // clear and set
			EIMSK = (EIMSK & ~2) | (c & 4) << 1; // enable / disable
		}
	}


/* basic analog pins */
  AnalogPin::AnalogPin(uint8_t pn)
	{
		pinnum = pn;
		//init(pn); //seems not to work here, even in main()
	}		
		
  void AnalogPin::init(uint8_t ps)
	{
		ADCSRA &= ~0x7; // clear prescaler
		ADCSRA |= ps; // set prescaler
		ADMUX &= ~0xF; // clear channel selection
		ADMUX |= (1<<REFS0) | pinnum; // assign channel
		DIDR0 |= 1 << pinnum; // disable digital input
		ADCSRA |= 1<<ADEN; // enable ADC
		//read();
	}
	
  uint16_t AnalogPin::read()
	{
		ADMUX &= ~0xF; // clear channel selection
		ADMUX |= pinnum; // assign channel
		ADCSRA |= 1<<ADSC; // start conversion
		while (ADCSRA & (1<<ADSC)) {} // wait until conversion is over
		return ( ADCL | (ADCH<<8) );
	}
		
  void AnalogPin::start()
	{
		ADMUX &= ~0xF; // clear channel selection
		ADMUX |= pinnum; // assign channel
		ADCSRA |= (1<<ADATE) | (1<<ADSC); // set auto trigger and start conversion
	}
	
  void AnalogPin::stop()
	{
		ADCSRA &= ~(1<<ADATE); // clear auto trigger
	}
  
  void AnalogPin::digitalEnable()
    {
        DIDR0 &= ~(1<<pinnum); 
    }
	
  uint16_t AnalogPin::check() // read the current conversion result in free running mode
	{
		return ( ADCL | (ADCH<<8) );
	}

  void AnalogPin::AREF(uint8_t m)
  // by default REFS 0b01 is used (AVcc with an external capacitor on AREF pin)
  // this can be changed here, with m standing for:
  // 0 or ADC_AREF for AREF pin as a ref voltage source with internal Vref turned off
  // 1 or ADC_AVCC for default operation above
  // 2 reserved
  // 3 or ADC_INT1V for Internal 1.1V reference with an external capacitor on AREF
  // (0 is identical to analogReference(EXTERNAL) in Arduino framework)
  // Note: an external capacitor on AREF is recommended, not required. Most boards have it.
    {
		ADCSRA &= ~(1<<ADEN); // disable ADC
		ADMUX &= ~(1<<REFS0); // clear bit if set
		ADMUX |= m << REFS0; // set bit
		ADCSRA |= 1<<ADEN; // enable ADC
	}
	
	
/* PWM basic stuff */
  PwmPin::PwmPin(uint8_t pn)
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

   void PwmPin::init()
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
	  
	  // phaseCorrect
	  *tccra = (*tccra & ~0x03) | (1<<WGM00); 
	  // clear on compare
	  if (pinnum&1) *tccra |= (1<<COM0B1); else *tccra |= (1<<COM0A1); 
	  // prescaler 64 (32 for Timer2)
	  *(tccra+1) = (*(tccra+1) & ~0x07) | (1<<CS01) | (1<<CS00); 
	  
  }
  
  /* PWM basic stuff inverted*/
  PwmPinInv::PwmPinInv(uint8_t pn)
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

   void PwmPinInv::init()
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
	 
	  // fast PWM - bad for pin application due to spikes
	  //if (pinnum&4) {*tccra = (*tccra & ~0x03) | 1<<WGM10; *(tccra+1) = 1<<WGM12;} // fast PWM, special for timer1
	  //	else  *tccra = (*tccra & ~0x03) | (1<<WGM01) | (1<<WGM00);
	  
	  // phaseCorrect
	  *tccra = (*tccra & ~0x03) | (1<<WGM00); 
	  // set on compare
	  if (pinnum&1) *tccra |= (1<<COM0B1) | (1<<COM0B0); else *tccra |= (1<<COM0A1) | (1<<COM0A0); 
	  // prescaler 64 (~490 Hz, maybe 8 is better for phase correct, but this is Arduino default)
	  // 32 for Timer 2 (again, as on Arduino)
	  *(tccra+1) = (*(tccra+1) & ~0x07) | (1<<CS01) | (1<<CS00); 
  }
  




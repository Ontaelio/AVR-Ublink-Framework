/*
 * Atmega328 timers functions source file
 * Part of Atmega328 register and peripherals library
 *
 * Documentation available in the provided MD file.
 *
 * (c) 2021-2022 Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

#include <avr_timers.h>
#include <stdio.h>

timer0::timer0 (uint8_t wgm, uint8_t prsclr = 1, uint8_t depth = 0) 
{
	tccrb = prsclr;
	ocra = depth;
	setMode(wgm);
}

void timer0:: config()
{
	TCCR0A = tccra & 0xF3; // remove pin states
	if (tccra & 0x08) DDRD |= 1<<6;
	if (tccra & 0x04) DDRD |= 1<<5;
	TCCR0B = tccrb;
	TIMSK0 = timsk;
	OCR0A = ocra;
}

void timer0:: enable()
{
	disable();
	config();
	start();
}

void timer0::disable() 
{
	TCCR0B &= ~0x7;
	if (tccra & 0x08) DDRD &= ~(1<<6); // disable outputs if enabled via this object
	if (tccra & 0x04) DDRD &= ~(1<<5);
}
	
void timer0:: setMode(uint8_t wgm)
{
	tccrb &= ~0x08;
	tccrb |= (wgm&4)<<1;
	tccra &= ~0x03;
	tccra |= wgm&3;
}

void timer0::setChannelA(uint8_t com, uint8_t pinmode = 1)
{
	tccra &= ~0xC0;
	tccra |= com<<6;
	if (pinmode) tccra |= 1<<3;
}

void timer0::setChannelB(uint8_t com, uint8_t pinmode = 1)
{
	tccra &= ~0x30;
	tccra |= com<<4;
	if (pinmode) tccra |= 1<<2;
}
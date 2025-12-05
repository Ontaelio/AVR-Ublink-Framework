/*
 * Atmega328 timers functions header file
 * Part of Atmega328 register and peripherals library
 *
 * Documentation available in the provided MD file.
 *
 * (c) 2022 Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

#ifndef ATMEGA328_TIMERS_H_
#define ATMEGA328_TIMERS_H_


#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>

// Compare Output Mode values
#define NORMAL 			0x00
#define CM_TOGGLE		0x01
#define CM_CLEAR		0x02
#define CM_SET			0x03

// Waveform Generation Values
#define PWM_PC			0x01
#define CTC				0x02
#define PWM_FAST		0x03
#define PWM_PC_CTC		0x05
#define PWM_FAST_CTC	0x07

// Prescaler Select Values
#define NO_CLOCK		0x00

// Timers 0 and 1 (deprecated)
#define CLOCK_PS1		0x01
#define CLOCK_PS8		0x02
#define CLOCK_PS64		0x03
#define CLOCK_PS256		0x04
#define CLOCK_PS1024	0x05
#define CLOCK_EXT_FALLING	0x06
#define CLOCK_EXT_RISING	0x07

// Timer 0
#define T0_PS1			0x01
#define T0_PS8			0x02
#define T0_PS64			0x03
#define T0_PS256		0x04
#define T0_PS1024		0x05
#define T0_EXT_FALLING	0x06
#define T0_EXT_RISING	0x07

// Timer 1
#define T1_PS1			0x01
#define T1_PS8			0x02
#define T1_PS64			0x03
#define T1_PS256		0x04
#define T1_PS1024		0x05
#define T1_EXT_FALLING	0x06
#define T1_EXT_RISING	0x07

// Timer 2
#define T2_PS1			0x01
#define T2_PS8			0x02
#define T2_PS32			0x03
#define T2_PS64			0x04
#define T2_PS128		0x05
#define T2_PS256		0x06
#define T2_PS1024		0x07

// interrupts
#define OVF		0x01
#define COMPA	0x02
#define COMPB	0x04

class timer0
{
public:
	uint8_t tccra, tccrb, ocra, timsk;
	timer0 (uint8_t wgm, uint8_t prsclr = 1, uint8_t depth = 0);
	void config(); // write set values into corresponding registers
	//void enableOnly() {TCCR0B = tccrb;}
	void enable(); // disable, config, start
	void disable();
	void stop() {TCCR0B &= ~0x7;}
	void start() {TCCR0B = tccrb;}
	uint8_t read() {return TCNT0;}
	operator uint8_t() {return read();}
	void write(uint8_t val) {TCNT0 = val;}
	timer0& operator= (const uint8_t& val) {write(val); return *this;}
	
	void setMode(uint8_t wgm); //set the timer waveform mode
	void setChannelA(uint8_t com, uint8_t pinmode = 1); //set COMxA value and output mode
	void setChannelB(uint8_t com, uint8_t pinmode = 1); //set COMxB value and output mode
	void setDepth(uint8_t res) {ocra = res;} //set depth/resolution/OCRA value
	void setClock(uint8_t psc) {tccrb &= ~0x7; tccrb |= psc;} //set clock source/prescaler
	void setPrescaler(uint8_t psc) {tccrb &= ~0x7; tccrb |= psc;} // compatibility/legacy
	void setInterrupts(uint8_t t) {timsk&= ~0x7; timsk = t;}
	
	//void setPWMchannel(uint8_t ch_num, uint8_t mode, uint8_t plrty = 0); //plrty 0 is active high
	//void pwmWrite(uint8_t ch_num, uint16_t val) {}
	void writeA(uint8_t val) {OCR0A = val;} // write compare value A
	void writeB(uint8_t val) {OCR0B = val;} // write compare value B
	uint8_t readA() {return OCR0A;} // get the current compare value A
	uint8_t readB() {return OCR0B;} // get the current compare value B
	void channelA(uint8_t com, uint8_t pinmode = 1) 
		{TCCR0A &= ~0xC0; TCCR0A |= com<<6; if (pinmode) DDRD |= 1<<6;} // change the channel output on the fly
	void channelB(uint8_t com, uint8_t pinmode = 1)  
		{TCCR0A &= ~0x30; TCCR0A |= com<<4; if (pinmode) DDRD |= 1<<5;}
		
	//void overflowInterrupt(uint8_t bit) {TIMSK0 &= ~0x01; TIMSK0 |= bit;}
	//void compareAinterrupt(uint8_t bit) {TIMSK0 &= ~0x02; TIMSK0 |= bit<<1;}
	//void compareBinterrupt(uint8_t bit) {TIMSK0 &= ~0x04; TIMSK0 |= bit<<2;}
	
	void OVFenable() {TIMSK0 |= 1;}
	void COMPAenable() {TIMSK0 |= 2;}
	void COMPBenable() {TIMSK0 |= 4;}
	void OVFdisable() {TIMSK0 &= ~1;}
	void COMPAdisable() {TIMSK0 &= ~2;}
	void COMPBdisable() {TIMSK0 &= ~4;}
	

	
	//void pwmEnable(uint8_t ch_num);
	//void pwmDisable(uint8_t ch_num);
};


class timer2
{
public:
	uint8_t tccra, tccrb, ocra, timsk;
	timer2 (uint8_t wgm, uint8_t prsclr = 1, uint8_t depth = 0);
	void config(); // write set values into corresponding registers
	//void enableOnly() {TCCR2B = tccrb;}
	void enable(); // disable, config, start
	void disable();
	void stop() {TCCR2B &= ~0x7;}
	void start() {TCCR2B = tccrb;}
	uint8_t read() {return TCNT2;}
	operator uint8_t() {return read();}
	void write(uint8_t val) {TCNT2 = val;}
	timer2& operator= (const uint8_t& val) {write(val); return *this;}
	
	void setMode(uint8_t wgm); //set the timer waveform mode
	void setChannelA(uint8_t com, uint8_t pinmode = 1); //set COMxA value and output mode
	void setChannelB(uint8_t com, uint8_t pinmode = 1); //set COMxB value and output mode
	void setDepth(uint8_t res) {ocra = res;} //set depth/resolution/OCRA value
	void setClock(uint8_t psc) {tccrb &= ~0x7; tccrb |= psc;} //set clock source/prescaler
	void setPrescaler(uint8_t psc) {tccrb &= ~0x7; tccrb |= psc;} // compatibility/legacy
	void setInterrupts(uint8_t t) {timsk&= ~0x7; timsk = t;}
	
	//void setPWMchannel(uint8_t ch_num, uint8_t mode, uint8_t plrty = 0); //plrty 0 is active high
	//void pwmWrite(uint8_t ch_num, uint16_t val) {}
	void writeA(uint8_t val) {OCR2A = val;} // write compare value A
	void writeB(uint8_t val) {OCR2B = val;} // write compare value B
	uint8_t readA() {return OCR2A;} // get the current compare value A
	uint8_t readB() {return OCR2B;} // get the current compare value B
	void channelA(uint8_t com, uint8_t pinmode = 1) 
		{TCCR2A &= ~0xC0; TCCR2A |= com<<6; if (pinmode) DDRD |= 1<<6;} // change the channel output on the fly
	void channelB(uint8_t com, uint8_t pinmode = 1)  
		{TCCR2A &= ~0x30; TCCR2A |= com<<4; if (pinmode) DDRD |= 1<<5;}
		
	//void overflowInterrupt(uint8_t bit) {TIMSK2 &= ~0x01; TIMSK2 |= bit;}
	//void compareAinterrupt(uint8_t bit) {TIMSK2 &= ~0x02; TIMSK2 |= bit<<1;}
	//void compareBinterrupt(uint8_t bit) {TIMSK2 &= ~0x04; TIMSK2 |= bit<<2;}
	
	void OVFenable() {TIMSK2 |= 1;}
	void COMPAenable() {TIMSK2 |= 2;}
	void COMPBenable() {TIMSK2 |= 4;}
	void OVFdisable() {TIMSK2 &= ~1;}
	void COMPAdisable() {TIMSK2 &= ~2;}
	void COMPBdisable() {TIMSK2 &= ~4;}
	

	
	//void pwmEnable(uint8_t ch_num);
	//void pwmDisable(uint8_t ch_num);
};


#endif
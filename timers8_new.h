#ifndef TIMER8_H
#define TIMER8_H

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>

/* =========================================================
 *  Defines
 * ========================================================= */

// prescalers, right 4 bits for timer 2, left 4 bits for timers 0 and 1
#define NO_CLOCK    0x00
#define DIV1        0x11
#define DIV8        0x22
#define DIV32       0x30 
#define DIV64       0x43
#define DIV128      0x50 
#define DIV256      0x64
#define DIV1024     0x75
#define EXT_FALLING	0x06
#define EXT_RISING	0x07

// interrupts
#define OVF		0x01
#define COMPA	0x02
#define COMPB	0x04

// compare output modes
#define COMP_NONE		0x00
#define COMP_TOGGLE		0x01
#define COMP_CLEAR		0x02
#define COMP_SET		0x03

// general functions (sync)
inline void syncTimers(uint8_t c) {GTCCR = 0x80; GTCCR |= c;} // c is 1 for timers 0 and 1 or 3 for all three
inline void syncStart() {GTCCR &= ~0x80;}


struct Timer8Config {
    uint8_t mode;
    uint8_t compA;
    uint8_t compB;
    uint8_t prescaler;
    uint8_t events;
    uint8_t onCompareA;
    uint8_t onCompareB;
};

/* =========================================================
 *  Timer8Profile
 * ========================================================= */

class Timer8Profile {
public:
    Timer8Profile();
    Timer8Profile& Timer8Profile::normal();
    Timer8Profile& Timer8Profile::ctc(uint8_t t);
    Timer8Profile& Timer8Profile::fastPWM();
    Timer8Profile& Timer8Profile::fastPWM(uint8_t t);
    Timer8Profile& Timer8Profile::phaseCorrectPWM();
    Timer8Profile& Timer8Profile::phaseCorrectPWM(uint8_t t);
    Timer8Profile& Timer8Profile::prescaler(uint8_t div);
    Timer8Profile& Timer8Profile::compA(uint8_t v);
    Timer8Profile& Timer8Profile::compB(uint8_t v);
    Timer8Profile& Timer8Profile::events(uint8_t i);
    Timer8Profile& Timer8Profile::onCompareA(uint8_t o);
    Timer8Profile& Timer8Profile::onCompareB(uint8_t o);

private:
    Timer8Config cfg;    
    friend class timer0;
};

/* =========================================================
 *  timer0 — hardware control
 * ========================================================= */

class timer0 {
public:

    timer0& clear() {
        TCCR0A = 0;
        TCCR0B = 0;
        TCNT0  = 0;
        TIMSK0 = 0;
        OCR0A  = 0;
        OCR0B  = 0;
        return *this;
    }

    timer0& normal() {
        TCCR0A = TCCR0A & ~0x03;
        return *this;
    }

    timer0& ctc(uint8_t top) {
        TCCR0A = (TCCR0A & ~0x03) | 0b010;
        OCR0A = top;
        return *this;
    }

    timer0& fastPWM() {
        TCCR0A = (TCCR0A & ~0x03) | 0b011;
        return *this;
    }
    
    timer0& fastPWM(uint8_t top) {
        TCCR0A = (TCCR0A & ~0x03) | 0b011;
        TCCR0B |= 1 << 3;
        OCR0A = top;
        return *this;
    }

    timer0& phaseCorrectPWM() {
        TCCR0A = (TCCR0A & ~0x03) | 0b001;
        return *this;
    }
    
    timer0& phaseCorrectPWM(uint8_t top) {
        TCCR0A = (TCCR0A & ~0x03) | 0b001;
        TCCR0B |= 1 << 3;
        OCR0A = top;
        return *this;
    }

    timer0& compA(uint8_t v) {
        OCR0A = v;
        return *this;
    }

    timer0& compB(uint8_t v) {
        OCR0B = v;
        return *this;
    }

    timer0& events(uint8_t i = 0) {
        TIMSK0 = i;
        return *this;
    }

    timer0& onCompareA(uint8_t m) {
        TCCR0A = (TCCR0A & ~0xC0) | (m << 6);
        return *this;
    }

    timer0& onCompareB(uint8_t m) {
        TCCR0A = (TCCR0A & ~0x30) | (m << 4);
        return *this;
    }

    timer0& prescaler(uint8_t ps) {
        _clkBits = ps;
        return *this;
    }

    inline void start(uint8_t ps) {TCCR0B = (TCCR0B & ~0x07) | ps;}
    inline void start() {TCCR0B = (TCCR0B & ~0x07) | _clkBits;}
    inline void stop() {TCCR0B &= ~0x07;}
    void disable() {TCCR0B &= ~0x07; TIMSK0 = 0;}

    uint8_t read() {return TCNT0;}
	operator uint8_t() {return read();}
	timer0& write(uint8_t val) {TCNT0 = val; return *this;}
	timer0& operator= (const uint8_t& val) {write(val); return *this;}

    void forceCompareA() {TCCR0B |= 1 << FOC0A;}
    void forceCompareB() {TCCR0B |= 1 << FOC0B;}

    void timer0::config(const Timer8Config& cfg);
    void timer0::profile(const Timer8Profile& p);

    // legacy
    void writeA(uint8_t v) {OCR0A = v; return *this;}
    void writeB(uint8_t v) {OCR0B = v; return *this;}
    void OVFenable() {TIMSK0 |= 1;}
	void COMPAenable() {TIMSK0 |= 2;}
	void COMPBenable() {TIMSK0 |= 4;}
	void OVFdisable() {TIMSK0 &= ~1;}
	void COMPAdisable() {TIMSK0 &= ~2;}
	void COMPBdisable() {TIMSK0 &= ~4;}


private:
    uint8_t _clkBits = 1;
};

#endif // TIMER8_H

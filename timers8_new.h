#ifndef TIMER8_H
#define TIMER8_H

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>

/* =========================================================
 *  Defines
 * ========================================================= */

// prescalers, right 4 bits for timer 2, left 4 bits for timers 0 and 1
#define STOP        0x00
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


/* =========================================================
 *  Timer8Profile
 * ========================================================= 

class Timer8Profile {
public:
    Timer8Profile();

    // setters

    Timer8Profile& Timer8Profile::normal() {
        _mode = 0b000;
        return *this;
    }

    Timer8Profile& Timer8Profile::ctc(uint8_t t) {
        _mode = 0b010;
        _compA  = t;
        return *this;
    }

    Timer8Profile& Timer8Profile::fastPWM(uint8_t t = 0) {
        _mode = 0b011;        
        _compA  = t;
        return *this;
    }

    Timer8Profile& Timer8Profile::phaseCorrectPWM(uint8_t t = 0) {
        _mode = 0b001;
        _compA  = t;
        return *this;
    }

    Timer8Profile& Timer8Profile::prescaler(uint8_t div) {
        _prescaler = div;
        return *this;
    }

    Timer8Profile& Timer8Profile::compA(uint8_t v) {
        _compA = v;
        return *this;
    }

    Timer8Profile& Timer8Profile::compB(uint8_t v) {
        _compB = v;
        return *this;
    }

    Timer8Profile& Timer8Profile::events(uint8_t i) {
        _ints = i;
        return *this;
    }

    Timer8Profile& Timer8Profile::ocMode(uint8_t o) {
        _ocm = o;
        return *this;
    }

    // getters
/*
    Timer8Profile::Mode Timer8Profile::mode() const    { return _mode; }
    uint8_t Timer8Profile::top() const                 { return _compA; }
    uint8_t Timer8Profile::compA() const               { return _compA; }
    uint8_t Timer8Profile::compB() const               { return _compB; }
    uint8_t Timer8Profile::prescaler() const           { return _prescaler; }
    uint8_t Timer8Profile::events() const              { return _ints; }  
    uint8_t Timer8Profile::compAmode() const           { return _ocm >> 6; }
    uint8_t Timer8Profile::compBmode() const           { return (_ocm >> 4) & 0x03; }


private:

    uint8_t _mode;
    uint8_t _compA;
    uint8_t _compB;
    uint8_t _prescaler;
    uint8_t _ints;
    uint8_t _ocm;

    friend class timer0;
 
    //uint8_t tccra, tccrb, ocra, timsk;
};

/* =========================================================
 *  timer0 — hardware control
 * ========================================================= */

class timer0 {
public:

    timer0& normal__() {
        TCCR0A = TCCR0A & ~0x03;
        return *this;
    }

    timer0& ctc(uint8_t top) {
        TCCR0A = (TCCR0A & ~0x03) | 0b010;
        OCR0A = top;
        return *this;
    }

    timer0& fastPWM(uint8_t top = 0) {
        TCCR0A = (TCCR0A & ~0x03) | 0b011;
        TCCR0B = (TCCR0B & ~0x08) | bool(top) << 3;
        OCR0A = top;
        return *this;
    }

    timer0& phaseCorrectPWM(uint8_t top = 0) {
        TCCR0A = (TCCR0A & ~0x03) | 0b001;
        TCCR0B = (TCCR0B & ~0x08) | bool(top) << 3;
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

    timer0& __events(uint8_t i = 0) {
        TIMSK0 = i;
        return *this;
    }

    timer0& outputModeA(uint8_t m) {
        TCCR0A = (TCCR0A & ~0xC0) | (m << 6);
        return *this;
    }

    timer0& outputModeB(uint8_t m) {
        TCCR0A = (TCCR0A & ~0x30) | (m << 4);
        return *this;
    }

    inline void start(uint8_t ps = 1) {
        TCCR0B = (TCCR0B & ~0x07) | ps;
    }

    inline void stop() {
        TCCR0B &= ~0x07;
    }

    void disable() {
        TCCR0B &= ~0x07;
        TIMSK0 = 0;
    }

private:
    uint8_t _clkBits;
};

#endif // TIMER8_H

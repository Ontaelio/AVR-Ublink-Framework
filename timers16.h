#ifndef TIMER16_H
#define TIMER16_H

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <macros.h>

// atomic wrapper for debugging
#define TIMER1_ATOMIC(code) \
  do { uint8_t s=SREG; cli(); code; SREG=s; } while(0)

/* =========================================================
 *  Timer16Config
 * ========================================================= */

struct Timer16Config {
    uint8_t  mode;
    uint16_t compA;
    uint16_t compB;
    uint8_t  prescaler;
    uint8_t  events;
    uint8_t  onCompareA;
    uint8_t  onCompareB;
    uint16_t inputCapture;
    uint8_t  inputCfg;
};

/* =========================================================
 *  Timer16Profile
 * ========================================================= */

class Timer16Profile {
public:
    Timer16Profile();

    Timer16Profile& Timer16Profile::normal();
    Timer16Profile& Timer16Profile::phaseCorrectPWM();
    Timer16Profile& Timer16Profile::phaseCorrectPWM9();
    Timer16Profile& Timer16Profile::phaseCorrectPWM10();
    Timer16Profile& Timer16Profile::ctc(uint16_t t);
    Timer16Profile& Timer16Profile::fastPWM();
    Timer16Profile& Timer16Profile::fastPWM9();
    Timer16Profile& Timer16Profile::fastPWM10();
    Timer16Profile& Timer16Profile::phaseFrequencyPWMicr(uint16_t t);
    Timer16Profile& Timer16Profile::phaseFrequencyPWM(uint16_t t);
    Timer16Profile& Timer16Profile::phaseCorrectPWMicr(uint16_t t);
    Timer16Profile& Timer16Profile::phaseCorrectPWM(uint16_t t);
    Timer16Profile& Timer16Profile::ctcIcr(uint16_t t);
    Timer16Profile& Timer16Profile::fastPWMicr(uint16_t t);    
    Timer16Profile& Timer16Profile::fastPWM(uint16_t t);

    Timer16Profile& Timer16Profile::fastPWM16(); // convenience modes using ICR=0xFFFF as top
    Timer16Profile& Timer16Profile::phaseCorrectPWM16();
    Timer16Profile& Timer16Profile::phaseFrequencyPWM16();

    Timer16Profile& Timer16Profile::prescaler(uint8_t div);
    Timer16Profile& Timer16Profile::compA(uint16_t v);
    Timer16Profile& Timer16Profile::compB(uint16_t v);
    Timer16Profile& Timer16Profile::events(uint8_t i);
    Timer16Profile& Timer16Profile::onCompareA(uint8_t o);
    Timer16Profile& Timer16Profile::onCompareB(uint8_t o);
    Timer16Profile& Timer16Profile::inputEdge(uint8_t ie);
    Timer16Profile& Timer16Profile::noiseCanceler(uint8_t nc);

private:
    Timer16Config cfg;    
    friend class timer1;    
};


/* =========================================================
 *  timer1_8bit — hardware 
 * ========================================================= */

class timer1 {
public:

    // WGM10 - TCCR1A #0 (0x01)
    // WGM11 - TCCR1A #1 (0x02)
    // WGM12 - TCCR1B #3 (0x08)
    // WGM13 - TCCR1B #4 (0x10)

    timer1& clear();

    // modes
    
    timer1& normal() {
        // all reset
        TCCR1A = (TCCR1A & ~0x03);
        TCCR1B = (TCCR1B & ~0x18);
        OCR1A = top;
        return *this;
    }

    timer1& phaseCorrectPWM() {
        // WGM10
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM10;
        TCCR1B = (TCCR1B & ~0x18);
        return *this;
    }

    timer1& phaseCorrectPWM9() {
        // WGM11
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM11;
        TCCR1B = (TCCR1B & ~0x18);
        return *this;
    }

    timer1& phaseCorrectPWM10() {
        // WGM10, WGM11
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM10 | 1 << WGM11;
        TCCR1B = (TCCR1B & ~0x18);
        return *this;
    }
    
    timer1& ctc(uint16_t top) {
        // WGM12
        TCCR1A = (TCCR1A & ~0x03);
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM12;
        OCR1A = top;
        return *this;
    }

    timer1& fastPWM() {
        // WGM10, WGM12
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM10;
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM12;
        return *this;
    }

    timer1& fastPWM9() {
        // WGM11, WGM12
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM11;
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM12;
        return *this;
    }

    timer1& fastPWM10() {
        // WGM10, WGM11, WGM12
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM10 | 1 << WGM11;
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM12;
        return *this;
    }
    
    timer1& phaseFrequencyPWMicr(uint16_t top) {
        // WGM13
        TCCR1A = (TCCR1A & ~0x03);
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM13;
        ICR1 = top;
        return *this;
    }

    timer1& phaseFrequencyPWM(uint16_t top) {
        // WGM10, WGM13, OCR1A is top
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM10;
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM13;
        OCR1A = top;
        return *this;
    }

    timer1& phaseCorrectPWMicr(uint16_t top) {
        // WGM11, WGM13
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM11;
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM13;
        ICR1 = top;
        return *this;
    }

    timer1& phaseCorrectPWM(uint16_t top) {
        // WGM10, WGM11, WGM13; OCR1A is top
        TCCR1A = (TCCR1A & ~0x03) | 1<< WGM10 | 1 << WGM11;
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM13;
        OCR1A = top;
        return *this;
    }

    timer1& ctcIcr(uint16_t top) {
        // WGM12, WGM13
        TCCR1A = (TCCR1A & ~0x03);
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM12| 1 << WGM13;
        ICR1 = top;
        return *this;
    }

    timer1& fastPWMicr(uint16_t top) {
        // WGM11, WGM12, WGM13
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM11;
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM12 | 1 << WGM13;
        ICR1 = top;
        return *this;
    }
    
    timer1& fastPWM(uint16_t top) {
        // WGM10, WGM11, WGM12, WGM13; OCR1A is top
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM10 | 1 << WGM11;
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM12 | 1 << WGM13;
        OCR1A = top;
        return *this;
    }

    // convenience modes for 16-bit PWM using ICR as TOP
    timer1& fastPWM16() {
        // WGM11, WGM12, WGM13
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM11;
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM12 | 1 << WGM13;
        ICR1 = 0xFFFF;
        return *this;
    }

    timer1& phaseCorrectPWM16() {
        // WGM11, WGM13
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM11;
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM13;
        ICR1 = 0xFFFF;
        return *this;
    }

    timer1& phaseFrequencyPWM16() {
        // WGM13
        TCCR1A = (TCCR1A & ~0x03);
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM13;
        ICR1 = 0xFFFF;
        return *this;
    }

    // configuration setters

    timer1& compA(uint16_t v) {
        OCR1A = v;
        return *this;
    }

    timer1& compB(uint16_t v) {
        OCR1B = v;
        return *this;
    }

    timer1& events(uint8_t i = 0) {
        TIMSK1 = i;
        return *this;
    }

    timer1& onCompareA(uint8_t m) {
        TCCR1A = (TCCR1A & ~0xC0) | (m << 6);
        return *this;
    }

    timer1& onCompareB(uint8_t m) {
        TCCR1A = (TCCR1A & ~0x30) | (m << 4);
        return *this;
    }

    timer1& prescaler(uint8_t ps) {
        _clkBits = ps & 0x0F;
        return *this;
    }

    timer1& inputEdge(uint8_t ed) {
        TCCR1B = (TCCR1B & ~0x40) | (ed << ICES1);
        return *this;
    }

    timer1& noiseCanceler(uint8_t nc = 1) {
        TCCR1B = (TCCR1B & ~0x80) | (nc << ICNC1);
        return *this;
    }

    inline void start(uint8_t ps) {TCCR1B = (TCCR1B & ~0x07) | ps & 0x0F;}
    inline void start() {TCCR1B = (TCCR1B & ~0x07) | _clkBits;}
    inline void stop() {TCCR1B &= ~0x07;}
    void disable() {TCCR1B &= ~0x07; TIMSK1 = 0;}

    uint16_t readA() {return OCR1A;} // disable interrupts?
    uint16_t readB() {return OCR1B;}
    uint16_t inputCapture() {return ICR1;}

    uint16_t read() {return TCNT1;} // technically should disable interrupts for this, check 20.6 in datasheet
	operator uint16_t() {return read();}
	timer1& write(uint16_t val) {TCNT1 = val; return *this;}
	timer1& operator= (const uint16_t& val) {write(val); return *this;}

    void forceCompareA() {TCCR1C |= 1 << FOC1A;}
    void forceCompareB() {TCCR1C |= 1 << FOC1B;}

    void timer1::config(const Timer16Config& cfg);
    void timer1::profile(const Timer16Profile& p);

}

    #endif //TIMER16_H
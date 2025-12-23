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
#define SYNC_TWO    0x01
#define SYNC_THREE  0x03
inline void syncTimers(uint8_t c) {GTCCR = 0x80; GTCCR |= c;} // c is 1 for timers 0 and 1 or 3 for all three
inline void syncStart() {GTCCR &= ~0x80;}

/* =========================================================
 *  Timer8Config
 * ========================================================= */

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
    friend class timer2;
    friend class timer1_8bit;
};

/* =========================================================
 *  timer0 — hardware 
 * ========================================================= */

class timer0 {
public:

    timer0& clear();

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
        TCCR0B &= ~0x08;
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
        TCCR0B &= ~0x08;
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
        _clkBits = ps & 0x0F;
        return *this;
    }

    inline void start(uint8_t ps) {TCCR0B = (TCCR0B & ~0x07) | ps & 0x0F;}
    inline void start() {TCCR0B = (TCCR0B & ~0x07) | _clkBits;}
    inline void stop() {TCCR0B &= ~0x07;}
    void disable() {TCCR0B &= ~0x07; TIMSK0 = 0;}

    uint8_t readA() {return OCR0A;}
    uint8_t readB() {return OCR0B;}

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


/* =========================================================
 *  timer2 — hardware 
 * ========================================================= */

class timer2 {
public:

    timer2& clear();

    timer2& normal() {
        TCCR2A = TCCR2A & ~0x03;
        return *this;
    }

    timer2& ctc(uint8_t top) {
        TCCR2A = (TCCR2A & ~0x03) | 0b010;
        OCR2A = top;
        return *this;
    }

    timer2& fastPWM() {
        TCCR2A = (TCCR2A & ~0x03) | 0b011;
        TCCR2B &= ~0x08;
        return *this;
    }
    
    timer2& fastPWM(uint8_t top) {
        TCCR2A = (TCCR2A & ~0x03) | 0b011;
        TCCR2B |= 1 << 3;
        OCR2A = top;
        return *this;
    }

    timer2& phaseCorrectPWM() {
        TCCR2A = (TCCR2A & ~0x03) | 0b001;
        TCCR2B &= ~0x08;
        return *this;
    }
    
    timer2& phaseCorrectPWM(uint8_t top) {
        TCCR2A = (TCCR2A & ~0x03) | 0b001;
        TCCR2B |= 1 << 3;
        OCR2A = top;
        return *this;
    }

    timer2& compA(uint8_t v) {
        OCR2A = v;
        return *this;
    }

    timer2& compB(uint8_t v) {
        OCR2B = v;
        return *this;
    }

    timer2& events(uint8_t i = 0) {
        TIMSK2 = i;
        return *this;
    }

    timer2& onCompareA(uint8_t m) {
        TCCR2A = (TCCR2A & ~0xC0) | (m << 6);
        return *this;
    }

    timer2& onCompareB(uint8_t m) {
        TCCR2A = (TCCR2A & ~0x30) | (m << 4);
        return *this;
    }

    timer2& prescaler(uint8_t ps) {
        _clkBits = ps >> 4;
        return *this;
    }

    inline void start(uint8_t ps) {TCCR2B = (TCCR2B & ~0x07) | ps >> 4;}
    inline void start() {TCCR2B = (TCCR2B & ~0x07) | _clkBits;}
    inline void stop() {TCCR2B &= ~0x07;}
    void disable() {TCCR2B &= ~0x07; TIMSK2 = 0;}

    uint8_t readA() {return OCR2A;}
    uint8_t readB() {return OCR2B;}

    uint8_t read() {return TCNT2;}
	operator uint8_t() {return read();}
	timer2& write(uint8_t val) {TCNT2 = val; return *this;}
	timer2& operator= (const uint8_t& val) {write(val); return *this;}

    void forceCompareA() {TCCR2B |= 1 << FOC2A;}
    void forceCompareB() {TCCR2B |= 1 << FOC2B;}

    void timer2::config(const Timer8Config& cfg);
    void timer2::profile(const Timer8Profile& p);

    // legacy
    void writeA(uint8_t v) {OCR2A = v; return *this;}
    void writeB(uint8_t v) {OCR2B = v; return *this;}
    void OVFenable() {TIMSK2 |= 1;}
	void COMPAenable() {TIMSK2 |= 2;}
	void COMPBenable() {TIMSK2 |= 4;}
	void OVFdisable() {TIMSK2 &= ~1;}
	void COMPAdisable() {TIMSK2 &= ~2;}
	void COMPBdisable() {TIMSK2 &= ~4;}


private:
    uint8_t _clkBits = 1;
};


/* =========================================================
 *  timer1_8bit — hardware 
 * ========================================================= */

class timer1_8bit {
public:

    // WGM10 - TCCR1A #0 (0x01)
    // WGM11 - TCCR1A #1 (0x02)
    // WGM12 - TCCR1B #3 (0x08)
    // WGM13 - TCCR1B #4 (0x10)

    timer1_8bit& clear();

    timer1_8bit& normal() = delete;

    timer1_8bit& ctc(uint8_t top) {
        // WGM12 in B, 10, 11, 13 = 0
        TCCR1A = (TCCR1A & ~0x03);
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM12;
        OCR1A = top;
        return *this;
    }

    timer1_8bit& fastPWM() {
        // WGM10, WGM12
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM10;
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM12;
        return *this;
    }
    
    timer1_8bit& fastPWM(uint8_t top) {
        // WGM10, WGM11, WGM12, WGM13
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM10 | 1 << WGM11;
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM12 | 1 << WGM13;
        OCR1A = top;
        return *this;
    }

    timer1_8bit& phaseCorrectPWM() {
        // WGM10
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM10;
        TCCR1B = (TCCR1B & ~0x18);
        return *this;
    }
    
    timer1_8bit& phaseCorrectPWM(uint8_t top) {
        //WGM10, WGM11, WGM13
        TCCR1A = (TCCR1A & ~0x03) | 1 << WGM10 | 1 << WGM11;
        TCCR1B = (TCCR1B & ~0x18) | 1 << WGM13;
        OCR1A = top;
        return *this;
    }

    timer1_8bit& compA(uint8_t v) {
        OCR1A = v;
        return *this;
    }

    timer1_8bit& compB(uint8_t v) {
        OCR1B = v;
        return *this;
    }

    timer1_8bit& events(uint8_t i = 0) {
        TIMSK1 = i;
        return *this;
    }

    timer1_8bit& onCompareA(uint8_t m) {
        TCCR1A = (TCCR1A & ~0xC0) | (m << 6);
        return *this;
    }

    timer1_8bit& onCompareB(uint8_t m) {
        TCCR1A = (TCCR1A & ~0x30) | (m << 4);
        return *this;
    }

    timer1_8bit& prescaler(uint8_t ps) {
        _clkBits = ps & 0x0F;
        return *this;
    }

    inline void start(uint8_t ps) {TCCR1B = (TCCR1B & ~0x07) | ps & 0x0F;}
    inline void start() {TCCR1B = (TCCR1B & ~0x07) | _clkBits;}
    inline void stop() {TCCR1B &= ~0x07;}
    void disable() {TCCR1B &= ~0x07; TIMSK1 = 0;}

    uint8_t readA() {return OCR1A;}
    uint8_t readB() {return OCR1B;}

    uint8_t read() {return TCNT1;}
	operator uint8_t() {return read();}
	timer1_8bit& write(uint8_t val) {TCNT1 = val; return *this;}
	timer1_8bit& operator= (const uint8_t& val) {write(val); return *this;}

    void forceCompareA() {TCCR1C |= 1 << FOC1A;}
    void forceCompareB() {TCCR1C |= 1 << FOC1B;}

    void timer1_8bit::config(const Timer8Config& cfg);
    void timer1_8bit::profile(const Timer8Profile& p);

    // legacy
    void writeA(uint8_t v) {OCR1A = v; return *this;}
    void writeB(uint8_t v) {OCR1B = v; return *this;}
    void OVFenable() {TIMSK1 |= 1;}
	void COMPAenable() {TIMSK1 |= 2;}
	void COMPBenable() {TIMSK1 |= 4;}
	void OVFdisable() {TIMSK1 &= ~1;}
	void COMPAdisable() {TIMSK1 &= ~2;}
	void COMPBdisable() {TIMSK1 &= ~4;}


private:
    uint8_t _clkBits = 1;
};

#endif // TIMER8_H

#include "timers8.h"
#include <avr/io.h>

/* =========================================================
 *  Timer8Profile
 * ========================================================= */

Timer8Profile::Timer8Profile()
    : cfg{ 
        0x00,  // mode
        0,     // compA
        0,     // compB
        1,     // prescaler
        0,     // events
        0,     // onCompareA
        0      // onCompareB
      }  
{}

Timer8Profile& Timer8Profile::normal() {
    cfg.mode = 0b000;
    return *this;
}

Timer8Profile& Timer8Profile::ctc(uint8_t t) {
    cfg.mode = 0b010;
    cfg.compA  = t;
    return *this;
}

Timer8Profile& Timer8Profile::fastPWM() {
    cfg.mode = 0b011;        
    return *this;
}

Timer8Profile& Timer8Profile::fastPWM(uint8_t t) {
    cfg.mode = 0b111;        
    cfg.compA  = t;
    return *this;
}

Timer8Profile& Timer8Profile::phaseCorrectPWM() {
    cfg.mode = 0b001;
    return *this;
}

Timer8Profile& Timer8Profile::phaseCorrectPWM(uint8_t t) {
    cfg.mode = 0b101;
    cfg.compA  = t;
    return *this;
}

Timer8Profile& Timer8Profile::prescaler(uint8_t div) {
    cfg.prescaler = div;
    return *this;
}

Timer8Profile& Timer8Profile::compA(uint8_t v) {
    cfg.compA = v;
    return *this;
}

Timer8Profile& Timer8Profile::compB(uint8_t v) {
    cfg.compB = v;
    return *this;
}

Timer8Profile& Timer8Profile::events(uint8_t i) {
    cfg.events = i;
    return *this;
}

Timer8Profile& Timer8Profile::onCompareA(uint8_t o) {
    cfg.onCompareA = o;
    return *this;
}

Timer8Profile& Timer8Profile::onCompareB(uint8_t o) {
    cfg.onCompareB = o;
    return *this;
}


/* =========================================================
 *  timer0
 * ========================================================= */

 timer0& timer0::clear() {
        TCCR0A = 0;
        TCCR0B = 0;
        TCNT0  = 0;
        TIMSK0 = 0;
        OCR0A  = 0;
        OCR0B  = 0;
        return *this;
    }

__attribute__((always_inline))
inline void timer0::config(const Timer8Config& cfg) {
    timer0::clear();

    TCCR0A = (cfg.mode & 0x03) | (cfg.onCompareA << 6) | (cfg.onCompareB & 0x08);
    TCCR0B = (cfg.mode & 0x08) << 1;
    OCR0A = cfg.compA;
    OCR0B  = cfg.compB;
    TIMSK0 = cfg.events;

    // these will be used in start()
    _clkBits = cfg.prescaler & 0x0F;
}

inline void timer0::profile(const Timer8Profile& p) {
    timer0::config(p.cfg);
}


/* =========================================================
 *  timer2
 * ========================================================= */

 timer2& timer2::clear() {
        TCCR2A = 0;
        TCCR2B = 0;
        TCNT2  = 0;
        TIMSK2 = 0;
        OCR2A  = 0;
        OCR2B  = 0;
        return *this;
    }

__attribute__((always_inline))
inline void timer2::config(const Timer8Config& cfg) {
    timer2::clear();

    TCCR2A = (cfg.mode & 0x03) | (cfg.onCompareA << 6) | (cfg.onCompareB << 4);
    TCCR2B = (cfg.mode & 0x08) << 1;
    OCR2A = cfg.compA;
    OCR2B  = cfg.compB;
    TIMSK2 = cfg.events;

    // these will be used in start()
    _clkBits = cfg.prescaler >> 4;
}

inline void timer2::profile(const Timer8Profile& p) {
    timer2::config(p.cfg);
}

/* =========================================================
 *  timer1 8 bit
 * ========================================================= */

 timer1_8bit& timer1_8bit::clear() {
        TCCR1A = 0;
        TCCR1B = 0;
        TCNT1  = 0;
        TIMSK1 = 0;
        OCR1A  = 0;
        OCR1B  = 0;
        return *this;
    }

__attribute__((always_inline))
inline void timer1_8bit::config(const Timer8Config& cfg) {
    timer1_8bit::clear();

    switch(cfg.mode) {
        case 0:
            //timer1_8bit::normal();
            abort();
            break;
        case 1: //phase correct
            TCCR1A = 1 << WGM10;
            break;
        case 2: // CTC
            TCCR1B = 1 << WGM12;
            break;
        case 3: // Fast PWM
            TCCR1A = 1 << WGM10;
            TCCR1B = 1 << WGM12;
            break;
        case 5: // phase correct ctc
            TCCR1A = 1 << WGM10 | 1 << WGM11;
            TCCR1B = 1 << WGM13;
        case 7: // fast pwm ctc
            TCCR1A = 1 << WGM10 | 1 << WGM11;
            TCCR1B = 1 << WGM12 | 1 << WGM13;
        default:
            break;
    }

    OCR1A = cfg.compA;
    OCR1B  = cfg.compB;
    TIMSK1 = cfg.events;

    // these will be used in start()
    _clkBits = cfg.prescaler & 0x0F;
}

inline void timer1_8bit::profile(const Timer8Profile& p) {
    timer1_8bit::config(p.cfg);
}

Timer8Config timer0::getConfig() {
    Timer8Config cfg;
    cfg.mode = (TCCR0A & 0x03) | ((TCCR0B & 0x08) >> 1);
    cfg.compA = OCR0A;
    cfg.compB = OCR0B;
    cfg.prescaler = TCCR0B & 0x07;
    cfg.events = TIMSK0;
    cfg.onCompareA = (TCCR0A & 0xC0) >> 6;
    cfg.onCompareB = (TCCR0A & 0x30) >> 4;
    return cfg;
}

Timer8Config timer2::getConfig() {
    Timer8Config cfg;
    cfg.mode = (TCCR2A & 0x03) | ((TCCR2B & 0x08) >> 1);
    cfg.compA = OCR2A;
    cfg.compB = OCR2B;
    cfg.prescaler = TCCR2B & 0x07;
    cfg.events = TIMSK2;
    cfg.onCompareA = (TCCR2A & 0xC0) >> 6;
    cfg.onCompareB = (TCCR2A & 0x30) >> 4;
    return cfg;
}
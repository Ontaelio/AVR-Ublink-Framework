#include "timers8_new.h"
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

//__attribute__((always_inline))

inline void timer0::config(const Timer8Config& cfg) {
    timer0::clear();

    TCCR0A = (cfg.mode & 0x03) | (cfg.onCompareA << 6) | (cfg.onCompareB << 4);
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


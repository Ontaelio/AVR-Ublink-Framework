#include "timers16.h"
#include <avr/io.h>

/* =========================================================
 *  Timer16Profile
 * ========================================================= */

Timer16Profile::Timer16Profile()
    : cfg{ 
        0,     // mode
        0,     // compA
        0,     // compB
        1,     // prescaler
        0,     // events
        0,     // onCompareA
        0,     // onCompareB
        0,     // inputCapt
        0      // inputCfg
      }  
{}

Timer16Profile& Timer16Profile::normal() {
    cfg.mode = 0b0000;
    return *this;
}

Timer16Profile& Timer16Profile::phaseCorrectPWM() {
    cfg.mode = 0b0001;
    return *this;
}

Timer16Profile& Timer16Profile::phaseCorrectPWM9() {
    cfg.mode = 0b0010;
    return *this;
}

Timer16Profile& Timer16Profile::phaseCorrectPWM10() {
    cfg.mode = 0b0011;
    return *this;
}

Timer16Profile& Timer16Profile::ctc(uint16_t t) {
    cfg.mode = 0b0100;
    cfg.compA = t;
    return *this;
}

Timer16Profile& Timer16Profile::fastPWM() {
    cfg.mode = 0b0101;
    return *this;
}

Timer16Profile& Timer16Profile::fastPWM9() {
    cfg.mode = 0b0110;
    return *this;
}

Timer16Profile& Timer16Profile::fastPWM10() {
    cfg.mode = 0b0111;
    return *this;
}

Timer16Profile& Timer16Profile::phaseFrequencyPWMicr(uint16_t t) {
    cfg.mode = 0b1000;
    cfg.inputCapture = t;
    return *this;
}

Timer16Profile& Timer16Profile::phaseFrequencyPWM(uint16_t t) {
    cfg.mode = 0b1001;
    cfg.compA = t;
    return *this;
}

Timer16Profile& Timer16Profile::phaseCorrectPWMicr(uint16_t t) {
    cfg.mode = 0b1010;
    cfg.inputCapture = t;
    return *this;
}

Timer16Profile& Timer16Profile::phaseCorrectPWM(uint16_t t) {
    cfg.mode = 0b1011;
    cfg.compA = t;
    return *this;
}

Timer16Profile& Timer16Profile::ctcIcr(uint16_t t) {
    cfg.mode = 0b1100;
    cfg.inputCapture = t;
    return *this;
}

Timer16Profile& Timer16Profile::fastPWMicr(uint16_t t) {
    cfg.mode = 0b1110;
    cfg.inputCapture = t;
    return *this;
}

Timer16Profile& Timer16Profile::fastPWM(uint16_t t) {
    cfg.mode = 0b1111;
    cfg.compA = t;
    return *this;
}

Timer16Profile& Timer16Profile::fastPWM16() {
    cfg.mode = 0b1110;
    cfg.inputCapture = 0xFFFF;
    return *this;
}

Timer16Profile& Timer16Profile::phaseCorrectPWM16() {
    cfg.mode = 0b1010;
    cfg.inputCapture = 0xFFFF;
    return *this;
}

Timer16Profile& Timer16Profile::phaseFrequencyPWM16() {
    cfg.mode = 0b1000;
    cfg.inputCapture = 0xFFFF;
    return *this;
}

Timer16Profile& Timer16Profile::prescaler(uint8_t div) {
    cfg.prescaler = div;
    return *this;
}

Timer16Profile& Timer16Profile::compA(uint16_t v) {
    cfg.compA = v;
    return *this;
}

Timer16Profile& Timer16Profile::compB(uint16_t v) {
    cfg.compB = v;
    return *this;
}

Timer16Profile& Timer16Profile::events(uint8_t i) {
    cfg.events = i;
    return *this;
}

Timer16Profile& Timer16Profile::onCompareA(uint8_t o) {
    cfg.onCompareA = o;
    return *this;
}

Timer16Profile& Timer16Profile::onCompareB(uint8_t o) {
    cfg.onCompareB = o;
    return *this;
}

Timer16Profile& Timer16Profile::inputEdge(uint8_t ie) {
    cfg.inputCfg = (cfg.inputCfg & ~0x40) | (ie << ICES1);
    return *this;
}

Timer16Profile& Timer16Profile::noiseCanceler(uint8_t nc) {
    cfg.inputCfg = (cfg.inputCfg & ~0x80) | (nc << ICNC1);
    return *this;
}


/* =========================================================
 *  timer1 Hardware
 * ========================================================= */

 timer1& timer1::clear() {
        TCCR1A = 0;
        TCCR1B = 0;
        TCNT1  = 0;
        TIMSK1 = 0;
        OCR1A  = 0;
        OCR1B  = 0;
        ICR1   = 0;
        return *this;
    }

// WGM10 - TCCR1A #0 (0x01)
// WGM11 - TCCR1A #1 (0x02)
// WGM12 - TCCR1B #3 (0x08)
// WGM13 - TCCR1B #4 (0x10)

timer1& timer1::config(const Timer16Config& cfg) {
    timer1::clear();

    TCCR1A = (cfg.mode & 0x03) | (cfg.onCompareA << 6) | (cfg.onCompareB << 4);
    TCCR1B = (cfg.mode & 0x18) << (1 | cfg.inputCfg);
    OCR1A = cfg.compA;
    OCR1B  = cfg.compB;
    ICR1 = cfg.inputCapture;
    TIMSK1 = cfg.events;

    // these will be used in start()
    _clkBits = cfg.prescaler & 0x0F;
    return *this;
}

timer1& timer1::profile(const Timer16Profile& p) {
    return timer1::config(p.cfg);
}
#include "timers8_new.h"
#include <avr/io.h>

/* =========================================================
 *  Timer8Profile
 * ========================================================= 

Timer8Profile::Timer8Profile()
    : _mode(0x0000),
      _compA(0),
      _compB(0),
      _prescaler(STOP)
{}
*/


/* =========================================================
 *  timer0
 * ========================================================= */

//timer0::timer0()
//    : _cfg(), _clkBits(0)
//{}

//timer0::timer0() {
//    _cfg = Timer8Profile();
//    _clkBits = 0;
//}

//timer0& timer0::apply() {
//    disable();
//    applyProfile(_cfg);
//    return *this;
//    }

/*
timer0& timer0::apply(const Timer8Profile& p) {
    disable();
    applyProfile(p);
    //_cfg = p;
    return *this;
}

__attribute__((always_inline))
inline void timer0::applyProfile(const Timer8Profile& p) {
    TCCR0A = 0;
    TCCR0B = 0;
    TCNT0  = 0;
    TIMSK0 &= ~0x7;

    uint8_t k = uint8_t(p._mode);
    TCCR0A |= k & 0x03;
    TCCR0B |= (k & 0x08) << 1;
    //if (k & 0x04) {
    //    TCCR0B |= (1 << WGM02);
    //    OCR0A = p._compA;
    // }
    OCR0A = p._compA;

    OCR0B  = p._compB;
    TIMSK0 |= p._ints;

    // these will be used in start()
    _clkBits = p._prescaler & 0x0F;
}



*/
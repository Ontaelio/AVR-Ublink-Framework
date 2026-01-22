#ifndef TIMER_PROFILES_H_
#define TIMER_PROFILES_H_

#include <timers8.h>
#include <timers16.h>
#include <macros.h>

namespace Timer8 {

inline Timer8Profile basicPWM() {
    Timer8Profile p;
    p.fastPWM()
     .prescaler(DIV64)
     .onCompareA(COMP_CLEAR)
     .onCompareB(COMP_CLEAR);
    return p;
    }

inline Timer8Profile invertedPWM() {
    Timer8Profile p;
    p.fastPWM()
     .prescaler(DIV64)
     .onCompareA(COMP_SET)
     .onCompareB(COMP_SET);
    return p;
    }
}


namespace Timer16 {

inline Timer16Profile basicPWM() {
    Timer16Profile p;
    p.fastPWM()
     .prescaler(DIV64)
     .onCompareA(COMP_CLEAR)
     .onCompareB(COMP_CLEAR);
    return p;
    }

inline Timer16Profile invertedPWM() {
    Timer16Profile p;
    p.fastPWM()
     .prescaler(DIV64)
     .onCompareA(COMP_SET)
     .onCompareB(COMP_SET);
    return p;
    }

inline Timer16Profile ultraSound() {
    Timer16Profile p;
    p.fastPWM(15000) // TOP in COMPA, mode 15
    .compB(3) // PWM for Trig
    .prescaler(DIV64)
    .inputEdge(INPUT_FALLING)
    .onCompareB(COMP_CLEAR)
    .noiseCanceler(1); // on
    return p;
    }
}


#endif // TIMER_PROFILES_H_


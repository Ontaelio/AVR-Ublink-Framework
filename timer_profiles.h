#ifndef TIMER_PROFILES_H_
#define TIMER_PROFILES_H_

#include <timers8_new.h>
#include <timers16.h>
#include <macros.h>

namespace Timer8Profiles {

inline Timer8Profile basicPWM() {
    Timer8Profile p;
    p.fastPWM()
     .prescaler(DIV64)
     .onCompareA(COMP_CLEAR)
     .onCompareB(COMP_CLEAR);
    return p;
    }
}


namespace Timer16Profiles {

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
}


//extern Timer8Profile basicPWM;
//extern Timer16Profile basicPWM_;


#endif // TIMER_PROFILES_H_


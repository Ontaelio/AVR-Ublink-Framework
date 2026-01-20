#ifndef UBLINK_UTILS_H_
#define UBLINK_UTILS_H_

#include <stdint.h>
#include <avr/io.h>


// proxy structs for overloading operators inside classes
// default 8 bit, 16 bit for Timer1
// require register accessors

template<class R>
struct RegProxy {
  inline void operator=(uint8_t v) const {
    R::ref() = v;
  }

  inline operator uint8_t() const {
    return R::ref();
  }
};


template<class R>
struct RegProxy16 {
  inline void operator=(uint16_t v) const {
    R::ref() = v;
  }

  inline operator uint16_t() const {
    return R::ref();
  }
};



#endif // UBLINK_UTILS_H_
#ifndef UBLINK_PROXIES_H_
#define UBLINK_PROXIES_H_

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
struct RegProxyR {
  inline operator uint8_t() const {
    return R::ref();
  }
};

template<class R>
struct RegProxyW {
  inline void operator=(uint8_t v) const {
    R::ref() = v;
  }
};

template<class R>
struct RegProxyFull {
  inline void operator= (uint8_t v) const {
    R::ref() = v;
  }

  inline operator uint8_t() const {
    return R::ref();
  }

  inline void operator++ () {
    R::ref() = R::ref() + 1;
  }

  uint8_t operator++ (int) {
    uint8_t old = R::ref();
    R::ref() = old + 1;
    return old;
  }

  inline void operator-- () {
    R::ref() = R::ref() - 1;
  }

  uint8_t operator-- (int) {
    uint8_t old = R::ref();
    R::ref() = old - 1;
    return old;
  }

  inline void operator+= (uint8_t v) const {
    R::ref() = R::ref() + v;
  }

  inline void operator-= (uint8_t v) const {
    R::ref() = R::ref() - v;
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

template<class R>
struct RegProxy16R {
  inline operator uint16_t() const {
    return R::ref();
  }
};

template<class R>
struct RegProxy16W {
  inline void operator=(uint16_t v) const {
    R::ref() = v;
  }
};

template<class R>
struct RegProxy16Full {
  inline void operator= (uint16_t v) const {
    R::ref() = v;
  }

  inline operator uint16_t() const {
    return R::ref();
  }

  inline void operator++ () {
    R::ref() = R::ref() + 1;
  }

  uint16_t operator++ (int) {
    uint16_t old = R::ref();
    R::ref() = old + 1;
    return old;
  }

  inline void operator-- () {
    R::ref() = R::ref() - 1;
  }

  uint16_t operator-- (int) {
    uint16_t old = R::ref();
    R::ref() = old - 1;
    return old;
  }

  inline void operator+= (uint16_t v) const {
    R::ref() = R::ref() + v;
  }

  inline void operator-= (uint16_t v) const {
    R::ref() = R::ref() - v;
  }
};


#endif // UBLINK_PROXIES_H_
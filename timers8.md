# AVR Timer0 Minimal Library

## Overview

This library provides a **minimal, low-overhead C++ interface** for controlling the **8-bit Timer/Counter0** on AVR microcontrollers (e.g. ATmega328).

The design goals are:

* direct and predictable access to hardware registers
* no hidden state, no background processing
* no callbacks, no virtual functions, no dynamic memory
* minimal Flash and RAM usage
* compatibility with pure `avr-gcc` workflows (no Arduino runtime required)

Interrupt service routines (ISRs) are **not abstracted** and must be implemented by the user.

---

## Philosophy

This library is **not a HAL** and **not an Arduino-style wrapper**.

* All configuration methods write **directly to hardware registers**
* The timer has no internal shadow state
* Method chaining is used only for convenience, not for delayed application
* Interrupt vectors belong to the user code, not the library

The library assumes that the user understands:

* basic AVR timer operation
* datasheet terminology (WGM, OCR, COM, prescaler, etc.)

---

## Supported Hardware

* ATmega328 / ATmega328P
* Other AVR MCUs with compatible Timer0 register layout

---

## Basic Usage

### Example: Fast PWM with overflow interrupt

```cpp
#include <avr/interrupt.h>
#include "timer0.h"

timer0 t0;

ISR(TIMER0_OVF_vect) {
    // user code
}

void setup() {
    t0.clear()
      .fastPWM()
      .events(TIMER0_OVF)
      .start(DIV64);

    sei();
}

int main() {
    setup();
    while (1) {
        // main loop
    }
}
```

---

## Class `timer0`

### `clear()`

```cpp
timer0& clear();
```

Resets all Timer0-related registers:

* `TCCR0A`
* `TCCR0B`
* `TCNT0`
* `TIMSK0`
* `OCR0A`
* `OCR0B`

Does **not** enable the timer.

---

### Waveform Generation Modes

#### `normal()`

```cpp
timer0& normal();
```

Sets **Normal mode** (WGM = 0).
Timer counts from 0 to 255 and overflows.

---

#### `ctc(uint8_t top)`

```cpp
timer0& ctc(uint8_t top);
```

Sets **CTC mode** (Clear Timer on Compare Match).

* `OCR0A` is used as TOP
* Timer resets when `TCNT0 == OCR0A`

---

#### `fastPWM(uint8_t top = 0)`

```cpp
timer0& fastPWM(uint8_t top = 0);
```

Sets **Fast PWM mode**.

* If `top == 0`: fixed TOP = 255
* If `top > 0`: `OCR0A` is used as TOP (WGM02 enabled)

---

#### `phaseCorrectPWM(uint8_t top = 0)`

```cpp
timer0& phaseCorrectPWM(uint8_t top = 0);
```

Sets **Phase Correct PWM mode**.

* Symmetric up/down counting
* Same TOP behavior as `fastPWM()`

---

## Compare Registers

#### `compA(uint8_t value)`

```cpp
timer0& compA(uint8_t value);
```

Writes to `OCR0A`.

---

#### `compB(uint8_t value)`

```cpp
timer0& compB(uint8_t value);
```

Writes to `OCR0B`.

---

## Output Compare Pin Behavior

#### `onCompareA(uint8_t mode)`

```cpp
timer0& onCompareA(uint8_t mode);
```

Sets COM0A bits.

#### `onCompareB(uint8_t mode)`

```cpp
timer0& onCompareB(uint8_t mode);
```

Sets COM0B bits.

Typical values:

| Mode | Effect                  |
| ---- | ----------------------- |
| 0    | Normal port operation   |
| 1    | Toggle on compare match |
| 2    | Clear on compare match  |
| 3    | Set on compare match    |

---

## Interrupts

### `events(uint8_t mask)`

```cpp
timer0& events(uint8_t mask);
```

Writes directly to `TIMSK0`.

### Interrupt Masks

```cpp
#define TIMER0_OVF   (1 << TOIE0)
#define TIMER0_COMPA (1 << OCIE0A)
#define TIMER0_COMPB (1 << OCIE0B)
```

Example:

```cpp
t0.events(TIMER0_OVF | TIMER0_COMPB);
```

### ISR Implementation

Interrupt handlers **must be implemented by the user**:

```cpp
ISR(TIMER0_OVF_vect) { }
ISR(TIMER0_COMPA_vect) { }
ISR(TIMER0_COMPB_vect) { }
```

The library does not provide callbacks or dispatching.

---

## Timer Control

### `start(uint8_t prescaler)`

```cpp
void start(uint8_t prescaler);
```

Starts the timer by setting clock source bits (`CS02..CS00`).

---

### `stop()`

```cpp
void stop();
```

Stops the timer clock.

---

### `disable()`

```cpp
void disable();
```

Stops the timer and disables all Timer0 interrupts.

---

## Prescaler Values

Prescaler values are passed directly:

```cpp
#define STOP    0
#define DIV1    1
#define DIV8    2
#define DIV64   3
#define DIV256  4
#define DIV1024 5
```

Example:

```cpp
t0.start(DIV256);
```

---

## Design Notes

* No dynamic memory
* No virtual methods
* No callbacks
* No Arduino core dependency
* No hidden timer state
* All operations are deterministic and synchronous

---

## Non-Goals

This library intentionally does **not** provide:

* automatic pin configuration
* ISR abstraction or routing
* Arduino `millis()` compatibility
* safety checks for invalid configurations

---

## License

[add license here]


Creating more than one instance of the same timer results in undefined behavior.
Normal mode is intentionally unavailable in 8-bit compatibility mode.
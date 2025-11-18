# Ublink AVR: gpio
*Digital, PWM and analog pins*

## Digital pins

Class **digitalPin**(uint8_t* prt, uint8_t pn, uint8_t mode)\
This class is used to create an object representing a single digital pin. Parameters are:\
int8_t* **prt**: a _pointer_ to the port name. Use & with the name, e.g. `&PORTD`;\
uint8_t **pn**: pin number;\
uint8_t **mode**: pin mode - OUTPUT, INPUT, INPUT_PULLUP.

Here's a cheatsheet of Arduino pin names and their port-pin actual representation:

|   Arduino pins   |  Port  |  Pins    |
|------|-----|----|
|0..7|PORTD|0..7|
|8..13|PORTB|0..5|
|A0..A5|PORTC|0..5|

Thus, to set the Arduino pin 13 (built-in LED) to output use:\
`digitalPin led(&PORTB, 5, OUTPUT)`.

-----

`digitalPin`'s methods:

void **high()** and **set()**: set pin HIGH (logic 1).

void **low()** and **reset()**: set pin LOW (logic 0).

void **invert()**: invert pin output (0 to 1, 1 to 0).

void **mode**(uint8_t md): set pin to mode `md`.

uint8_t **read()**: read pin value. Can be substituted with an overloaded operator `=` (`uint8_t a = pin`).

void **write**(uint8_t val): write 0 or 1 to pin configured as OUTPUT. Can be substituted with an overloaded operator `=` (`pin = 1`).

void **pinChangeIRQ**(uint8_t c = 1): turn correcponding pin change interrupt (PCINT) on (c == 1) or off (c == 0).

void **externalIRQ**(uint8_t c): turn corresponding INT interrupt on or off. INT0 is connected to pin 2 (PORTD 2) and INT1 to pin 3 (PORTD 3).

## Analog pins

This library provides basic ADC functionality that should be enough in most cases. This includes both analogRead-style functions as well as free-running mode support. While analogRead-style allows multiple sensors on ADC pins it takes some time to get the readings and the result is not exact. Free-running mode allows only one sensor but provides better results instantly. One can switch between different inputs in free-running mode, bu only one of them will be working at any given time.

Class **analogPin**(uint8_t pn)
Creates an object representing a single ADC pin. This object can only be used to access ADC, not digital functions. `pn` is pin number without an 'A' (0-5).

void **init**(uint8_t ps = ADCPS128): must be used at least once in setup to start the ADC. `ps` (prescaler) is optional and is set to 128 by default.

uint16_t **read()**: same as `analogRead()`, i.e. the slow way to read a single pin; useful when there are multiple sensors connected. Starts the conversion and waits for it to finish to return the result (blocking).

void **start()**: starts the free-running mode (continuous conversion).

void **stop()**: stops the free-running mode.

uint16_t **check()**: returns the current value in free-running mode. Overloaded operator `=` also returns this (as in `uint16_t a = adc_pin`).

void **AREF**(uint8_t m = 0): can be used to change the reference voltage:

| setting | meaning
|---|---|
|0 or `ADC_AREF`|AREF pin as a ref voltage source with internal Vref turned off|
|1 or `ADC_AVCC`|AVcc with an external capacitor on AREF pin (default mode)|
|3 or `ADC_INT1V`|Internal 1.1V reference with an external capacitor on AREF

Note: an external capacitor on AREF is recommended, not required. Most boards have it.


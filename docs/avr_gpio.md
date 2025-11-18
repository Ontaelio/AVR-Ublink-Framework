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


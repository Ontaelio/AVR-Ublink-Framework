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

## Digital pins standalone functions

void **pinChangeBenable()**\
void **pinChangeCenable()**\
void **pinChangeDenable()**\
void **pinChangeBdisable()**\
void **pinChangeCdisable()**\
void **pinChangeDdisable()**\
Enable/disable pin change interrupt on ports B, C, D.

void **pullupDisable()**\
void **pullupEnable()**\
Disable/enable pull-ups on all pins.

## Analog pins

This library provides basic ADC functionality that should be enough in most cases. This includes both analogRead-style functions as well as free-running mode support. While analogRead-style allows multiple sensors on ADC pins it takes some time to get the readings and the result is not exact. Free-running mode allows only one sensor but provides better results instantly. One can switch between different inputs in free-running mode, bu only one of them will be working at any given time.

Class **analogPin**(uint8_t pn, uint8_t ps = ADCPS128)\
Creates an object representing a single ADC pin. This object can only be used to access ADC, not digital functions. `pn` is pin number without an 'A' (0-5). `ps` is useful only if the constructor is invoked inside main(), as it runs init() there too.

void **init**(uint8_t ps = ADCPS128): must be used at least once in setup to start the ADC _unless_ you create objects inside main() avoiding Arduino shenanigans. `ps` (prescaler) is optional and is set to 128 by default.

Available prescaler constants are `ADCPS128`, `ADCPS64`, `ADCPS32`, `ADCPS16`, `ADCPS8`, `ADCPS4` and `ADCPS2`

uint16_t **read()**: same as `analogRead()`, i.e. the slow way to read a single pin; useful when there are multiple sensors connected. Starts the conversion and waits for it to finish to return the result (blocking).

void **start()**: starts the free-running mode (continuous conversion).

void **stop()**: stops the free-running mode.

uint16_t **check()**: returns the current reading in free-running mode. Overloaded operator `=` also returns this (as in `uint16_t a = adc_pin`).

void **digitalEnable()**: enables sigital input buffer on the pin (disabled in `init()`). For the rare cases when same pin is used for analog and digital input simultaneously. 

void **AREF**(uint8_t m = 0): can be used to change the reference voltage for the entire ADC(!):

| setting | meaning
|---|---|
|0 or `ADC_AREF`|AREF pin as a ref voltage source with internal Vref turned off|
|1 or `ADC_AVCC`|AVcc with an external capacitor on AREF pin (default mode)|
|3 or `ADC_INT1V`|Internal 1.1V reference with an external capacitor on AREF

Note: an external capacitor on AREF is recommended, not required. Most boards have it.

## PWM pins

This library provides very basic PWM functionality, mostly corresponding to the `analogWrite()` except all pins work in fast PWM mode (8 bit, /64 prescaler). For better PWM control dedicated timer libraries should be used.

Class **pwmPin**(uint8_t pn)\
Creates a pwm-outputting pin `pn`. `pn` is a 4-bit value where three left bits represent a timer (2|1|0)and the lsb represents the channel. Any of these constants can be used here:
|Arduino pin | The real pin|Value|_Timer, channel_|
|----|------|---|---|
|PIN6 | PIND6 | 2|_timer0, A_
|PIN5 | PIND5 | 3| _timer0, B_
|PIN9 | PINB1 | 4|_timer1, A_
|PIN10 | PINB2 | 5|_timer1, B_
|PIN11 | PINB3 | 8|_timer2, A_
|PIN3 | PIND3 | 9|_timer2, B_

void **init()**: not needed, included just in case, you may use it in `setup()` if something goes wrong.

void **write**(uint8_t val): write a pwm value to the pwm pin.

Overloaded operators can also be used on pwmPin objects:\
`=` to set or read pwm value;\
`++` and `--`, both prefix and postfix;\
`+=` and `-=`.

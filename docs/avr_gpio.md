# Ublink AVR: gpio
*Digital, PWM and analog pins*

## Digital pins

Class **digitalPin**(volatile uint8_t& prt, uint8_t pn, uint8_t mode)\
This class is used to create an object representing a single digital pin. Parameters are:\
int8_t& **prt**: port name, e.g. `PORTD`;\
uint8_t **pn**: pin number;\
uint8_t **mode**: pin mode - OUTPUT, INPUT, INPUT_PULLUP.

Here's a cheatsheet of Arduino pin names and their port-pin actual representation:

|   Arduino pins   |  Port  |  Pins    |
|------|-----|----|
|0..7|PORTD|0..7|
|8..13|PORTB|0..5|
|A0..A5|PORTC|0..5|

Thus, to set the Arduino pin 13 (built-in LED) to output use:\
`digitalPin led(PORTB, 5, OUTPUT)`.

-----

`digitalPin`'s methods:

void **high()** and **set()**: set pin HIGH (logic 1).

void **low()** and **reset()**: set pin LOW (logic 0).

void **invert()**: invert pin output (0 to 1, 1 to 0).

void **mode**(uint8_t md): set pin to mode `md`.

uint8_t **read()**: read pin value. Can be substituted with an overloaded operator `=` (`uint8_t a = pin`).

void **write**(uint8_t val): write 0 or 1 to pin configured as OUTPUT. Can be substituted with an overloaded operator `=` (`pin = 1`).

void **pinChangeIRQ**(uint8_t c = 1): turn correcponding pin change interrupt (PCINT) on (c == 1) or off (c == 0).

void **externalIRQ**(uint8_t c): clear interrupt flag, setup and turn corresponding INT interrupt on or off. INT0 is connected to pin 2 (PORTD 2) and INT1 to pin 3 (PORTD 3); this method works with these pins' instances only. `c` is three-bit and includes both the INT setup and its on/off bit; set it to **0** to turn the corresponding interrupt off. Use these macros for `c` (note that these values and names differ from the standalone INT functions macros):

| macro | value | meaning |
|-------|-|---|
| PIN_OFF |0b000  | Turn this INT off. |
| PIN_LOW | 0b100 | Low level generates an interrupt request. |
| PIN_CHANGE | 0b101 | Any logical change generates an interrupt request. |
| PIN_FALLING | 0b110 | The falling edge generates an interrupt request. |
| PIN_RISING | 0b111 | The rising edge generates an interrupt request. |

*Note: `externalIRQ` method duplicates the standalone functions described below. Choose one approach and stick to it to avoid confusion*


## Digital pins standalone functions

void **pinChangeBenable()**\
void **pinChangeCenable()**\
void **pinChangeDenable()**\
void **pinChangeBdisable()**\
void **pinChangeCdisable()**\
void **pinChangeDdisable()**\
Enable/disable pin change interrupt on ports B, C, D.

void **pinChangeBmask**(uint8_t mask)\
void **pinChangeCmask**(uint8_t mask)\
void **pinChangeDmask**(uint8_t mask)\
Each bit of the `mask` selects whether pin change interrupt is enabled on the corresponding pin:

|bits:|7|6|5|4|3|2|1|0|
|----|-|-|-|-|-|-|-|-|
|B|PCINT7|PCINT6|PCINT5|PCINT4|PCINT3|PCINT2|PCINT1|PCINT0|
|C|-|PCINT14|PCINT13|PCINT12|PCINT11|PCINT10|PCINT9|PCINT8|
|D|PCINT23|PCINT22|PCINT21|PCINT20|PCINT19|PCINT18|PCINT17|PCINT16|

void **pinChangeBclear()**\
void **pinChangeCclear()**\
void **pinChangeDclear()**\
Clears the corresponding Pin Change interrupt flag.

void **int0setup**(uint8_t val)\
void **int1setup**(uint8_t val)\
Sets level and edges for External Interrupts INT0 (pin D2) and INT1 (pin D3). The following macros/values should be used here:

| macro | value | meaning |
|-------|-|---|
| INT_LOW | 0b00 | Low level generates an interrupt request. |
| INT_CHANGE | 0b01 | Any logical change generates an interrupt request. |
| INT_FALLING | 0b10 | The falling edge generates an interrupt request. |
| INT_RISING | 0b11 | The rising edge generates an interrupt request. |

void **int0enable**()\
void **int1enable**()\
void **int0disable**()\
void **int1disable**()\
Enable/disable INT0 and INT1 external interrupts.

void **int0clear()**\
void **int1clear()**\
Clears the corresponding INT interrupt flag.

*Note: `int` functions duplicate the `externalIRQ` method above, but have lesser footprint.*

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

This library provides very basic PWM functionality, mostly corresponding to the `analogWrite()` except all pins work in Phase Correct PWM mode (8 bit, /64 prescaler for timers 0 and 1, /32 for timer 2). For better PWM control dedicated timer libraries should be used.

Class **pwmPin**(uint8_t pn)\
Creates a pwm-outputting pin `pn`. `pn` is a 4-bit value where three left bits represent a timer (2|1|0)and the lsb represents the channel. Any of these macros can be used here:
|Arduino pin | The real pin|Value|_Timer, channel_|
|----|------|---|---|
|PWM_6 | PWM_D6 | 2|_timer0, A_
|PWM_5 | PWM_D5 | 3| _timer0, B_
|PWM_9 | PWM_B1 | 4|_timer1, A_
|PWM_10 | PWM_B2 | 5|_timer1, B_
|PWM_11 | PWM_B3 | 8|_timer2, A_
|PWM_3 | PWM_D3 | 9|_timer2, B_

_There's an additional **pwmPinInv** class that uses inverted PWM mode (set on compare). Use it if you ground stuff on the Atmega (connect cathode to the pin in case of LEDs). Note that this approach is right and should be used always._

void **init()**: not _exactly_ needed, you may use it in `setup()` if something goes wrong or if you didn't disable Arduino's `init()`.

void **write**(uint8_t val): write a pwm value to the pwm pin.

Overloaded operators can also be used on pwmPin objects:\
`=` to set or read pwm value;\
`++` and `--`, both prefix and postfix (postfix returns `uint8_t`);\
`+=` and `-=`.

_Note: avoid using postfixes unless specifically required (e.g. `for` loops), as postfix has higher overhead (it has to save and return previous value)._

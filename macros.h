#ifndef UBLINK_MACROS_H_
#define UBLINK_MACROS_H_

/* =========================================================
 *  GPIO
 * ========================================================= */

#ifndef INPUT
#define INPUT 0x0
#endif

#ifndef OUTPUT
#define OUTPUT 0x1
#endif

#ifndef INPUT_PULLUP
#define INPUT_PULLUP 0x2
#endif

// macros for externalIRQ() digitalPin method, include on/off bit
#define PIN_LOW		0x4
#define PIN_CHANGE	0x5
#define PIN_FALLING	0x6
#define PIN_RISING	0x7

// ADC (gpio)

#define ADCPS128	0x7
#define ADCPS64		0x6
#define ADCPS32		0x5
#define ADCPS16		0x4
#define ADCPS8		0x3
#define ADCPS4		0x2
#define ADCPS2		0x1

#define ADC_AREF	0x0
#define ADC_AVCC	0x1
#define ADC_INT1V	0x3

// PWM (gpio)

#define PIN6	2
#define PIND6	2
#define PIN5	3
#define PIND5	3

#define PIN9	4
#define PINB1	4
#define PIN10	5
#define PINB2	5

#define PIN11	8
#define PINB3	8
#define PIN3	9
#define PIND3	9

// GPIO interrupts, standalone setup, enable, disable functions

#define INT_LOW     0
#define INT_CHANGE  1
#define INT_FALLING 2
#define INT_RISING  3


/* =========================================================
 *  Timer8
 * ========================================================= */

// prescalers, right 4 bits for timer 2, left 4 bits for timers 0 and 1
#define NO_CLOCK    0x00
#define DIV1        0x11
#define DIV8        0x22
#define DIV32       0x30 
#define DIV64       0x43
#define DIV128      0x50 
#define DIV256      0x64
#define DIV1024     0x75
#define EXT_FALLING	0x06
#define EXT_RISING	0x07

// interrupts
#define OVF		0x01
#define COMPA	0x02
#define COMPB	0x04

// compare output modes
#define COMP_NONE		0x00
#define COMP_TOGGLE		0x01
#define COMP_CLEAR		0x02
#define COMP_SET		0x03

// general functions (sync)
#define SYNC_TWO    0x01
#define SYNC_THREE  0x03

#endif // UBLINK_MACROS_H_
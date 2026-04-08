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

#ifndef HIGH
#define HIGH 0x01
#endif

#ifndef LOW
#define LOW 0x00
#endif

#ifndef A0
#define A0 14
#endif

#ifndef A1
#define A1 15
#endif

#ifndef A2
#define A2 16
#endif

#ifndef A3
#define A3 17
#endif

#ifndef A4
#define A4 18
#endif

#ifndef A5
#define A5 19
#endif

#ifndef A6
#define A6 20
#endif

#ifndef A7
#define A7 21
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

#define PWM_6	2
#define PWM_D6	2
#define PWM_5	3
#define PWM_D5	3

#define PWM_9	4
#define PWM_B1	4
#define PWM_10	5
#define PWM_B2	5

#define PWM_11	8
#define PWM_B3	8
#define PWM_3	9
#define PWM_D3	9

// GPIO interrupts, standalone setup, enable, disable functions

#define INT_LOW     0
#define INT_CHANGE  1
#define INT_FALLING 2
#define INT_RISING  3


/* =========================================================
 *  Timers
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
#define INCAPT  0x20

// compare output modes
#define COMP_NONE		0x00
#define COMP_TOGGLE		0x01
#define COMP_CLEAR		0x02
#define COMP_SET		0x03

// input capture
#define INPUT_FALLING   0x00
#define INPUT_RISING    0x01

// general functions (sync)
#define SYNC_TWO    0x01
#define SYNC_THREE  0x03

/* =========================================================
 *  SPI
 * ========================================================= */

#define LEADING 0x00
#define TRAILING 0x01

#define SPI_DIV4 0x00
#define SPI_DIV16 0x01
#define SPI_DIV64 0x02
#define SPI_DIV128 0x03
#define SPI_IDLE_HIGH 0x08
#define SPI_PHASE_TRAILING 0x04
#define SPI_LSB_FIRST 0x20 

/* =========================================================
 *  TWI (I2C)
 * ========================================================= */

// private
#define TW_WRITE 0
#define TW_READ  1

// public: prescalers
#define I2C_PSC1  0x00
#define I2C_PSC4  0x01
#define I2C_PSC16 0x02
#define I2C_PSC64 0x03

// public: big- and little-endian
#define I2C_MSB_FIRST 0x00
#define I2C_LSB_FIRST 0x01

// public: Error/state codes
// Generic results
#define I2C_OK                     0x00

// Internal / software errors (never overlap with TWSR)
#define I2C_START_TIMEOUT          0x01  // ping - START timeout
#define I2C_SLA_TIMEOUT            0x02  // ping - SLA timeout
#define I2C_BUS_ERROR              0x03  // bus error, including illegal start/stop error

// Master mode - transmitter
#define I2C_START_TRANSMITTED      0x08
#define I2C_REP_START_TRANSMITTED  0x10

#define I2C_MT_SLA_ACK             0x18  // SLA+W transmitted, ACK received
#define I2C_MT_SLA_NACK            0x20  // SLA+W transmitted, NACK received

#define I2C_MT_DATA_ACK            0x28  // data transmitted, ACK received
#define I2C_MT_DATA_NACK           0x30  // data transmitted, NACK received

#define I2C_MT_ARB_LOST            0x38  // arbitration lost

// Master mode - receiver
#define I2C_MR_SLA_ACK             0x40  // SLA+R transmitted, ACK received
#define I2C_MR_SLA_NACK            0x48  // SLA+R transmitted, NACK received

#define I2C_MR_DATA_ACK            0x50  // data received, ACK returned
#define I2C_MR_DATA_NACK           0x58  // data received, NACK returned

// general errors
#define I2C_NO_INFO                0xF8
#define I2C_ILLEGAL_SS             0x00  // illegal START/STOP

#endif // UBLINK_MACROS_H_
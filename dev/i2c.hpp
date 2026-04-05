/*
 * Atmega328 8-bit TWI (I2C) library
 * Part of Ublink Atmega328 register and peripherals framework
 *
 * Documentation available in the provided MD file.
 *
 * (c) 2026 Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

#ifndef UBLINK_AVRTWI_H
#define UBLINK_AVRTWI_H

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr_gpio.h>
#include <macros.h>
#include <avr/delay.h>




uint8_t i2c_start(){
    // 1. Start | Enable | Clear TWINT
    TWCR = (1<<TWSTA) | (1<<TWEN) | (1<<TWINT);

    // 2. Wait for flag
    while (!(TWCR & (1<<TWINT)));

    // 3. Status 0x08 means START has been transmitted
    //    0x10 means repeated START has been transmitted
    uint8_t status = TWSR & 0xF8;
    if (status == 0x08) {return 0;}
    return 1;
}

uint8_t i2c_addr2write(uint8_t slave_addr){
    // 1. 7-bit address | READ/WRITE bit (read = 1)
    TWDR = (slave_addr << 1) | 0; //TW_WRITE == 0, TW_READ == 1

    // 2. Enable | Clear TWINT; enable not necessary, but doesn't hurt
    TWCR = (1<<TWEN) | (1<<TWINT);

    // 3. Wait for flag
    while (!(TWCR & (1<<TWINT)));

    // 4. ACK check. Errors: 0x20 SLA+W was sent, NOT ACK received. 0x48: SLA+R sent, NOT ACK received
    //               Errors: 0x40 Arbitration lost in SLA+R or NOT ACK
    uint8_t status = TWSR & 0xF8;
    if ((TWDR & 1) == 0) { // just for now, checks whether we wanted to read or write
        if (status == 0x18) { return 0; /* SLA+W ACK ok*/ }
    } else {
        if (status == 0x40) { return 0; /* SLA+R ACK ok*/ }
    }
    return status;
}

uint8_t i2c_writeByte(uint8_t dataByte){
    TWDR = dataByte;
    TWCR = (1<<TWEN) | (1<<TWINT); // TWEN not needed but doesn't hurt
    while (!(TWCR & (1<<TWINT)));
    uint8_t status = TWSR & 0xF8;
    if (status == 0x28) { return 0; /* Data ACK */ }
    return status; // error happened
}

uint8_t i2c_repeatedStart(uint8_t slave_addr){
    // 1. REPEATED START, same as start
    TWCR = (1<<TWSTA) | (1<<TWEN) | (1<<TWINT);
    while (!(TWCR & (1<<TWINT)));
    uint8_t status = TWSR & 0xF8;
    if (status != 0x10) return status; /* Repeated START failed somehow*/

    TWDR = (slave_addr << 1) | 1; // TW_READ
    TWCR = (1<<TWEN) | (1<<TWINT);
    while (!(TWCR & (1<<TWINT)));
    status = TWSR & 0xF8;
    if (status != 0x40) return status; /* SLA+R ACK failed*/
    return 0;
}

uint8_t i2c_readStream(uint8_t* data, uint8_t len) {
    if (len == 0) return 0;

    for (uint8_t i = 0; i < len; i++) {
        if (i < (len - 1)) {
            // All except the last with ACK
            TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWEA);
        } else {
            // Last byte with NACK
            TWCR = (1<<TWEN) | (1<<TWINT);
        }

        while (!(TWCR & (1<<TWINT)));

        uint8_t status = TWSR & 0xF8;
        // 0x50 = Data received, ACK returned
        // 0x58 = Data received, NACK returned
        if (!((status == 0x50) || (status == 0x58))) return status;

        data[i] = TWDR; 
    }

    return 0; 
}

void i2c_stop(){
    TWCR = (1<<TWSTO) | (1<<TWEN) | (1<<TWINT);
}

// slave_addr already set to SLA+W, START sent
uint8_t i2c_writeStream(uint8_t* data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        TWDR = data[i];                 // 1. load a byte
        TWCR = (1<<TWEN) | (1<<TWINT); // 2. clear flag
        while (!(TWCR & (1<<TWINT)));  // 3. wait for flag

        uint8_t status = TWSR & 0xF8;  // 4. check ACK
        if (status != 0x28) return status; // 0x28 = Data ACK ok, else return error
    }
    return 0; // all done
}

uint8_t i2c_resetBus(){
    // disable TWI
    TWCR &= ~(1 << TWEN);

    // SDA input
    DDRC &= ~(1 << PC4);

    // SCL released (input)
    DDRC &= ~(1 << PC5);

    // if SDA is HIGH, everything's fine
    if (PINC & (1 << PC4)) return 0;

    for (uint8_t i = 0; i < 9; i++)
    {
        // SCL LOW
        DDRC |= (1 << PC5);
        PORTC &= ~(1 << PC5);
        _delay_us(5);

        // SCL HIGH (release)
        DDRC &= ~(1 << PC5);
        _delay_us(5);

        // check SDA
        if (PINC & (1 << PC4))
        {
            // START: SDA LOW while SCL HIGH
            DDRC |= (1 << PC4);
            PORTC &= ~(1 << PC4);
            _delay_us(5);

            // STOP: SDA HIGH while SCL HIGH
            DDRC &= ~(1 << PC4);
            _delay_us(5);

            return 0;
        }
    }

    return 1;
}

uint8_t i2c_ping(uint8_t slave_addr) {
    volatile uint16_t timeout = 0xFFFF;

    // 1. START
    TWCR = (1<<TWSTA) | (1<<TWEN) | (1<<TWINT);
    while (!(TWCR & (1<<TWINT))) {
        if (--timeout == 0) return 1; // START timeout
    }

    uint8_t status = TWSR & 0xF8;
    if (status != 0x08) return status; // START failed

    // 2. SLA+W
    TWDR = (slave_addr << 1) | TW_WRITE;
    TWCR = (1<<TWEN) | (1<<TWINT);
    timeout = 0xFFFF;
    while (!(TWCR & (1<<TWINT))) {
        if (--timeout == 0) return 2; // SLA timeout
    }

    status = TWSR & 0xF8;
    if (status == 0x18) {
        // SLA+W ACK ok
        TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWSTO); // generate STOP
        return 0;
    }
    else if (status == 0x20) {
        // SLA+W NACK
        TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWSTO); // generate STOP
        return 3;
    }
    else if (status == 0x38) {
        // Arbitration lost
        TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWSTO); // STOP
        return 4;
    }

    // Any other error
    TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWSTO);
    return status;
}

#endif // UBLINK_AVRTWI
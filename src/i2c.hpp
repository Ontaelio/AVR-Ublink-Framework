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
#include <util/delay.h>

/* ------------------------
 *   Standalone functions
 * -----------------------*/

//setup

inline void i2c_setFreq(uint32_t scl_freq){
    uint32_t twbr = ((F_CPU / scl_freq) - 16) / 2; // calculate TWBR
    TWBR = (uint8_t)twbr;
}

inline void i2c_init(uint32_t scl_freq = 100000) {    
    TWSR &= ~((1<<TWPS0) | (1<<TWPS1)); // prescaler = 1
    i2c_setFreq( scl_freq);    
    TWCR = (1<<TWEN); // enable TWI
}

inline void i2c_setPrescaler(uint8_t ps) {TWSR = (TWSR & ~0x03) | (ps & 0x03);}
inline void i2c_IRQenable() {TWCR |= (1 << TWIE);}
inline void i2c_IRQdisable() {TWCR &= ~(1 << TWIE);}

//troubleshooting

uint8_t i2c_resetBus();
uint8_t i2c_ping(uint8_t addr);
uint8_t i2c_scanAddr(uint8_t startAddr = 0x07);

// START and STOP

inline uint8_t i2c_start(){    
    TWCR = (1<<TWSTA) | (1<<TWEN) | (1<<TWINT); // Start | Enable | Clear TWINT
    while (!(TWCR & (1<<TWINT))); // Wait for flag
    uint8_t status = TWSR & 0xF8;
    if (status == I2C_START_TRANSMITTED) return I2C_OK;
    if (status == 0) return I2C_BUS_ERROR;
    return status;
}

inline uint8_t i2c_repeatedStart(){
    TWCR = (1<<TWSTA) | (1<<TWEN) | (1<<TWINT); // same as start
    while (!(TWCR & (1<<TWINT)));
    uint8_t status = TWSR & 0xF8;
    if (status != I2C_REP_START_TRANSMITTED) return status; /* Repeated START failed somehow*/
    if (status == 0) return I2C_BUS_ERROR;
    return status;
}

inline void i2c_stop(){
    TWCR = (1<<TWSTO) | (1<<TWEN) | (1<<TWINT);
}

// communication

uint8_t i2c_addrWrite(uint8_t slave_addr);
uint8_t i2c_addrWrite10(uint16_t addr);
uint8_t i2c_addrRead(uint8_t slave_addr);
uint8_t i2c_addrRead10(uint16_t addr);
uint8_t i2c_writeByte(uint8_t dataByte);
uint8_t i2c_writeLast(uint8_t dataByte); // compatibility
uint8_t i2c_writeStream(uint8_t* data, uint16_t len);
uint8_t i2c_readByte(uint8_t& dataByte); // returns ok/error, not byte; sends ACK
uint8_t i2c_readLast(uint8_t& dataByte); // sends NACK
uint8_t i2c_readStream(uint8_t* data, uint16_t len);


/* ------------------------
 *   Class
 * -----------------------*/

class Twi{
private:
    uint8_t device_addr, word_addr;
    inline void startAndSendRegisterAddress(uint16_t regaddr){
        i2c_start();
        i2c_addrWrite(device_addr);
        if (word_addr) i2c_writeByte(regaddr>>8);
        i2c_writeByte(regaddr&0xFF);
    }

public:
    Twi(uint8_t addr, uint8_t reglen = 8) : device_addr(addr), word_addr(reglen > 8) {}

    void init(uint32_t scl_freq = 100000) {i2c_init(scl_freq);}
    uint8_t ping() {return i2c_ping(device_addr);}

    Twi& startWrite() {i2c_start(); i2c_addrWrite(device_addr); return *this;}
    Twi& startRead() {i2c_start(); i2c_addrRead(device_addr); return *this;}
    Twi& write(uint8_t tx) {i2c_writeByte(tx); return *this;}
    Twi& writeStream(uint8_t* data, uint16_t len) {i2c_writeStream(data, len); return *this;}
    Twi& read(uint8_t& rx) {i2c_readByte(rx); return *this;}
    Twi& readLast(uint8_t& rx) {i2c_readLast(rx); return *this;}
    Twi& readStream(uint8_t* arr, uint16_t len) {i2c_readStream(arr, len); return *this;}
    inline void stop() {i2c_stop();}

    inline void write(uint16_t addr, uint8_t stuff) {
        startAndSendRegisterAddress(addr);
        i2c_writeByte(stuff);
        i2c_stop();
    }

	void write16(uint16_t addr, uint16_t stuff, uint8_t littleEndian = I2C_MSB_FIRST);
	void write32(uint16_t addr, uint32_t stuff, uint8_t littleEndian = I2C_MSB_FIRST);

	inline void writeStream(uint16_t addr, uint8_t* stuff, uint16_t num){
        startAndSendRegisterAddress(addr);
        i2c_writeStream(stuff, num);
        i2c_stop();
    }

	inline uint8_t read(uint16_t addr){
        uint8_t res;
        startAndSendRegisterAddress(addr);
        i2c_repeatedStart();
        i2c_addrRead(device_addr);
        i2c_readLast(res);
        i2c_stop();
        return res;
    }

	uint16_t read16(uint16_t addr, uint8_t littleEndian = I2C_MSB_FIRST);
	uint32_t read32(uint16_t addr, uint8_t littleEndian = I2C_MSB_FIRST);
    
	inline void readStream(uint16_t addr, uint8_t* arr, uint16_t num){
        startAndSendRegisterAddress(addr);
        i2c_repeatedStart();
        i2c_addrRead(device_addr);
        i2c_readStream(arr, num);
        i2c_stop();
    }
};

// aliaces for compatibility
using i2c  	= Twi;
using i2cSlave = Twi;

#endif // UBLINK_AVRTWI
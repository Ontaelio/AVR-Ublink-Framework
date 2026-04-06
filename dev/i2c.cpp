/*
 * Atmega328 8-bit TWI (I2C) library cpp file
 * Part of Ublink Atmega328 register and peripherals framework
 *
 * Documentation available in the provided MD file.
 *
 * (c) 2026 Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

 #include <..\dev\i2c.hpp>

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

    return 3;
}



uint8_t i2c_ping(uint8_t slave_addr) {
    volatile uint16_t timeout = 0xFFFF;

    // 1. START
    TWCR = (1<<TWSTA) | (1<<TWEN) | (1<<TWINT);
    while (!(TWCR & (1<<TWINT))) {
        if (--timeout == 0) return 1; // START timeout
    }

    uint8_t status = TWSR & 0xF8;
    if (status != I2C_START_TRANSMITTED && status != I2C_REP_START_TRANSMITTED) return status; // START failed

    // 2. SLA+W
    TWDR = (slave_addr << 1) | TW_WRITE;
    TWCR = (1<<TWEN) | (1<<TWINT);
    timeout = 0xFFFF;
    while (!(TWCR & (1<<TWINT))) {
        if (--timeout == 0) return 2; // SLA timeout
    }

    status = TWSR & 0xF8;
    if (status == I2C_MT_SLA_ACK) {
        // SLA+W ACK ok
        TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWSTO); // generate STOP
        return 0;
    }
    else if (status == I2C_MT_SLA_NACK) {
        // SLA+W NACK
        TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWSTO); // generate STOP
        return status;
    }
    else if (status == I2C_MT_ARB_LOST) {
        // Arbitration lost
        TWCR = (1<<TWEN) | (1<<TWINT); 
        return status;
    }

    // Any other error
    TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWSTO);
    return status;
}

uint8_t i2c_scanAddr(uint8_t startAddr){
    uint8_t a;
	while (startAddr < 0x77)	{
		startAddr++;
        //return i2c_ping(startAddr);
		//if (i2c_ping(startAddr) == 0) return 254; //startAddr;
        a = i2c_ping(startAddr);
        if (a == 0) return startAddr;
	}
	return 0;
}


uint8_t i2c_addrWrite(uint8_t slave_addr){
    // 1. 7-bit address | READ/WRITE bit (read = 1)
    TWDR = (slave_addr << 1) | TW_WRITE; //TW_WRITE == 0, TW_READ == 1

    // 2. Enable | Clear TWINT; enable not necessary, but doesn't hurt
    TWCR = (1<<TWEN) | (1<<TWINT);

    // 3. Wait for flag
    while (!(TWCR & (1<<TWINT)));

    // 4. ACK check. Errors: 0x20 SLA+W was sent, NOT ACK received. 0x48: SLA+R sent, NOT ACK received
    //               Errors: 0x40 Arbitration lost in SLA+R or NOT ACK
    uint8_t status = TWSR & 0xF8;
    if (status == I2C_MT_SLA_ACK) return I2C_OK;
    return status;
}

uint8_t i2c_addrWrite10(uint16_t addr){
    uint8_t status;

    // 1. First byte: 11110 A9 A8 + W
    TWDR = 0xF0 | ((addr >> 7) & 0x06); 
    TWCR = (1<<TWEN) | (1<<TWINT);
    while (!(TWCR & (1<<TWINT)));
    status = TWSR & 0xF8;
    if (status != I2C_MT_SLA_ACK) return status;

    // 2. Second byte: A7..A0
    TWDR = (uint8_t)(addr & 0xFF);
    TWCR = (1<<TWEN) | (1<<TWINT);
    while (!(TWCR & (1<<TWINT)));
    status = TWSR & 0xF8;
    if (status != I2C_MT_DATA_ACK) return status;

    return I2C_OK;
}

uint8_t i2c_addrRead(uint8_t slave_addr){
    TWDR = (slave_addr << 1) | TW_READ; //TW_WRITE == 0, TW_READ == 1
    TWCR = (1<<TWEN) | (1<<TWINT);
    while (!(TWCR & (1<<TWINT)));
    uint8_t status = TWSR & 0xF8;
    if (status == I2C_MR_SLA_ACK) return I2C_OK; /* SLA+R ACK ok*/ 
    return status;
}

uint8_t i2c_addrRead10(uint16_t addr){
    uint8_t status;

    // 1. First byte: 11110 A9 A8 + W
    TWDR = 0xF0 | ((addr >> 7) & 0x06); // 11110 A9 A8 0
    TWCR = (1<<TWEN) | (1<<TWINT);
    while (!(TWCR & (1<<TWINT)));
    status = TWSR & 0xF8;
    if (status != I2C_MT_SLA_ACK) return status;

    // 2. Second byte: A7..A0
    TWDR = (uint8_t)(addr & 0xFF);
    TWCR = (1<<TWEN) | (1<<TWINT);
    while (!(TWCR & (1<<TWINT)));
    status = TWSR & 0xF8;
    if (status != I2C_MT_DATA_ACK) return status;

    // 3. Repeated START
    TWCR = (1<<TWSTA) | (1<<TWEN) | (1<<TWINT);
    while (!(TWCR & (1<<TWINT)));
    status = TWSR & 0xF8;
    if (status != I2C_REP_START_TRANSMITTED) return status;

    // 4. First byte again, but READ
    TWDR = 0xF1 | ((addr >> 7) & 0x06); // 11110 A9 A8 1
    TWCR = (1<<TWEN) | (1<<TWINT);
    while (!(TWCR & (1<<TWINT)));
    status = TWSR & 0xF8;
    if (status == I2C_MR_SLA_ACK) return I2C_OK;

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

uint8_t i2c_writeLast(uint8_t dataByte) {return i2c_writeByte(dataByte);} 

uint8_t i2c_readLast(uint8_t& dataByte){
    TWCR = (1<<TWEN) | (1<<TWINT); // NACK
    while (!(TWCR & (1<<TWINT)));
    uint8_t status = TWSR & 0xF8;
    if (status != I2C_MR_DATA_NACK) return status; //
    dataByte = TWDR; 
    return I2C_OK;
}

uint8_t i2c_readByte(uint8_t& dataByte){
    TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWEA); // ACK
    while (!(TWCR & (1<<TWINT)));
    uint8_t status = TWSR & 0xF8;
    if (status != I2C_MR_DATA_ACK) return status; //
    dataByte = TWDR; 
    return I2C_OK;
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
        if (!((status == I2C_MR_DATA_ACK) || (status == I2C_MR_DATA_NACK))) return status;

        data[i] = TWDR; 
    }

    return 0; 
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


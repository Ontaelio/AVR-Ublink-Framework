/*
 * Spi memory module W25 library for Atmega328. 
 * Requires the Ublink framework, in particular SPI and GPIO libraries.
 *
 * Documentation available in the provided MD file.
 *
 * (c) 2026 Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

#ifndef SPIMEMW25_H_
#define SPIMEMW25_H_

#include <spi.hpp>
#include <avr_gpio.h>
#include <util/delay.h>

//digitalPin cs(PORTD, 7, OUTPUT);
//SpiSlave device(cs);

template<typename Bus = SpiSlave>
class SpiMemory{
private:
    Bus& device;

public:
    SpiMemory(Bus& dev) : device(dev) {}

    void enable(){
        device.speed2x().enable();
    }

    void disable(){
        device.disable();
    }

    uint8_t busy(){
        uint8_t sr;
        device.begin().write(0x05);
        sr = device.read();
        device.end();
        return (sr & 0x01);
    }

    SpiMemory& wait(){
        while (busy()) {}
        return *this;
    }

    SpiMemory& writeEnable(){
	    device.single(0x06);
        return *this;
    }

    SpiMemory& write(uint32_t address, const uint8_t* arr_out, uint16_t num = 256){
        writeEnable();
        device.begin()
            .write(0x02)
            .write(address >> 16)
            .write(address >> 8)
            .write(address)
            .write(arr_out, num)
            .end();
        return *this;
    }

    SpiMemory& read(uint32_t address, uint8_t* arr_in, uint16_t num){
        device.begin()
            .write(0x03)
            .write(address>>16)
            .write(address>>8)
            .write(address)
            .read(arr_in, num)
            .end();
        return *this;
    }

    SpiMemory& fastRead(uint32_t address, uint8_t* arr_in, uint16_t num){
        device.begin()
            .write(0x0B)
            .write(address>>16)
            .write(address>>8)
            .write(address)
            .write(0)
            .read(arr_in, num)
            .end();
        return *this;
    }

    SpiMemory& beginStream(uint32_t address){
        device.begin()
            .write(0x03)
            .write(address>>16)
            .write(address>>8)
            .write(address);
        return *this;
    }

    SpiMemory& beginFastStream(uint32_t address){
        device.begin()
            .write(0x0B)
            .write(address>>16)
            .write(address>>8)
            .write(address)
            .write(0);
        return *this;
    }

    SpiMemory& read(uint8_t* arr_in, uint16_t num){
        device.read(arr_in, num);
        return *this;
    }

    SpiMemory& endStream(){
        device.end();
        return *this;
    }

    SpiMemory& erase(uint32_t address){
        if (!(address & 0x000FFF)) writeEnable();
        device.begin()
            .write(0x20)
            .write(address>>16)
            .write((address&0x00F000)>>8)
            .write(0)
            .end();
        return *this;	
    }

    SpiMemory& block32erase(uint32_t address){
        if (!(address & 0x007FFF)) writeEnable();
        device.begin()
            .write(0x20)
            .write(address>>16)
            .write((address&0x008000)>>8)
            .write(0)
            .end();	
        return *this;
    }

    SpiMemory& block64erase(uint32_t address){
        if (!(address & 0x00FFFF)) writeEnable();
        device.begin()
            .write(0x20)
            .write(address>>16)
            .write(0)
            .write(0)
            .end();	
        return *this;
    }

    SpiMemory& chipErase(){
        device.single(0xC7);
        return *this;
    }

    uint16_t verify8(uint32_t addr, const uint8_t* ref, uint16_t len){
        uint8_t buf[8];   
        uint16_t diff = 0;
        if (!len) len = 256;
        beginStream(addr);
        while (len) {
            uint8_t n = (len > 8) ? 8 : len;
            read(buf, n);
            for (uint8_t i = 0; i < n; i++) {
                if (buf[i] != ref[i])
                    diff++;
            }
            ref  += n;
            len  -= n;
        }
        endStream();
        return diff;
    }

    uint16_t verify(uint32_t addr, const uint8_t* ref, uint16_t len = 256){
        uint16_t diff = 0;
        beginStream(addr);
        while(len--) 
            diff += (device.read() != *ref++);
        endStream();
        return diff;
    }

    uint16_t status(){
        device.begin().write(0x05);
        uint8_t lsb = device.read();
        device.end();
        device.begin().write(0x35);
        uint8_t msb = device.read();
        device.end();
        return ((msb << 8) | lsb);
    }

    uint8_t manufacturerID(){
        uint8_t res;
        device.begin().write(0x9F);
        res = device.read();
        device.end();
        return res;
    }

    uint16_t deviceID(){
        uint16_t res;
        device.begin()
            .write(0x90)
            .write(0)
            .write(0)
            .write(0)
            .write(0);
        res = device.transfer(0);
        res = res | (device.transfer(0) << 8);
        device.end();
        return res;
    }

    uint64_t uniqueID(){
        uint64_t res;
        device.begin()
            .write(0x4B)
            .write(0)
            .write(0)
            .write(0)
            .write(0);
        res = (uint64_t)(device.read()) << 56;
        res |= (uint64_t)(device.read()) << 48;
        res |= (uint64_t)(device.read()) << 40;
        res |= (uint64_t)(device.read()) << 32;
        res |= (uint64_t)(device.read()) << 24;
        res |= (uint64_t)(device.read()) << 16;
        res |= (uint64_t)(device.read()) << 8;
        res |= (uint64_t)(device.read());
        device.end();
        return res;
    }

    uint32_t JEDEC(){
        uint32_t res = 0;
        device.begin().write(0x9F);
        res = ((uint32_t)device.read()) << 16;
        res |= ((uint32_t)device.read()) << 8;
        res |= (uint32_t)device.read();
        device.end();
        return res;
    }
};

using SpiMem      = SpiMemory<SpiSlave>;
using UsartSpiMem = SpiMemory<UsartSpiSlave>;
using SoftSpiMem  = SpiMemory<SoftSpiSlave>;

#endif // SPIMEMW25_H_
/*
 * Atmega328 USART library header file
 * Part of Ublink Atmega328 register and peripherals framework
 *
 * Documentation available in the provided MD file.
 *
 * (c) 2021-2022 Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

#ifndef ATMEGA328_USART_H_
#define ATMEGA328_USART_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif // F_CPU

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>

#define USE_BUFFER_FOR_INTS // asm stacking doesn't work on Atmega328, must use buffer
//#define NAN_INFINITY_ENABLED

#define USART_TX	(1<<TXCIE0)
#define USART_RX	(1<<RXCIE0)
#define USART_EMPTY (1<<UDRIE0)

class usart
{
public:
	//usart() {}
	usart (uint32_t baud = 0);
	void init(uint32_t baud);
	void initTX(uint32_t baud);
	void initRX(uint32_t baud);
	
	void sendByte(uint8_t dat);
	uint8_t getByte();
	void dump();

	void sendStream(uint8_t* dat, uint32_t size);

	void printString(const char *str);
	//void printCPPString(std::string s);
	void printUInt(uint32_t a);
	void printInt(int32_t a);

	void printUInt64(uint64_t a);
	void printInt64(int64_t a);
	void fprintUInt(uint32_t a, uint_fast8_t s);

	void printFloat(long double a, uint_fast8_t s = 2);
	void printHex(unsigned long val);
	void printBin(unsigned long val);

	char getChar();
	void getStream(uint8_t* dat, uint16_t size);
	uint64_t getUInt();
	int64_t getInt();
	long double getFloat();
	void getString(char* arr);

	void RXenable() {UCSR0B |= (1<<RXEN0);}
	void RXdisable() {UCSR0B &= ~(1<<RXEN0);}
	void TXenable() {UCSR0B |= (1<<TXEN0);}
	void TXdisable() {UCSR0B &= ~(1<<TXEN0);}
	
	void IRQenable(uint8_t irqs) {UCSR0B |= irqs;}
	void IRQdisable(uint8_t irqs) {UCSR0B &= ~irqs;}
	

	friend usart& operator << (usart& out, const char* str);

	friend usart& operator << (usart& out, const char ch);
//	friend usart& operator << (usart& out, const unsigned dat);
	friend usart& operator << (usart& out, const uint8_t dat);
	friend usart& operator << (usart& out, const uint16_t dat);
	friend usart& operator << (usart& out, const uint32_t dat);
//	friend usart& operator << (usart& out, const uint64_t dat);

//	friend usart& operator << (usart& out, const int dat);
	friend usart& operator << (usart& out, const int8_t dat);
	friend usart& operator << (usart& out, const int16_t dat);
	friend usart& operator << (usart& out, const int32_t dat);
	friend usart& operator << (usart& out, const int64_t dat);

	friend usart& operator << (usart& out, const float dat);
	friend usart& operator << (usart& out, const double dat);
	friend usart& operator << (usart& out, const long double dat);

	friend usart& operator >> (usart& in, char* str);

	friend usart& operator >> (usart& in, char &dat);
//	friend usart& operator >> (usart& in, unsigned &dat);
	friend usart& operator >> (usart& in, uint8_t &dat);
	friend usart& operator >> (usart& in, uint16_t &dat);
	friend usart& operator >> (usart& in, uint32_t &dat);
//	friend usart& operator >> (usart& in, uint64_t &dat);

//	friend usart& operator >> (usart& in, int &dat);
	friend usart& operator >> (usart& in, int8_t &dat);
	friend usart& operator >> (usart& in, int16_t &dat);
	friend usart& operator >> (usart& in, int32_t &dat);
//	friend usart& operator >> (usart& in, int64_t &dat);

	friend usart& operator >> (usart& in, float &dat);
	friend usart& operator >> (usart& in, double &dat);
	friend usart& operator >> (usart& in, long double &dat);
};

char hexChar(uint8_t val);
inline void USARTdisable() {UCSR0B = 0;}

#endif

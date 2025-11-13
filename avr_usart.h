/*
 * Atmega328 USART functions header file
 * Part of Atmega328 register and peripherals library
 *
 * Documentation available in the provided MD file.
 *
 * (c) 2021-2022 Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

#ifndef ATMEGA328_USART_H_
#define ATMEGA328_USART_H_

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
	void printHex(unsigned val);

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
	
/*	void CLKenable(uint8_t cpol = 0, uint8_t cpha = 0, uint8_t lbcl = 0)
		{_USART1_(USART_CR2) |= USART_CR2_CLKEN | (cpha << 9) | (cpol << 10) | (lbcl << 8);}
	void CLKdisable() {_USART1_(USART_CR2) &= ~(USART_CR2_CLKEN | 0x0700);}
	void RTSenable() {_USART1_(USART_CR3) |= USART_CR3_RTSE;}
	void RTSdisable() {_USART1_(USART_CR3) &= ~USART_CR3_RTSE;}
	void CTSenable() {_USART1_(USART_CR3) |= USART_CR3_CTSE;}
	void CTSdisable() {_USART1_(USART_CR3) &= ~USART_CR3_CTSE;}
	void HDenable() {_USART1_(USART_CR3) |= USART_CR3_HDSEL;}
	void HDdisable() {_USART1_(USART_CR3) &= ~USART_CR3_HDSEL;}
	void DMARXenable() {_USART1_(USART_CR3) |= USART_CR3_DMAR;}
	void DMARXdisable() {_USART1_(USART_CR3) &= ~USART_CR3_DMAR;}
	void DMATXenable() {_USART1_(USART_CR3) |= USART_CR3_DMAT;}
	void DMATXdisable() {_USART1_(USART_CR3) &= ~USART_CR3_DMAT;}
	void sendBreak() {_USART1_(USART_CR1) |= USART_CR1_SBK;}

	void wordLength(uint8_t M) {BB_USART1_CR1_M = M&0x01;}
	void stopBits(uint8_t stp) {_USART1_(USART_CR2) &= ~USART_CR2_STOP; _USART1_(USART_CR2) |= stp << 12;}
	void parityEnable(uint8_t ps) {_USART1_(USART_CR1) |= USART_CR1_PCE | (ps<<9);}
	void parityDisable() {_USART1_(USART_CR1) &= ~(USART_CR1_PCE | USART_CR1_PS);}



	void clearCTS() {_USART1_(USART_SR) &= ~USART_SR_CTS;}
	uint16_t checkError() {return (_USART1_(USART_SR) & 0x001F);}
	uint16_t checkIdle() {return (_USART1_(USART_SR) & 0x0010);}
	uint16_t readDR() {return _USART1_(USART_DR);}

	void sendStreamDMA(uint8_t* dat, uint16_t size);
	void getStreamDMA(uint8_t* dat, uint16_t size);
	void startTX(uint8_t* dat, uint16_t size, uint16_t pri = 0x1000);
	void stopTX();
	void startRX(uint8_t* dat, uint16_t size, uint16_t pri = 0x1000);
	void stopRX();

*/
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

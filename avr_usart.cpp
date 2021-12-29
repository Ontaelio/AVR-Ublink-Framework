/*
 * Atmega328 USART functions source file
 * Part of Atmega328 register and peripherals library
 *
 * Documentation available in the provided MD file.
 *
 * (c) 2021-2022 Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

#include <avr_usart.h>
#include <stdio.h>

usart::usart(uint32_t baud)
{
	if (baud) init(baud);
}

void usart::init(uint32_t baud)
{
	uint16_t ubrr = F_CPU/16/baud - 1;
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	// Enable receiver and transmitter 
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	// Set frame format: 8 bit data, 2 stop bits
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void usart::initTX(uint32_t baud)
{
	uint16_t ubrr = F_CPU/16/baud - 1;
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	// Enable transmitter 
	UCSR0B = (1<<TXEN0);
	// Set frame format: 8 bit data, 2 stop bits
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void usart::initRX(uint32_t baud)
{
	uint16_t ubrr = F_CPU/16/baud - 1;
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	// Enable receiver 
	UCSR0B = (1<<RXEN0);
	// Set frame format: 8 bit data, 2 stop bits
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void usart::sendByte(uint8_t dat)
{
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = dat;
	/*
	while(!(_USART1_(USART_SR) & USART_SR_TC));
	_USART1_(USART_DR) = dat;
	*/
}

uint8_t usart::getByte()
{
	//uint8_t res;
	while(!(UCSR0A & (1<<RXC0))); //_USART1_(USART_SR) & USART_SR_RXNE));
	//res = UDR0;
	return UDR0;
}


void usart::dump()
{
	uint8_t dummy;
	while (UCSR0A & (1<<RXC0)) dummy = UDR0;
}

void usart::sendStream(uint8_t* dat, uint32_t size)
{
	for (uint32_t k = 0; k<size; k++)
	{
		sendByte(dat[k]);
	}
}

void usart::printString(const char *str)
{
	while (*str) //print chars until termination encountered
	{
		sendByte(*str);
		++str;
	}
}

#ifdef USE_BUFFER_FOR_INTS

//this version uses buffer. May be useful if something goes wrong
void usart::printUInt(uint32_t a)
{
	char buf[10];
	uint_fast8_t t = 0;
	do
	{
		buf[t] = ((a % 10) + '0');
		a/=10;
		++t;
	} while(a);

	do
	{
		--t;
		sendByte(buf[t]);

	} while(t);
}

#else

//this version uses stack. Faster and smaller, but MAY produce errors with interrupts
void usart::printUInt(uint32_t a)
{
	uint_fast8_t cnt = 0;
	uint_fast8_t t;
	do
	{
		t = ((a % 10) + '0');
		__asm__ __volatile__ ("push {%0}" ::"r"(t):);
		a /= 10;
		++cnt;
	} while (a);

	do
	{
		__asm__ __volatile__ ("pop {%0}" :"=r"(t)::);
		sendByte((char)t);
		--cnt;
	} while (cnt);
}

#endif

void usart::printInt(int32_t a)
{
	if (a < 0) { sendByte('-'); a = abs(a); }
	printUInt(a);
}

void usart::fprintUInt(uint32_t a, uint_fast8_t s)
{
	uint32_t pow10[10] = {
     1, 10, 100, 1000, 10000,
      100000, 1000000, 10000000, 100000000, 1000000000
    };
	if (s>10) s=10;
	while((s>0) && (a<pow10[--s])) sendByte('0');
	if (a>0) printUInt(a);
}


void usart::printFloat(long double a, uint_fast8_t s)
{

#ifdef NAN_INFINITY_ENABLED

	if (!isfinite(a))
	{
		if (isnan(a)) {printString("NaN"); return;}
		if (a<0) sendByte('-');
		printString("infinity");
		return;
	}

#endif

	if (a<0) { sendByte('-'); a = abs(a); }
	printUInt((uint32_t)a);
	sendByte('.');
	a -= (uint32_t)a;
	while(s-- && (a>0))
	{
		a*=10;
		printUInt(a);
		a -= (uint32_t)a;
	}
}

void usart::printHex(unsigned val)
{
	uint8_t k = 32;
	printString("0x");
	while (k-=4)
		if (val >> k) sendByte(hexChar((val >> k)&0x0F));
	sendByte(hexChar(val&0x0F));
}



void usart::getStream(uint8_t* dat, uint16_t size)
{
	for (uint16_t k = 0; k<size; k++)
	{
		dat[k] = getByte();
	}
}

char usart::getChar()
{
	char res;
	res = getByte();
	return res;
}


uint64_t usart::getUInt()
{
	uint64_t res = 0;
	uint8_t b;
	do b = getByte() - 48; while ((uint8_t)b>9);
	do
	{
		res = res*10 + b;
		b = getByte() - 48;
	} while (b<10);
	return res;
}

int64_t usart::getInt()
{
	int64_t res = 0;
	uint8_t b;
	int8_t neg = 0;
	do b = getByte(); while ((uint8_t)(b-48)>9 && (b!='-'));
	if (b == '-') {neg = 1; b = getByte() - 48;} else b-=48;
	do
	{
		res = res*10 + b;
		b = getByte() - 48;
	} while (b<10);
	if (neg) res = 0-res;
	return res;
}

long double usart::getFloat()
{
	long double res = 0;
	uint32_t divider = 10;
	uint8_t b = 0;
	int8_t neg = 0;
	do {b = getByte(); if (b == '-') neg = 1; b-=48;} while ((uint8_t)b>9);
	do
	{
		res = res*10 + b;
		b = getByte() - 48;
	} while (b<10);
	if (b != 254) {if (neg) res = 0-res; return res;}
	res = (uint32_t)res;
	b = getByte() - 48;
	while (b<10)
	{
		res = res + (double)b/divider;
		b = getByte() - 48;
		divider *= 10;
	}
	if (neg) res = 0-res;
	return res;
}

void usart::getString(char* arr)
{
	char c;
	do
	{
		c = getByte();
		if (c>31) {*arr = c; ++arr;}
	} while (c>31);
	*arr = 0;
}


void usart::printUInt64(uint64_t a)
{
	char buf[20];
	uint_fast8_t t = 0;
	do
	{
		buf[t] = ((a % 10) + '0');
		a/=10;
		++t;
	} while(a);

	do sendByte(buf[--t]); while(t);
}

void usart::printInt64(int64_t a)
{
	if (a < 0) { sendByte('-'); a = abs(a); }
	printUInt64(a);
}

//not sure about this. Can be substituted in cout or totally omitted
//if omitted should mention c_str in docs.
//void usart::printCPPString(std::string s)
//{
//	printString(s.c_str());
//}


usart& operator << (usart& out, const char* str)
{
	out.printString(str); return out;
}

/*usart& operator << (usart& out, const unsigned dat)
{
	out.printUInt(dat); return out;
}
*/
usart& operator << (usart& out, const uint8_t dat)
{
	out.printUInt(dat); return out;
}

usart& operator << (usart& out, const uint16_t dat)
{
	out.printUInt(dat); return out;
}

usart& operator << (usart& out, const uint32_t dat)
{
	out.printUInt(dat); return out;
}

/*usart& operator << (usart& out, const uint64_t dat)
{
	out.printUInt64(dat); return out;
}
*/
usart& operator << (usart& out, const float dat)
{
	out.printFloat(dat); return out;
}

usart& operator << (usart& out, const double dat)
{
	out.printFloat(dat); return out;
}

usart& operator << (usart& out, const long double dat)
{
	out.printFloat(dat); return out;
}

/*usart& operator << (usart& out, const int dat)
{
	out.printInt(dat); return out;
}
*/
usart& operator << (usart& out, const int8_t dat)
{
	out.printInt(dat); return out;
}

usart& operator << (usart& out, const int16_t dat)
{
	out.printInt(dat); return out;
}

usart& operator << (usart& out, const int32_t dat)
{
	out.printInt(dat); return out;
}

/*usart& operator << (usart& out, const int64_t dat)
{
	out.printInt64(dat); return out;
}
*/
usart& operator << (usart& out, const char ch)
{
	out.sendByte(ch); return out;
}

usart& operator >> (usart& in, char* str)
{
	in.getString(str);
	return in;
}

usart& operator >> (usart& in, char &dat)
{
	dat = in.getChar();
	return in;
}

/*usart& operator >> (usart& in, unsigned &dat)
{
	dat = in.getUInt();
	return in;
}
*/
usart& operator >> (usart& in, uint8_t &dat)
{
	dat = in.getUInt();
	return in;
}

usart& operator >> (usart& in, uint16_t &dat)
{
	dat = in.getUInt();
	return in;
}

usart& operator >> (usart& in, uint32_t &dat)
{
	dat = in.getUInt();
	return in;
}

/*usart& operator >> (usart& in, uint64_t &dat)
{
	dat = in.getUInt();
	return in;
}
*/

/*usart& operator >> (usart& in, int &dat)
{
	dat = in.getInt();
	return in;
}
*/
usart& operator >> (usart& in, int8_t &dat)
{
	dat = in.getInt();
	return in;
}

usart& operator >> (usart& in, int16_t &dat)
{
	dat = in.getInt();
	return in;
}

usart& operator >> (usart& in, int32_t &dat)
{
	dat = in.getInt();
	return in;
}

/*usart& operator >> (usart& in, int64_t &dat)
{
	dat = in.getInt();
	return in;
}
*/
usart& operator >> (usart& in, float &dat)
{
	dat = in.getFloat();
	return in;
}

usart& operator >> (usart& in, double &dat)
{
	dat = in.getFloat();
	return in;
}

usart& operator >> (usart& in, long double &dat)
{
	dat = in.getFloat();
	return in;
}

/*




void usart1::sendStreamDMA(uint8_t* dat, uint16_t size)
{
	DMATXenable();
	//channel 4 (TX), paddr, maddr, num,  ccr = high pri, 8 bit mem, 8 bit periph, memory increments, non-circular
	//mem2per
	dma1_setup( 4, //0x40013804,
				(uint32_t)&(_USART1_(USART_DR)),
				(uint32_t)dat,
				size,
				(uint16_t)(DMA_PLHIGH | DMA_MSIZE8 | DMA_PSIZE8 | DMA_CCR_MINC | DMA_CCR_DIR));
	while(!(_USART1_(USART_SR) & USART_SR_TC));
	dma1_enable(4);
}


void usart1::getStreamDMA(uint8_t* dat, uint16_t size)
{
	DMARXenable();
	//channel 5 (RX), paddr, maddr, num,  ccr = high pri, 8 bit mem, 8 bit periph, memory increments, non-circular
	dma1_setup( 5, //0x40013804,
				(uint32_t)&(_USART1_(USART_DR)),
				(uint32_t)dat,
				size,
				(uint16_t)(DMA_PLHIGH | DMA_MSIZE8 | DMA_PSIZE8 | DMA_CCR_MINC));
	dma1_enable(5);
}

void usart1::startTX(uint8_t* dat, uint16_t size, uint16_t pri)
{
	DMATXenable();
	//channel 4 (TX), paddr, maddr, num,  ccr = pri, 8 bit mem, 8 bit periph, memory increments, from mem, circular
	dma1_setup( 4, //0x40013804,
				(uint32_t)&(_USART1_(USART_DR)),
				(uint32_t)dat,
				size,
				(uint16_t)(pri | DMA_MSIZE8 | DMA_PSIZE8 | DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_CIRC));
	while(!(_USART1_(USART_SR) & USART_SR_TC));
	dma1_enable(4);
}

void usart1::startRX(uint8_t* dat, uint16_t size, uint16_t pri)
{
	DMARXenable();
	//channel 5 (RX), paddr, maddr, num,  ccr = pri, 8 bit mem, 8 bit periph, memory increments, circular
	dma1_setup( 5, //0x40013804,
				(uint32_t)&(_USART1_(USART_DR)),
				(uint32_t)dat,
				size,
				(uint16_t)(pri | DMA_MSIZE8 | DMA_PSIZE8 | DMA_CCR_MINC | DMA_CCR_CIRC));
	dma1_enable(5);
}

void usart1::stopTX() {dma1_disable(4); DMATXdisable();}
void usart1::stopRX() {dma1_disable(5); DMARXdisable();}

*/

char hexChar(uint8_t val)
{
	char c;
	if (val > 9) c = val + 55;
	else c = val + 48;
	return c;
}

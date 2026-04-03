# UBlink Atmega328p SPI libraries

[General](#general)\
[Hardware SPI](#hardware-spi)

* [Hardware SPI :: Initialization](#initialization)

## General

### Class naming

In previous versions of this API, class names used to reflect the role of the device the MCU is talking to, not the MCU’s activity, e.g. `SpiSlave` object was used for Master mode. Current version simplified this with the `Spi` classes that assume Master mode for the MCU, as it's the most common one. Slave mode class, however, follows the previous designation and is called `SpiBus`, reflecting the role of the hardware the MCU is connected to (i.e. the bus). Older names are aliased for compatibility:

`SpiSlave` == `Spi`\
`SpiMaster` == `SpiBus`\
`UsartSpiSlave` == `UsartSpi`\
`SoftSpiSlave` == `SoftSpi`

This documentation follows the slave/master syntax (as in datasheet) to avoid confusion caused by modern unstable renamings.

### Multiple devices

If multiple slaves are connected to the SPI bus, one of the following approaches can be used:

* create a `Spi` object for each device, then `enable()`/`disable()` them as needed:

```C++
DigitalPin memCsPin(...);
DigitalPin ledCsPin(...);
Spi memChip(memCsPin);
Spi ledScreen(ledCsPin);
...
memChip.enable();
memChip.readStream(screenArray, len);
memChip.disable(); // may be omitted
ledScreen.enable(); 
ledScreen.writeStrean(screenArray, len);
ledScreen.disable(); // may be omitted
```

* create one `Spi` object (e.g. `spiBus`) and switch between slaves with `begin(DigitalPin cs)`:

```C++
DigitalPin memCsPin(...);
DigitalPin ledCsPin(...);
Spi spiBus;
...
spiBus.enable();
spiBus.begin(memCsPin)
      .read(screenArray, len)
      .end();
spiBus.begin(ledCsPin)
      .write(screenArray, len)
      .end();
```

The first approach is recommended if different slaves use different speed/CPOL/CPHA/LSB settings, as each instance of the `Spi` class keeps its configuration.

## Hardware SPI - Master mode - Spi class

The library supports full functionality of Atmega328p's SPI hardware except the Write Collision detection. The following pins are used by the hardware SPI and are configured and controlled by the `Spi` class objects:

Function  | Pin | Arduino pin
----------|-----|------------
MOSI      | B3  | 11
MISO      | B4  | 12
SCK       | B5  | 13
SS*       | B2  | 10

* _any pin can be used as SS, default one is listed. The default SS pin must remain in OUTPUT mode during SPI operation even if unused_

### Initialization

#### Constructors

**Spi()** creates an object of the `Spi` class with default CS (Chip Select aka Slave Select) pin (PB2 aka pin 10 on Arduino).

**Spi(DigitalPin gpin)** creates an object of the `Spi` class with `gpin` as the CS pin.

#### Settings

Spi& **LSBfirst()**: LSB of the data byte transmitted first.

Spi& **MSBfirst()**: MSB of the data byte transmitted first (default).

Spi& **polarity(uint8_t pol)**: if `pol == 1`, SCK is high when idle, else SCK is low when idle (default 0).

Spi& **phase(uint8_t ph)**: if `ph == 1`, data is sampled on the trailing edge of SCK, else on the leading edge of SCK (default 0).

Spi& **clock(uint8_t dvd)**: selects the SPI clock rate. The following macros can be used as an argument here: `SPI_DIV4`, `SPI_DIV16`, `SPI_DIV64` and `SPI_DIV128`. The number in the `DIVX` part represents the frequency divisor, as in `F_CPU/DIVX`. Default is `SPI_DIV4`.

Spi& **speed2x()**: double the SPI speed, effectively halving the devisor (default off).

Spi& **IRQenable()**\
Spi& **IRQdisable()**: enable/disable the SPI IRQ; see the ISR section for details.

#### SPI activation

void **enable()** enables hardware SPI in Master Mode with the settings provided. CS is set HIGH, SPIF is cleared if set.

void **disable()** pulls CS pin high and disables hardware SPI.

#### Notes

All settings methods write into the internal configuration variable and are applied when the `enable()` method is called. IRQ methods _also_ write directly into the SPI Control register (see below).

All settings methods are chainable, and in most cases can be concluded with the `enable()` method, e.g. `spiDevice.speed2x().enable()` will configure SPI at 8 MHz (F_CPU/2) and enable it in master mode. Default settings are:

```C++
spiDevice.MSBfirst()
         .polarity(0)
         .phase(0)
         .clock(SPI_DIV4) // no .speed2x()
         .IRQdisable()
```

### Fluent interface

In fluent interface, each exchange starts with begin() and finishes with end(). Most of the methods are chainable, except returning read() and transfer(uint8_t) and terminating end(), e.g.:

```c++
spiDevice.begin()
         .write(cmd);
responceByte = spiDevice.read();
spiDevice.transfer(dataOut, dataIn, len)
         .write(dataout, len)
         .end();
```

is equal to

```c++
spiDevice.begin()
         .write(cmd);
         .read(responceByte);
         .transfer(dataOut, dataIn, len)
         .write(dataout, len)
         .end();
```

#### Chainable methods

Spi& **begin()**: open SPI communication by setting CS low.

Spi& **begin(DigitalPin cs_pin)**: change CS pin to `cs_pin`, then open SPI communication by setting it low

Spi& **latch()**: set CS high, them immediately set CS low.

Spi& **transfer(const uint8_t\* tx, uint8_t\* rx, uint16_t len)**: full-duplex transfer of `len` bytes from an array `tx` to Spi and from Spi into an array `rx`.

Spi& **write(uint8_t dat)**: send a single byte `dat`.

Spi& **write(const uint8_t\* tx, uint16_t len)**: send `len` bytes from an array `tx`.

Spi& **read(uint8_t& received)**: read a single byte into `received` variable.

Spi& **read(uint8_t\* rx, uint16_t len)**: receive `len` bytes into an array `rx`.

#### Reading methods

uint8_t **transfer(uint8_t dat)**: initiate a full-duplex transfer of one byte; `dat` goes out, returns whatever came back while `dat` was being shifted.

uint8_t **read()**: receive a single byte.

#### Terminating method

void **end()**: close SPI communication by setting CS high.

### Transactional interface

Methods of transactional interface contain `begin()` and `end()` wrappers, the connection to the slave is opened and closed inside the method. `begin()` can still be used outside of transactional methods to switch between slaves.

uint8_t **single(uint8_t dat)**: transfer single byte.

void **writeStream(const uint8_t\* dat, uint16_t len)**: send `len` of data from an array `dat`.

void **readStream(uint8_t\* dat, uint16_t len)**: read `len` of data into an array `dat`.

void **transferStream(const uint8_t\* arr_out, uint8_t\* arr_in, uint16_t len)**: full-duplex transfer of `len` bytes from `arr_out` and into `arr_in`.

void **seqTransfer(const uint8_t\* arr_out, uint16_t num_out, uint8_t\* arr_in, uint16_t num_in)**: sequentional transaction. First, it sends `num_out` bytes from an array `arr_out`, then proceeds to read `num_in` bytes into an array `arr_in`. Useful for communications with memory chips that receive a command, then return data - in one single session.

### ISR methods

Atmega328's SPI has a single interrupt - SPI Serial Transfer Complete. The interrupt service routine should be placed in your code outside of `main()` like this:

```c++
ISR(SPI_STC_vect)
{
    //some actions;
}

```

This library does not provide any `attachInterrupt` routins as they are contrary to the concept of bare metal programming.

Spi& **IRQenable()** enables the interrupt;\
Spi& **IRQdisable()** disables the interrupt.

Note that methods above set the ISR bit in _both_ the internal configuration variable used in `enable()` _and_ the hardware SPI control register, instantly enabling/disabling the interrupt. This feature is needed in most ISR applications to quickly change interrupt behaviour.

In interrupt mode, only `begin()` and `end()` from the methods above are useful, as the rest are blocking and should not be used in the ISR.

To read and write bytes, the overloaded `=` operator is used:

`(Spi) spiDevice = (uint8_t) value;` writes `value` into SPI data register;\
`(uint8_t) receivedByte = (Spi) spiDevice;` reads contents of SPI data register into `receivedByte`.

void **clear()** clears the interrupt flag and frees the bus for the next transmission.

## Hardware SPI - Slave mode - SpiBus class

The spi.hpp library provides full functionality for SPI Hardware in slave mode, except collision detection. The following shuld be used and are configured and controlled by the `SpiBus` class object:

Function  | Pin | Arduino pin
----------|-----|------------
MOSI      | B3  | 11
MISO      | B4  | 12
SCK       | B5  | 13
SS        | B2  | 10

### Constructor

**SpiBus** constructor has no arguments.

### Settings

Chainable configuration methods are the same as in `Spi` class except speed settings:

Spi& **LSBfirst()**\
Spi& **MSBfirst()**\
Spi& **polarity(uint8_t pol)**\
Spi& **phase(uint8_t ph)**\
Spi& **IRQenable()**\
Spi& **IRQdisable()**

### SPI activation

void **enable(uint8_t firstByte = 0xFF)** configures SPI pins, enables hardware SPI in Slave Mode with the settings provided, and prepares `firstByte` for transfer once the Master initiates it.

void **disable()** disables hardware SPI.

### ISR mode

```c++
uint8_t nextByte, receivedByte = 0xFF;
spiBus masterBus;

ISR(SPI_STC_vect){
    receivedByte = masterBus.transfer(nextByte);
}
```

or

```c++
ISR(SPI_STC_vect){
    receivedByte = masterBus;
    nextByte = processByte(receivedByte); // must be very quick to allow collision
    masterBus = nextByte;
}
```

### Polling mode

## USART SPI - UsartSpi class

USART on Atmega328p has a dedicated SPI mode. In this mode, USART capabilities are replaced with SPI ones (thus, no Serial connection possible). USART SPI can function only as a Master, and uses the following piout:

Function  | Pin | Arduino pin
----------|-----|------------
MOSI      | D0  | 0 (RX)
MISO      | D1  | 1 (TX)
SCK       | D4  | 4
SS        | Any | Any

USART SPI library has the same interface as the Hardware SPI one. All the methods listed for `Spi` can be used in the `UsartSpi` class; only the object declaration needs to be changed to switch between the two. The following are two differences that should be taken into account.

### Speed settings

Speed setting differ on the UART SPI, as it allows setting an exact value for the divisor, not a preset one. The formula is f_SCK = f_CPU / (2 * (set_speed + 1)). Thus, `clock()` setting in `UsartSpi` accepts any value (even 16-bit) for `set_speed`. The following table shows USART SPI `clock()` values correcponding to the hardware SPI divisors:

| Hardware SPI  |  USART SPI  |
| ------------- |  ---------- |
| SPI_DIV4, 2x  |  0          |
| SPI_DIV4      |  1          |
| SPI_DIV16, 2x |  3          |
| SPI_DIV16     |  7          |
| SPI_DIV64, 2x |  15         |
| SPI_DIV64     |  31         |
| SPI_DIV128    |  63         |

`speed2x()` method will alwasys set the maximum transmission speed (f_CPU/2).

### Interruprs

USART SPI has three interrupts. The default `IRQenable()` and `IRQdisable()` control the USART Rx Complete interrupt. This is basically the same thing as the SPI interrupt.

Additionally, Tx Complete and Data Register Empty interrupts are controlled by the following methods:

UsartSpi& **IRQenableTX()**
UsartSpi& **IRQdisableTX()**
UsartSpi& **IRQenableUDRE()**
UsartSpi& **IRQdisableUDRE()**

These also both set and reset bits in the configuration _and_ actually enable and disable the corresponding interrupts in the hardware.

USART SPI interrupts are handled by the following ISRs:

```c++
ISR(USART_RX_vect){}
ISR(USART_TX_vect){}
ISR(USART_UDRE_vect){}
```

_Note: USART SPI pipelined read() is a tiny bit (1-2 clock ticks per byte) faster than the SPI one, as the hardware allows seamless data flow._

## Software SPI - SoftSpi class

Software SPI utilizes the bit-bang approach to the SPI sommunication. Any pins can be used. Software SPI library has the same interface as the Hardware SPI one. All the methods listed for `Spi` can be used in the `SoftSpi` class; only the object declaration needs to be changed to switch between the two. The following are key differences that should be taken into account.

### Mode od operation

Only SPI Mode 0 is supported (Sample on the Rising edge of SCK, Setup on the Falling edge).

### Constructor

You must declare `DigitalPin` objects for four pins: MISO, MOSI, SCK and SS. Then, pass these pins to the constructor as follows:

**SoftSpi (DigitalPin MISO, DigitalPin MOSI, DigitalPin SCK, DigitalPin SS)**

Pin modes will be applied automatically with the `enable()` method.

### Settings

Only `clock()` and `speed2x()` settings are present, although they do nothing. The rest of the setting methods produce errors. This is done to avoid undefined behaviour, as the Software SPI can only work in Mode 0.

### Speed configuration

This Software SPI will try to work as fast as possible, taking around 25-27 MCU cycles per bit (~f_CPU/26 or ~615 kHz). However, the master samples the MISO line approximately 4 CPU cycles after the rising edge of SCK (~250 ns at 16 MHz). Therefore, the SPI slave must provide valid data on MISO within this time. It is possible to slow the connection down by defining a `SOFTSPI_DELAY()` macro, e.g.:

```c++
#define SOFTSPI_DELAY() \
    __asm__ __volatile__ ("nop\n\t" "nop\n\t" "nop\n\t")
```

`SOFTSPI_DELAY()` will be called three times per each bit; each NOP in it will increase the f_CPU divisor by 3. Some approximate timigs are below:

 \# of NOPs | f_CPU div | Ticks per byte | SPI frequency at 16 mHz
 ---------- | --------- | -------------- | -------
 0          | 26        | 208            | 615 kHz
 1          | 29        | 232            | 552 kHz
 3          | 35        | 280            | 457 kHz
 5          | 41        | 328            | 390 kHz

**Note:** while `SOFTSPI_DELAY()` will decrease the overall speed thrice, it will decrease the MISO sampling time only once, as the three occurences are spread throught the transfer routine.

For even slower operation, `_delay_ns` or even `_delay_ms` can be used in `SOFTSPI_DELAY()`.

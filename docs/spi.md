# UBlink Atmega328p SPI libraries

[General](#general)\
[Hardware SPI](#hardware-spi)

* [Hardware SPI :: Initialization](#initialization)

## General

### Class naming

In this API, **class names reflect the role of the device the MCU is talking to**, not the MCU’s activity, e.g.:\
`SpiSlave w25q32;` -- `w25q32` has a _reactive_ role, it awaits a command from the MCU, and responds to it;\
`SpiMaster stm32;` -- `stm32` has an _active_ role, the MCU awaits a command from it.\
This documentation follows the slave/master syntax to avoid confusion. For _modern audiences(tm)_ class names aliases are provided in the library:\
`SpiPeripheral`\
`SpiDevice`\
`SpiHost`\
`SpiController`\
Aliaces require C++11, older toolchains may not support them. Please download the latest one (ver 4.0.0+ from Microchip).

### Multiple devices

If multiple slaves are connected to the SPI bus, one of the following approaches can be used:

* create a `SpiSlave` object for each device, then `enable()`/`disable()` them as needed:

```C++
DigitalPin memCsPin(...);
DigitalPin ledCsPin(...);
SpiSlave memChip(memCsPin);
SpiSlave ledScreen(ledCsPin);
...
memChip.enable();
memChip.readStream(screenArray, len);
memChip.disable(); // may be omitted
ledScreen.enable(); 
ledScreen.writeStrean(screenArray, len);
ledScreen.disable(); // may be omitted
```

* create one `SpiSlave` object (e.g. `spiBus`) and switch between slaves with `begin(DigitalPin cs)`:

```C++
DigitalPin memCsPin(...);
DigitalPin ledCsPin(...);
SpiSlave spiBus;
...
spiBus.enable();
spiBus.begin(memCsPin)
      .read(screenArray, len)
      .end();
spiBus.begin(ledCsPin)
      .write(screenArray, len)
      .end();
```

The first approach is recommended if different slaves use different speed/CPOL/CPHA/LSB settings, as each instance of the `SpiSlave` class keeps its configuration.

## Hardware SPI

### Initialization

#### Constructors

**SpiSlave()** creates an object of the `SpiSlave` class with default CS (Chip Select aka Slave Select) pin (PB2 aka pin 10 on Arduino).

**SpiSlave(DigitalPin gpin)** creates an object of the `SpiSlave` class with `gpin` as the CS pin.

#### Settings

SpiSlave& **LSBfirst()**: LSB of the data byte transmitted first.

SpiSlave& **MSBfirst()**: MSB of the data byte transmitted first (default).

SpiSlave& **polarity(uint8_t pol)**: if `pol == 1`, SCK is high when idle, else SCK is low when idle (default 0).

SpiSlave& **phase(uint8_t ph)**: if `ph == 1`, data is sampled on the trailing edge of SCK, else on the leading edge of SCK (default 0).

SpiSlave& **clock(uint8_t dvd)**: selects the SPI clock rate. The following macros can be used as an argument here: `SPI_DIV4`, `SPI_DIV16`, `SPI_DIV64` and `SPI_DIV128`. The number in the `DIVX` part represents the frequency divisor, as in `F_CPU/DIVX`. Default is `SPI_DIV4`.

SpiSlave& **speed2x()**: double the SPI speed, effectively halving the devisor (default off).

SpiSlave& **IRQenable()**\
SpiSlave& **IRQdisable()**: enable/disable the SPI IRQ; see the ISR section for details.

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

In fluent interface, each exchange starts with begin() and finishes with end(). Most of the methods are chainable, except single read() and transfer() (returning methods) and terminating end(), e.g.:

```c++
spiDevice.begin()
         .write(cmd);
responceByte = spiDevice.read();
spiDevice.transfer(dataOut, dataIn, len)
         .write(dataout, len)
         .end();
```

#### Chainable methods

SpiSlave& **begin()**: open SPI communication by setting CS low.

SpiSlave& **begin(DigitalPin cs_pin)**: change CS pin to `cs_pin`, then open SPI communication by setting it low

SpiSlave& **latch()**: set CS high, them immediately set CS low.

SpiSlave& **transfer(const uint8_t\* tx, uint8_t\* rx, uint16_t len)**: full-duplex transfer of `len` bytes from an array `tx` to SpiSlave and from SpiSlave into an array `rx`.

SpiSlave& **write(uint8_t dat)**: send a single byte `dat`.

SpiSlave& **write(const uint8_t\* tx, uint16_t len)**: send `len` bytes from an array `tx`.

SpiSlave& **read(uint8_t\* rx, uint16_t len)**: receive `len` bytes into an array `rx`.

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

SpiSlave& **IRQenable()** enables the interrupt;\
SpiSlave& **IRQdisable()** disables the interrupt.

Note that methods above set the ISR bit _both_ in the internal configuration variable used in `enable()` _and_*_ in the hardware SPI control register, instantly enabling/disabling the interrupt. This feature is needed in most ISR application to quickly change interrupt behaviour.

In interrupt mode, only `begin()` and `end()` from the methods above are useful, as the rest are blocking and should not be used in the ISR.

To read and write bytes, the overloaded `=` operator is used:

`(SpiSlave) spiDevice = (uint8_t) value;` writes `value` into SPI data register;\
`(uint8_t) receivedByte = (SpiSlave) spiDevice;` reads contents of SPI data register into `receivedByte`.

void **clear()** clears the interrupt flag and frees the connection for the next transmission.

## USART SPI

## Software SPI

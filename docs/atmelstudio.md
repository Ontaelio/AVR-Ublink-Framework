Both Atmel Studio and Microchip Studio use old avr-gcc aka AVR8/GNU C Compiler 5.4.0 (C++ 11 or earlier). It does not support inline variables.

To fix:

* download a newer AVR 8-bit GNU Toolchain;
* unpack it and copy the path to `bin` folder inside;
* Tools -> Options -> Toolchain -> under Toolchains select 'Atmel AVR 8-bit (CPP Language) -> Add Flavour. Type a name and paste the path to `bin`. Set as default.
* restart the Studio.

Note: the default will apply to new projects. In an existing ptoject:

* Project -> Properties -> Advanced -> Toolchain Flavour.

Alternatively, you may create your own toolchain with CMake.
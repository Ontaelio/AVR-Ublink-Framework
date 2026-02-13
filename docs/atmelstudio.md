Both Atmel Studio and Microchip Studio use old avr-gcc aka AVR8/GNU C Compiler 5.4.0 (C++ 11 or earlier). It does not support inline variables.

To fix:

* download a newer AVR 8-bit GNU Toolchain;
* unpack it and copy the path to `bin` folder inside;
* Tools -> Options -> Toolchain -> under Toolchains select 'Atmel AVR 8-bit (CPP Language) -> Add Flavour. Type a name and paste the path to `bin`. Set as default.
* restart the Studio.

Note: the default will apply to new projects. In an existing ptoject:

* Project -> Properties -> Advanced -> Toolchain Flavour.

Alternatively, you may create your own toolchain with CMake.

Then:

* download [avrdude](https://github.com/avrdudes/avrdude/releases)
* Tools -> External Tools
* Provide Title
* Command: path to avrdude.exe (avrdude.exe included)
* Initial directory: `-C"<path to avrdude.conf>" -v -patmega328p -carduino -PCOM# -b115200 -D -Uflash:w:"$(ProjectDir)Debug\$(TargetName).hex":i` Change path to avrdude.conf AND -PCOM# (port the board is connected to)
* Add, OK.

Now you will have a loader in Tools -> Title (provided at step 3)

Do not forget to:
`#include <stdint.h>`\
`#include <stdlib.h>`\
`#include <avr/io.h> // these three are already included`\
`#include <avr/interrupt.h> // ISR`\
`#include <avr/pgmspace.h> // PROGMEM`
`#include <util/delay.h> // _delay_ms()`

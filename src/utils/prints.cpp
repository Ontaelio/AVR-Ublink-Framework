/*
 * Utils to print registers for troubleshooting
 * Part of Ublink Atmega328 register and peripherals framework
 *
 * Documentation available in the provided MD file.
 *
 * (c) 2026 Dmitry Reznikov ontaelio(at)gmail.com
 *
 * Can be freely used according to the GNU GPL license.
 */

 #include <stdint.h>
 #include <avr_usart.h>

 void printTimer1(){
    usart term(9600);
    term << "Hello there";

 }
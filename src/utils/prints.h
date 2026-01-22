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

#ifndef UTILS_PRINTS_H_
#define UTILS_PRINTS_H_

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr_usart.h>
#include <timers16.h>
#include <timers8.h>

void printTimer0(usart term);
void printTimer1(usart term);
void printTimer2(usart term);
void printTimer16Config(usart term, timer1 tim);
void printTimer16Profile(usart term, timer1 tim);
void printTimer8Config(usart term, timer0 tim);
void printTimer8Config(usart term, timer2 tim);
void printTimer8Profile(usart term, timer0 tim);
void printTimer8Profile(usart term, timer2 tim);

#endif // UTILS_PRINTS_H_
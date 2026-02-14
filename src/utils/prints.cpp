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
#include <stdlib.h>
#include <avr/io.h>
#include <avr_usart.h>
#include <timers16.h>
#include <timers8.h>
#include <macros.h>

void printTimer0(usart term){
   term << "TCCR0A: ";
   term.printBin(TCCR0A);
   term << "\r\nTCCR0B: ";
   term.printBin(TCCR0B);
   term << "\r\nTCNT0: " << TCNT0;
   term << "\r\nOCR0A: " << OCR0A << "\r\nOCR0B: " << OCR0B;
   term << "\r\nTIMSK0: ";
   term.printBin(TIMSK0);
   term << "\r\nTIFR0: ";
   term.printBin(TIFR0);
   term << "\r\n";
}

void printTimer1(usart term){
   term << "TCCR1A: ";
   term.printBin(TCCR1A);
   term << "\r\nTCCR1B: ";
   term.printBin(TCCR1B);
   term << "\r\nTCNT1: " << TCNT1;
   term << "\r\nOCR1A: " << OCR1A << "\r\nOCR1B: " << OCR1B;
   term << "\r\nICR1: " << ICR1;
   term << "\r\nTIMSK1: ";
   term.printBin(TIMSK1);
   term << "\r\nTIFR1: ";
   term.printBin(TIFR1);
   term << "\r\n";
}
   
void printTimer2(usart term){
   term << "TCCR2A: ";
   term.printBin(TCCR2A);
   term << "\r\nTCCR2B: ";
   term.printBin(TCCR2B);
   term << "\r\nTCNT2: " << TCNT2;
   term << "\r\nOCR2A: " << OCR2A << "\r\nOCR2B: " << OCR2B;
   term << "\r\nTIMSK2: ";
   term.printBin(TIMSK2);
   term << "\r\nTIFR2: ";
   term.printBin(TIFR2);
   term << "\r\n";
}

void printTimer16Config(usart term, timer1 tim){
   Timer16Config cfg = tim.getConfig();
   term << "Timer16Config cfg = {\r\n"
        << "              " << cfg.mode << ", // mode\r\n"
        << "              " << cfg.compA << ", // compA\r\n"
        << "              " << cfg.compB << ", // compB\r\n"
        << "              " << cfg.prescaler << ", // prescaler\r\n"
        << "              " << cfg.events << ", // events\r\n"
        << "              " << cfg.onCompareA << ", // onCompareA\r\n"
        << "              " << cfg.onCompareB << ", // onCompareB\r\n"
        << "              " << cfg.inputCapture << ", // inputCapture\r\n"
        << "              " << cfg.inputCfg << " // inputCfg\r\n"
        << "}\r\n";
}

void printTimer16Profile(usart term, timer1 tim){
   Timer16Config cfg = tim.getConfig();
   term << "Timer16Profile prof;\r\nprof.";
   switch (cfg.mode){
   case 0b0000: term << "normal()\r\n"; break;
   case 0b0001: term << "phaseCorrectPWM()\r\n"; break;
   case 0b0010: term << "phaseCorrectPWM9()\r\n"; break;
   case 0b0011: term << "phaseCorrectPWM10()\r\n"; break;
   case 0b0100: term << "ctc(" << cfg.compA << ")\r\n"; break;
   case 0b0101: term << "fastPWM()\r\n"; break;
   case 0b0110: term << "fastPWM9()\r\n"; break;
   case 0b0111: term << "fastPWM10()\r\n"; break;
   case 0b1000: term << "phaseFrequencyPWMicr(" << cfg.inputCapture << ")\r\n"; break;
   case 0b1001: term << "phaseFrequencyPWM(" << cfg.compA << ")\r\n"; break;
   case 0b1010: term << "phaseCorrectPWMicr(" << cfg.inputCapture << ")\r\n"; break;
   case 0b1011: term << "phaseCorrectPWM(" << cfg.compA << ")\r\n"; break;
   case 0b1100: term << "ctcIcr(" << cfg.inputCapture << ")\r\n"; break;
   case 0b1110: term << "fastPWMicr(" << cfg.inputCapture << ")\r\n"; break;
   case 0b1111: term << "fastPWM(" << cfg.compA << ")\r\n"; break;
   default: term << "BAZINGA!\r\n"; break;
   }

   switch (cfg.prescaler){
      case 0: term << "    .prescaler(NO_CLOCK)\r\n"; break;
      case 1: term << "    .prescaler(DIV1)\r\n"; break;
      case 2: term << "    .prescaler(DIV8)\r\n"; break;
      case 3: term << "    .prescaler(DIV64)\r\n"; break;
      case 4: term << "    .prescaler(DIV256)\r\n"; break;
      case 5: term << "    .prescaler(DIV1024)\r\n"; break;
      case 6: term << "    .prescaler(EXT_FALLING)\r\n"; break;
      case 7: term << "    .prescaler(EXT_RISING)\r\n"; break;
      default: term << "BAZINGA!\r\n"; break;
   }

   term << "    .events(";
   if (cfg.events & OVF) {term << "OVF"; cfg.events &= ~(cfg.events & OVF);}
   else if (cfg.events & COMPA) {term << "COMPA"; cfg.events &= ~(cfg.events & COMPA);}
   else if (cfg.events & COMPB) {term << "COMPB"; cfg.events &= ~(cfg.events & COMPB);}
   else if (cfg.events & INCAPT) {term << "INCAPT"; cfg.events &= ~(cfg.events & INCAPT);}
   else term << "0";
   while (cfg.events){
      if (cfg.events & COMPA) {term << " | COMPA"; cfg.events &= ~(cfg.events & COMPA);}
      else if (cfg.events & COMPB) {term << " | COMPB"; cfg.events &= ~(cfg.events & COMPB);}
      else if (cfg.events & INCAPT) {term << " | INCAPT"; cfg.events &= ~(cfg.events & INCAPT);}
   }
   term << ")\r\n";

   term << "    .onCompareA(";
   switch (cfg.onCompareA){
      case COMP_NONE: term << "COMP_NONE)\r\n"; break;
      case COMP_TOGGLE: term << "COMP_TOGGLE)\r\n"; break;
      case COMP_CLEAR: term << "COMP_CLEAR)\r\n"; break;
      case COMP_SET: term << "COMP_SET)\r\n"; break;
      default: term << "BAZINGA!\r\n"; break;
   }
   
   term << "    .onCompareB(";
   switch (cfg.onCompareB){
      case COMP_NONE: term << "COMP_NONE)\r\n"; break;
      case COMP_TOGGLE: term << "COMP_TOGGLE)\r\n"; break;
      case COMP_CLEAR: term << "COMP_CLEAR)\r\n"; break;
      case COMP_SET: term << "COMP_SET)\r\n"; break;
      default: term << "BAZINGA!\r\n"; break;
   }
   
   if (cfg.inputCfg & 0x40) term << "    .inputEdge(INPUT_RISING)\r\n";
      else term << "    .inputEdge(INPUT_FALLING)\r\n";

   term << "    .noiseCanceler(" << ((cfg.inputCfg & 0x80) >> ICNC1) << ")\r\n";

   if (cfg.mode != 0b0100 
    && cfg.mode != 0b1001 
    && cfg.mode != 0b1011 
    && cfg.mode != 0b1111) 
    term << "    .compA(" << cfg.compA << ")\r\n";
   
   term << "    .compB(" << cfg.compB << ");\r\n";
}

void print8cfg(usart term, Timer8Config cfg){
   term << "Timer8Config cfg = {\r\n"
        << "              " << cfg.mode << ", // mode\r\n"
        << "              " << cfg.compA << ", // compA\r\n"
        << "              " << cfg.compB << ", // compB\r\n"
        << "              " << cfg.prescaler << ", // prescaler\r\n"
        << "              " << cfg.events << ", // events\r\n"
        << "              " << cfg.onCompareA << ", // onCompareA\r\n"
        << "              " << cfg.onCompareB << ", // onCompareB\r\n"
        << "}\r\n";
}

void print8profile(usart term, Timer8Config cfg){
   term << "Timer8Profile prof;\r\nprof.";
   switch (cfg.mode){
   case 0b0000: term << "normal()\r\n"; break;
   case 0b0001: term << "phaseCorrectPWM()\r\n"; break;
   case 0b0010: term << "ctc(" << cfg.compA << ")\r\n"; break;
   case 0b0011: term << "fastPWM()\r\n"; break;
   case 0b0101: term << "phaseCorrectPWM(" << cfg.compA << ")\r\n"; break;
   case 0b0111: term << "fastPWM(" << cfg.compA << ")\r\n"; break;
   default: term << "BAZINGA!\r\n"; break;
   }

   term << "    .events(";
   if (cfg.events & OVF) {term << "OVF"; cfg.events &= ~(cfg.events & OVF);}
   else if (cfg.events & COMPA) {term << "COMPA"; cfg.events &= ~(cfg.events & COMPA);}
   else if (cfg.events & COMPB) {term << "COMPB"; cfg.events &= ~(cfg.events & COMPB);}
   else term << "0";
   while (cfg.events){
      if (cfg.events & COMPA) {term << " | COMPA"; cfg.events &= ~(cfg.events & COMPA);}
      else if (cfg.events & COMPB) {term << " | COMPB"; cfg.events &= ~(cfg.events & COMPB);}
   }
   term << ")\r\n";

   term << "    .onCompareA(";
   switch (cfg.onCompareA){
      case COMP_NONE: term << "COMP_NONE)\r\n"; break;
      case COMP_TOGGLE: term << "COMP_TOGGLE)\r\n"; break;
      case COMP_CLEAR: term << "COMP_CLEAR)\r\n"; break;
      case COMP_SET: term << "COMP_SET)\r\n"; break;
      default: term << "BAZINGA!\r\n"; break;
   }
   
   term << "    .onCompareB(";
   switch (cfg.onCompareB){
      case COMP_NONE: term << "COMP_NONE)\r\n"; break;
      case COMP_TOGGLE: term << "COMP_TOGGLE)\r\n"; break;
      case COMP_CLEAR: term << "COMP_CLEAR)\r\n"; break;
      case COMP_SET: term << "COMP_SET)\r\n"; break;
      default: term << "BAZINGA!\r\n"; break;
   }
   
   if (cfg.mode != 0b0010 
    && cfg.mode != 0b0101 
    && cfg.mode != 0b0111) 
    term << "    .compA(" << cfg.compA << ")\r\n";
   
   term << "    .compB(" << cfg.compB << ");\r\n";
}

void printT0prescaler(usart term, Timer8Config cfg){
   switch (cfg.prescaler){
      case 0: term << "    .prescaler(NO_CLOCK);\r\n"; break;
      case 1: term << "    .prescaler(DIV1);\r\n"; break;
      case 2: term << "    .prescaler(DIV8);\r\n"; break;
      case 3: term << "    .prescaler(DIV64);\r\n"; break;
      case 4: term << "    .prescaler(DIV256);\r\n"; break;
      case 5: term << "    .prescaler(DIV1024);\r\n"; break;
      case 6: term << "    .prescaler(EXT_FALLING);\r\n"; break;
      case 7: term << "    .prescaler(EXT_RISING);\r\n"; break;
      default: term << "BAZINGA!\r\n"; break;
   }
}

void printT2prescaler(usart term, Timer8Config cfg){
   switch (cfg.prescaler){
      case 0: term << "    .prescaler(NO_CLOCK);\r\n"; break;
      case 1: term << "    .prescaler(DIV1);\r\n"; break;
      case 2: term << "    .prescaler(DIV8);\r\n"; break;
      case 3: term << "    .prescaler(DIV32);\r\n"; break;
      case 4: term << "    .prescaler(DIV64);\r\n"; break;
      case 5: term << "    .prescaler(DIV128);\r\n"; break;
      case 6: term << "    .prescaler(DIV256);\r\n"; break;
      case 7: term << "    .prescaler(DIV1024);\r\n"; break;
      default: term << "BAZINGA!\r\n"; break;
   }
}

void printTimer8Config(usart term, timer0 tim){
   Timer8Config cfg = tim.getConfig();
   print8cfg(term, cfg);
}

void printTimer8Config(usart term, timer2 tim){
   Timer8Config cfg = tim.getConfig();
   print8cfg(term, cfg);
}

void printTimer8Profile(usart term, timer0 tim){
   Timer8Config cfg = tim.getConfig();
   print8profile(term, cfg);
   printT0prescaler(term, cfg);
}

void printTimer8Profile(usart term, timer2 tim){
   Timer8Config cfg = tim.getConfig();
   print8profile(term, cfg);
   printT2prescaler(term, cfg);
}



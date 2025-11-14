/*  ISR(TIMER0_OVF_vect) MUST be commented off in wiring.c for this to run on timer0 
 *
 *  Another option: add
 *  ISR(TIMER0_OVF_vect) __attribute__ ((weak));
 *  line right before 
 *  ISR(TIMER0_OVF_vect)
 *  in wiring.c.
 *
 *  The best option: make your own main(), thank you chatGPT.
 */


#include <avr_timers.h>
#include <avr_gpio.h>
#include <avr_usart.h>

uint8_t k;
uint16_t b;
uint8_t light;

timer0 test(PWM_FAST, CLOCK_PS256);
digitalPin led(&PORTB, 5, OUTPUT);
usart term; // used for demo code in loop()
uint8_t blink_speed = 4; //0 off, 1 on, 2..5 more is slower

// lookup table for a sin wave
const uint8_t lights[360]={
  0,   0,   0,   0,   0,   1,   1,   2, //8
  2,   3,   4,   5,   6,   7,   8,   9, //16
 11,  12,  13,  15,  17,  18,  20,  22, 
 24,  26,  28,  30,  32,  35,  37,  39, //32
 42,  44,  47,  49,  52,  55,  58,  60, 
 63,  66,  69,  72,  75,  78,  81,  85, 
 88,  91,  94,  97, 101, 104, 107, 111, 
114, 117, 121, 124, 127, 131, 134, 137, //64
141, 144, 147, 150, 154, 157, 160, 163, 
167, 170, 173, 176, 179, 182, 185, 188, 
191, 194, 197, 200, 202, 205, 208, 210, 
213, 215, 217, 220, 222, 224, 226, 229, 
231, 232, 234, 236, 238, 239, 241, 242, 
244, 245, 246, 248, 249, 250, 251, 251, 
252, 253, 253, 254, 254, 255, 255, 255, 
255, 255, 255, 255, 254, 254, 253, 253, //128
252, 251, 251, 250, 249, 248, 246, 245, 
244, 242, 241, 239, 238, 236, 234, 232, 
231, 229, 226, 224, 222, 220, 217, 215, 
213, 210, 208, 205, 202, 200, 197, 194, 
191, 188, 185, 182, 179, 176, 173, 170, 
167, 163, 160, 157, 154, 150, 147, 144, 
141, 137, 134, 131, 127, 124, 121, 117, 
114, 111, 107, 104, 101,  97,  94,  91, //192
 88,  85,  81,  78,  75,  72,  69,  66, 
 63,  60,  58,  55,  52,  49,  47,  44, 
 42,  39,  37,  35,  32,  30,  28,  26, 
 24,  22,  20,  18,  17,  15,  13,  12, //224
 11,   9,   8,   7,   6,   5,   4,   3, 
  2,   2,   1,   1,   0,   0,   0,   0, //240
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, //256
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, //288
  0,   0,   0,   0,   0,   0,   0,   0, //296
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, //320
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, //352
  0,   0,   0,   0,   0,   0,   0,   0};

ISR(TIMER0_OVF_vect)
{
  if (OCR0B) led.high();
  test.writeB(lights[++light]);
}

ISR(TIMER0_COMPB_vect)
{
  led.low();
}

void setup() {
  // put your setup code here, to run once:
test.setChannelA(CM_CLEAR);
test.setInterrupts(OVF | COMPB);
sei(); // if init is commented out in main, interrupts must be enabled here
term.init(9600);
test.setPrescaler(blink_speed);
test.enable();
}

void loop() {
  // put your main code here, to run repeatedly:
  // the following is just an example used to change the 'breathing speed' via Serial.
  // can be replaced with useful code

term << "Change speed: " >> blink_speed << "\r\nSpeed set to " << blink_speed%6 << "\r\n";
test.setPrescaler(blink_speed%6);
test.enable();
}


// get rid of wiring.c
int main() {
  // init();  // Do NOT call the standard Arduino initialization function while using Timer0

  setup();

  while (true) {
    loop();
  }

  return 0;
}

#include <avr_gpio.h>

digitalPin led(&PORTB, 5, OUTPUT);

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  led = !led;
  _delay_ms(300);

}

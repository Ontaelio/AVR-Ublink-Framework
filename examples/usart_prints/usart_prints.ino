#include <avr_usart.h>
#include <avr_gpio.h>

// 3232 without main
// 2930 with main

usart term;
double a, b;
digitalPin led(&PORTB, 5, OUTPUT);

void setup() {
  // put your setup code here, to run once:
  term.init(9600); //setup baud rate
  term << "Starting...\r\n";
}

void loop() {
  // put your main code here, to run repeatedly:
    term << "Enter two numbers: " >> a << a << " : ">> b << b <<"\r\n";
    term    <<"entered: "<< a << " and "<< b <<"\r\n";
    term    <<"The sum is: " << a + b << "\r\n";
    term << "And the long version is: ";
    term.printFloat(a+b, 10); //print the double with 10 decimals
    term << "\r\n";
    led = (int(a+b))%2;
    // term << (int(a+b))%2;
}


int mainsss() {
  // init();  // Call the standard Arduino initialization function

  setup();

  while (true) {
    loop();
    // delay(1);
  }

  return 0;
}

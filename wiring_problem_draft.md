# The wiring.c problem

You may not know it, but Arduino IDE adds wiring.c to your code while compiling the latter. This is being done to make your life easier, but that is true only if you are new to MC programming. At some point this becomes a problem, as it is really annoying when some unknown code gets added to yours without your consent nor knowledge.

I'm sure Arduino team is also pretty much annoyed by this, as `wiring.c` is like 20 years old, but they can do nothing about it. A lot of already written code relies on this file. It sets up timers and provides timing-related functions like `delay()` and `millis()`. Thing is, by doing so it expropriates Timer0; you can not use it anymore. Also, all this setting of timers, not needed in most cases, takes up program space. So: how can we rid of it, considering that the file itself is hard-coded into the IDE and lies hidden deep somewhere in the file system?

Note: unless you really need all three timers, you do not need to do anything about it. Just use timers 1 and 2. But if you do need all three, or you need to squeeze these extra 200 bytes of code into program memory, go on.

There are three ways. The first two are not so good as they require changing Arduino system files, which means if you share your code, the recepient must do the same. In any case, here we go:

1) NOT RECOMMENDED. Find `wiring.c` itself. In Windows, it lies in `user\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6\cores\arduino` or something similar, even if you are using portable version of Arduino IDE. In other OS's the path is different, try to follow the obscure logic. In any case, this `wiring.c` contains the following line:

`ISR(TIMER0_OVF_vect)`

Comment it out. 

The problem of this approach is your *other* sketches that use `delay` or `millis` won't run, as `wiring.c` is shared between all your code.

2) A BIT BETTER. Find the same line and add this one right before it:

`ISR(TIMER0_OVF_vect) __attribute__ ((weak));`

This way you will be able to write your own implementation for Timer0 overflow interrupt, but if you do not, it will use the default one. Meaning all your other code with `delays` will still work.

3) THE BEST. Program your sketch as a normal program, that is, add

```
int main() {

  setup();

  while (true) {
    loop();
  }

  return 0;
}
```

to it. This way `wiring.c` won't be loaded, **unless** you use any Arduino framework functions, be it `delay` or `digitalWrite`. Or any library that uses them. 

Thus, this approach works only if you do not intend to use any, else either use the previous solution or forget about Timer0. Sad, but this is life.
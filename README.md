# avr-sound

Really simple sound drivers to AVR / Arduino platform written in C.
Audio out only. Amplifier may be needed depending on your needs.

# Requirements
- AtMega328P-PU or similar, or Arduino (Uno) Board
- R-2R resistor ladder
    - For example, 8x 20kOhm and 8x 10kOhm resistors for 8-bit sound, but you can do 1-bit as well; edit header
- some speakers or headphones or scope to test. 
I'm using headphones and 3.5mm audio jack connector with jumper wires soldered in.

This library gives you a fully customizable audio out buffer and some simple functions to handle it.
I am using this on my project (/filbe/synth/) to generate 8-bit sound. Feel free to comment and contribute if you want to.
I have left out any sound generating functions in the library on purpose due to the well known limitations of avr's memory as well as ease to create one, as shown in the example.

# Installing dependencies, cloning project and burning example 
```
sudo apt-get install gcc-avr avr-libc
sudo apt-get install avrdude
git clone https://github.com/filbe/avr-sound.git
make fuses && make flash
```

Feel free to comment and contribute if you have better ideas or solutions to the same problem.

[Listen to recorded example audio](https://github.com/filbe/avr-sound/blob/main/examples/example1.mp3?raw=true)

![Example 1 pic 1](https://github.com/filbe/avr-sound/blob/main/examples/example1_1.jpg?raw=true)
![Example 1 pic 2](https://github.com/filbe/avr-sound/blob/main/examples/example1_2.jpg?raw=true)
![Example 1 pic 3](https://github.com/filbe/avr-sound/blob/main/examples/example1_3.jpg?raw=true)

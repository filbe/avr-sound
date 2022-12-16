# universal-synth

Really simple cross platform audio synth written in C...

...or at least attempt to do that.

The main goal is to do scalable synth tool that you can work with on your Linux PC, and then reduce specs to run neat songs / audio stuff on your smallest microcontroller unit (MCU) with 2kb of RAM @ 16MHz 8-bit CPU, and everything sounds still pretty okay.

This synth is under construction (and that's the case forever?) and this README doesn't guarantee if it works at all, but it is a mix of estimated goals and/or what it is today, so feel free to test and smash your head to the concrete wall (no injuries will be compensated in any way!).

# Minimum Device Requirements on non-buffered MONO mode

(meant for real-time computing, eg. MCU / FreeRTOS etc)

- 16MHz MCU / CPU, output samplerate will be 20kHz maximum\*
- 2kb of RAM
- 16kb of persistent storage compatible with your device
- Timer interrupts at your selected samplerate

# Minimum Device Requirements on buffered STEREO mode

(meant for PC's and high-end MCU's, but runs OK on mid-/low-end MCU's with reduced specs)

- 200MHz MCU / CPU, output samplerate will be 44.1kHz maximum\* (estimate)
- 4kb of RAM + buffer length worth of RAM (estimate)
- 16kb of persistent storage compatible with your device (estimate)
- Timer interrupts at your selected samplerate and in-built buffer OR
- External buffer-managing library that runs on another thread with sample reading callback functionality

\*increasing clock speed will allow increasing maximum samplerate that is not limited in any way!

# Installing dependencies, cloning project and burning example

```
sudo apt install -y libsndfile1-dev portaudio19-dev
git clone https://github.com/filbe/avr-sound.git
cd avr-sound/src
git checkout x64
make example
```

Feel free to comment and contribute if you have better ideas or solutions to the same problem.

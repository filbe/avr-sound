/*
 * AVR Sound library
 * avr-sound-example.c
 *
 * Simple waveform generation example. You will need a R-R2 Resistor ladder network to get n-bit sound.
 * Tested on AtMega328P-PU with bitrates of 1kHz and 40kHz @ 16MHz external crystal oscillator
 * If you want to use this with internal 8MHz oscillator, remember to change fuses in Makefile.
 * Fuses works straight on Arduino Uno board, but feel free to test it standalone as well (with custom fuses).
 *
 * Authors:
 *  Ville-Pekka Lahti <ville-pekka.lahti@hotmail.com>
 */

#ifndef __SOUND_LINUX_H__
#define __SOUND_LINUX_H__

#include <stdint.h>
#include <string.h>
#define SOUND_PORT 				PORTD
#define SOUND_DDR				DDRD
#define SOUND_BITS				8
#define SOUND_PINS_IN_PORT		8
#define SOUND_BIG_ENDIAN			1
#define SOUND_LITTLE_ENDIAN		0
#define SOUND_ENDIANESS 			SOUND_BIG_ENDIAN
#define SOUND_PINMASK_BE			(1 << SOUND_BITS) - 1
#define SOUND_PINMASK_LE			(~SOUND_PINMASK_BE) >> (SOUND_PINS_IN_PORT - SOUND_BITS)

#define SOUND_DEFAULT_BITRATE 44100
#define SOUND_MAXIMUM_SAMPLE_LENGTH 256
#define SOUND_MAX_CHANNELS       3
#define SOUND_ADRS_ENABLED       1

extern volatile uint64_t sound_time;

void sound_init();
void sound_sample_init(uint16_t sample_len, float hz);
void sound_setbuffer(uint8_t waveform, uint16_t index, int8_t value);
void sound_set_hz(uint8_t channel, float hz);
void sound_set_waveform(uint8_t channel, uint8_t waveform);
void sound_finetune(uint16_t tune);
void sound_set_volume(uint8_t channel, uint8_t volume);
void sound_set_pan(uint8_t channel, int8_t pan);
void sound_set_adsr(uint8_t channel, uint16_t attack, uint16_t decay, uint8_t sustain, uint16_t release);
volatile uint8_t sound_process_one_sample(int8_t pan);
volatile int sound_fill_buffer();


#endif /* __SOUND_LINUX_H__*/
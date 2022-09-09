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

 #ifndef __AVRSOUND_H__
#define __AVRSOUND_H__

#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>
#define AVRSOUND_PORT 				PORTD
#define AVRSOUND_DDR				DDRD
#define AVRSOUND_BITS				8
#define AVRSOUND_PINS_IN_PORT		8
#define AVRSOUND_BIG_ENDIAN			1
#define AVRSOUND_LITTLE_ENDIAN		0
#define AVRSOUND_ENDIANESS 			AVRSOUND_BIG_ENDIAN
#define AVRSOUND_PINMASK_BE			(1 << AVRSOUND_BITS) - 1
#define AVRSOUND_PINMASK_LE			(~AVRSOUND_PINMASK_BE) >> (AVRSOUND_PINS_IN_PORT - AVRSOUND_BITS)
/*

*/
#define AVRSOUND_BITRATE			25000
#define AVRSOUND_PCM_SPEED_SCALE 	8
#define AVRSOUND_MAXIMUM_SAMPLE_LENGTH 128
#define AVRSOUND_MAX_CHANNELS       8

#ifndef F_CPU
#error "F_CPU is missing!"
#endif

void avrsound_init();
void avrsound_sample_init(uint16_t sample_len, float hz);
void avrsound_setbuffer(uint8_t waveform, uint16_t index, int8_t value);
void avrsound_set_hz(uint8_t channel, float hz);
void avrsound_set_waveform(uint8_t channel, uint8_t waveform);
void avrsound_finetune(uint16_t tune);


volatile uint32_t time;
volatile uint8_t current_waveform[AVRSOUND_MAX_CHANNELS];

#endif
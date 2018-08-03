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

#include "avr-sound.h"

#define MAX_CHANNELS 3

volatile uint16_t avrsound_buffercursor[MAX_CHANNELS];
volatile float avrsound_buffer_jump = 1 >> 8;
volatile uint16_t avrsound_buffer_speed[MAX_CHANNELS];
volatile float avrsound_buffer_hz = 440.0;
volatile uint16_t avrsound_buffer_len = 256;
volatile int16_t avrsound_buffer[AVRSOUND_MAXIMUM_SAMPLE_LENGTH];

void avrsound_init() {

	if (AVRSOUND_ENDIANESS == AVRSOUND_BIG_ENDIAN) {
		AVRSOUND_DDR |= AVRSOUND_PINMASK_BE;
	} else {
		AVRSOUND_DDR |= AVRSOUND_PINMASK_LE;
	}

	TCCR1B |= (1 << WGM13) | (1 << WGM12);
	ICR1 = F_CPU / AVRSOUND_BITRATE / AVRSOUND_PCM_SPEED_SCALE - 1;
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << CS11);
}

void avrsound_sample_init(uint16_t sample_len, float hz) {
	// Forcing maximum sample length if trying bigger
	if (sample_len > AVRSOUND_MAXIMUM_SAMPLE_LENGTH) {
		sample_len = AVRSOUND_MAXIMUM_SAMPLE_LENGTH;
	}
	avrsound_buffer_len = sample_len;

	//avrsound_buffer_jump = hz*1080.17/sample_len;
	avrsound_buffer_hz = hz;
}

void avrsound_setbuffer(uint16_t index, uint8_t value) {
	avrsound_buffer[index] = value;
}

void avrsound_set_hz(uint8_t channel, float hz) {
	avrsound_buffer_speed[channel] = 256.0 * 256.0 * avrsound_buffer_len * hz / (AVRSOUND_BITRATE * avrsound_buffer_hz);
}


ISR (TIMER1_COMPA_vect) 
{
	int16_t bufsum = 0;
	for (uint8_t i=0;i<MAX_CHANNELS;i++) {
		switch(i) {
			case 0:
			bufsum += avrsound_buffer[(avrsound_buffercursor[i] >> 8)];
			break;
			case 1:
			bufsum += avrsound_buffer[(avrsound_buffercursor[i] >> 8)];
			break;
			case 2:
			bufsum += avrsound_buffer[(avrsound_buffercursor[i] >> 8)] >> 1;
			break;	
		}
		avrsound_buffercursor[i] += avrsound_buffer_speed[i];
	}
	bufsum = (bufsum / 2)-127;
	if (bufsum > 127)
		bufsum=127;
	if (bufsum < -128)
		bufsum=0;
	AVRSOUND_PORT = bufsum-128;
	
}
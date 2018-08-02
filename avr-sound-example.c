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

#include <math.h>
#include <util/delay.h>
#include "avr-sound.h"

uint8_t song[] = {
	48, 48, 60, 60, 48, 48, 60, 60,
	43, 43, 55, 55, 43, 41, 39, 38,
	36, 36, 48, 48, 36, 36, 48, 48,
	43, 43, 43, 43, 43, 41, 39, 38,
};
uint8_t song2[] = {
	75, 77, 75, 74, 72, 75, 74, 72,
	71, 72, 74, 71, 67, 69, 71, 67,
	72, 71, 72, 74, 75, 74, 75, 77, 
	79, 78, 79, 78, 79, 80, 79, 77,
};


uint8_t song3[] = {
	87, 94, 99, 94, 87, 94, 99, 94,
	82, 89, 94, 89, 82, 89, 94, 89,
	87, 94, 99, 94, 87, 94, 99, 94,
	82, 89, 94, 89, 82, 89, 94, 89,
};

float midi[100];
int main() 
{
	for (float i=0;i<132;i+=1.0) {
		midi[(uint8_t)(i)] = pow(2.0, (i-69.0)*0.083333)*440.0;
	}
	sei();
	avrsound_init();
	
	avrsound_sample_init(256, 440.0);

	for (uint16_t b=0;b<256;b++) {
		avrsound_setbuffer(b, (uint8_t)(sin(b/256.0*M_PI)*127.5+127.5));
	}

	uint8_t c = 0;



	while(1) {
		avrsound_set_hz(0, midi[song[c % sizeof(song)]]);
		avrsound_set_hz(1, midi[song2[c % sizeof(song2)]]);
		avrsound_set_hz(2, midi[song3[c % sizeof(song3)]-3]);
		c++;
		_delay_ms(85);
	}
	return 0;
}
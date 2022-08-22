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
	64,65,65,64,64,62,69,69,
	69,69,69,69,69,69,67,65,
	67,67,67,72,72,67,76,76,
	76,76,76,76,76,76,74,76,

	76,77,77,76,76,74,69,69,
	69,69,69,69,69,69,67,65,
	67,67,67,67,67,67,74,76,
	77,76,74,69,67,65,64,60,
};
uint8_t song2[] = {
	50,0,0,50,0,50,50,0,
	0,0,50,50,0,50,50,0,
	48,0,0,48,0,48,48,0,
	0,0,48,48,0,48,48,0,

	46,0,0,46,0,46,46,0,
	0,0,46,46,0,46,46,0,
	48,0,0,48,0,48,48,0,
	60,0,57,0,55,0,48,0,
};

uint8_t song3[] = {
	86,88,89,93,86,88,89,93,
	86,88,89,93,86,88,89,93,
	84,86,88,91,84,86,88,91,
	84,86,88,91,84,86,88,91,

	82,84,86,89,82,84,86,89,
	82,84,86,89,82,84,86,89,
	84,86,88,91,84,86,88,91,
	84,86,88,91,84,86,88,91,
};

float midi[110];

void dummyDelay(uint32_t val) {
	while(((time) >> 7) % val != 0) {};
	while(((time) >> 7) % val == 0) {};
}

uint16_t samlen = AVRSOUND_MAXIMUM_SAMPLE_LENGTH;
int main() 
{
	for (float i=0;i<110;i+=1.0) {
		midi[(uint8_t)(i)] = pow(2.0, (i-69.0)*0.083333)*440.0;
	}

	avrsound_init();
	
	avrsound_sample_init(samlen, 440.0);

	for (int16_t b=0;b<samlen;b++) {
		// Buffer range is -128....127
		
		
		avrsound_setbuffer(0, b, 
		sin(b/(float)(samlen)*M_PI)*30.0 + 
		sin(2*b/(float)(samlen)*M_PI)*32.0 + 
		sin(4*b/(float)(samlen)*M_PI)*20.0 + 
		sin(8*b/(float)(samlen)*M_PI)*15.0 + 
		sin(16*b/(float)(samlen)*M_PI)*13.0
		); // organ


		avrsound_setbuffer(2, b, 
		sin(b/(float)(samlen)*M_PI)*127
		
		); // organ
		
		
		avrsound_setbuffer(1, b, (b % samlen) - samlen / 2); // SAWTOOTH
		//avrsound_setbuffer(2, b, b < samlen / 2 ? -128 : 127); // SQUARE WAVE
	}

	uint8_t c = 0;
	//avrsound_set_hz(0, 500.0);

	avrsound_set_hz(0,0);
	avrsound_set_hz(1,0);
	avrsound_set_hz(2,0);

	avrsound_set_waveform(0,0);
	avrsound_set_waveform(1,1);
	avrsound_set_waveform(2,2);

	while(1) {
		avrsound_set_hz(0, midi[song[c % sizeof(song)]]);
		avrsound_set_hz(1, midi[song2[c % sizeof(song2)]]);
		avrsound_set_hz(2, midi[song3[c % sizeof(song3)]]);
		c++;

		dummyDelay(24);
		

	}
	return 0;
}
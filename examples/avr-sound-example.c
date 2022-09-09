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
	64, 0, 0, 0, 64, 0, 67, 0, 0, 0, 67,0, 71,0,  0,0,  71,0,  71,0,  0,0,  71,0,  76,0,  0,0,  76,0, 0, 0,  
};

uint8_t song2[] = {
	0, 0, 64, 0, 0, 0, 64, 0, 67, 0, 0, 0, 67,0, 71,0,  0,0,  71,0,  71,0,  0,0,  71,0,  76,0,  0,0,  76,0,  
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
		midi[(uint8_t)(i)] = pow(2.0, (i-12.0-69.0)*0.083333)*440.0;
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
			sin(b*4/(float)(samlen)*M_PI)*127
		
		); // sin
		
		avrsound_setbuffer(1, b, b); // SAWTOOTH
		//avrsound_setbuffer(2, b, b < samlen / 2 ? -128 : 127); // SQUARE WAVE
	}

	uint32_t c = 0;
	avrsound_set_waveform(0,2);
	//avrsound_set_waveform(1,2);


	avrsound_set_volume(0,255);
	//avrsound_set_volume(1,255);

	DDRB = 255;

	avrsound_set_adsr(0, 200, 300,90,4000);
	
	while(1) {
		avrsound_set_hz(0, midi[song[c % sizeof(song)]]);
		//avrsound_set_hz(1, midi[song2[c % sizeof(song2)]]);
		avrsound_set_adsr(0, (uint16_t)(c * 12) % 4096, ((uint16_t)(c * 17)) % 2345,90,200 + (uint16_t)(c * 20) % 4000);
		c++;

		dummyDelay(7);


		

	}
	return 0;
}
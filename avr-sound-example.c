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

int main() 
{
	avrsound_init();
	sei();
	avrsound_set_pcm_length(1024);
	float a=0;
	avrsound_buffer_clean();
	float c = 0;
	DDRB = 0;
	while(1) {
		a=0;
		for (uint16_t b=0;b<1024;b++) {
			avrsound_buffer_write_ring((sin(a)*127.5+127.5));
			a+=0.1;
		}
		c = c + 3.1;
		_delay_ms(50);
	}
	return 0;
}
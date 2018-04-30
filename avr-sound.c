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

volatile static uint8_t avrsound_buffer[AVRSOUND_BUFFERSIZE];
volatile static uint16_t avrsound_buffer_cursor = 0;
volatile static uint16_t avrsound_pcm_length = AVRSOUND_BUFFERSIZE << AVRSOUND_PCM_SPEED_SCALE;
volatile static uint16_t avrsound_pcm_length_256 = 0;
volatile static uint16_t avrsound_pcm_speed = (1 << AVRSOUND_PCM_SPEED_SCALE);
volatile static uint16_t avrsound_write_cursor = 0;

void avrsound_init()
{
	avrsound_pcm_length_256 = avrsound_pcm_length << AVRSOUND_PCM_SPEED_SCALE;
	AVRSOUND_DDR |= 0xff; // enable output
	if (AVRSOUND_ENDIANESS == AVRSOUND_BIG_ENDIAN) {
		AVRSOUND_DDR |= AVRSOUND_PINMASK_BE;
	} else if (AVRSOUND_ENDIANESS == AVRSOUND_LITTLE_ENDIAN) {
		AVRSOUND_DDR |= AVRSOUND_PINMASK_LE;
	}
	TCCR0A |= (1 << WGM01); // CTC-mode
	OCR0A = F_CPU / (AVRSOUND_BITRATE) / 8 - 1;
	TIMSK0 |= (1 << OCIE0A);
	TCCR0B |= (1 << CS01);
}

void avrsound_set_pcm_length(uint16_t length)
{
	avrsound_pcm_length = length;
	avrsound_pcm_length_256 = avrsound_pcm_length << AVRSOUND_PCM_SPEED_SCALE;
}

void avrsound_buffer_write_byte_at(uint8_t pcm_sample, uint16_t offset)
{
	avrsound_buffer[offset] = pcm_sample;
}

void avrsound_buffer_write_ring(uint8_t pcm_sample)
{
	avrsound_buffer[avrsound_write_cursor] = pcm_sample;
	avrsound_write_cursor++;
	if (avrsound_write_cursor > avrsound_pcm_length)
		avrsound_write_cursor -= avrsound_pcm_length;
}

void avrsound_buffer_set_ring_position(uint16_t position)
{
	avrsound_write_cursor = position;
}

void avrsound_buffer_write(uint8_t * pcm_in, uint16_t length, uint16_t offset)
{
	memcpy(&avrsound_buffer+offset, pcm_in, length);
}

void avrsound_buffer_write_sync(uint8_t * pcm_in, uint16_t length, uint16_t offset)
{
	cli();
	memcpy(&avrsound_buffer+offset, pcm_in, length);
	sei();
}

void avrsound_buffer_clean()
{
	memset(&avrsound_buffer, 0, sizeof(avrsound_buffer));
}

ISR (TIMER0_COMPA_vect) 
{
	AVRSOUND_PORT = avrsound_buffer[avrsound_buffer_cursor >> AVRSOUND_PCM_SPEED_SCALE];
	avrsound_buffer_cursor += avrsound_pcm_speed;
	if (avrsound_buffer_cursor > avrsound_pcm_length_256)
		avrsound_buffer_cursor -= avrsound_pcm_length_256;


}
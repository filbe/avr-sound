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
#define AVRSOUND_BITRATE			2000
#define AVRSOUND_BUFFERSIZE			1024
#define AVRSOUND_PCM_SPEED_SCALE 	8

#ifndef F_CPU
#error "F_CPU is missing!"
#endif

void avrsound_init();
void avrsound_set_pcm_length(uint16_t length);
void avrsound_buffer_write_byte_at(uint8_t pcm_sample, uint16_t offset);
void avrsound_buffer_write_ring(uint8_t pcm_sample);
void avrsound_buffer_set_ring_position(uint16_t position);
void avrsound_buffer_write(uint8_t * pcm_in, uint16_t length, uint16_t offset);
void avrsound_buffer_write_sync(uint8_t * pcm_in, uint16_t length, uint16_t offset);
void avrsound_buffer_clean();

#endif
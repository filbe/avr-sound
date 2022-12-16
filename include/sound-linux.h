/*
 * Universal Synth
 * Authors:
 *  Ville-Pekka Lahti <ville-pekka.lahti@hotmail.com>
 */

#ifndef __SOUND_LINUX_H__
#define __SOUND_LINUX_H__

#include "sound-init.h"
#include <stdint.h>
#include <string.h>
#define SOUND_PORT PORTD
#define SOUND_DDR DDRD
#define SOUND_BITS 8
#define SOUND_PINS_IN_PORT 8
#define SOUND_BIG_ENDIAN 1
#define SOUND_LITTLE_ENDIAN 0
#define SOUND_ENDIANESS SOUND_BIG_ENDIAN
#define SOUND_PINMASK_BE (1 << SOUND_BITS) - 1
#define SOUND_PINMASK_LE                                                       \
  (~SOUND_PINMASK_BE) >> (SOUND_PINS_IN_PORT - SOUND_BITS)

#define SOUND_DEFAULT_BITRATE 44100
#define SOUND_MAXIMUM_SAMPLE_LENGTH 256
#define SOUND_MAX_CHANNELS 3
#define SOUND_ADRS_ENABLED 1

#define FRAMES_PER_BUFFER (256)

#define SOUND_BITRATE SOUND_DEFAULT_BITRATE
#define DELAY_BUFFER_LENGTH 10000

#define ADSR_ATTACKING 0
#define ADSR_DECAYING 1
#define ADSR_SUSTAINING 2
#define ADSR_RELEASING 3
#define ADSR_OFF 4

void sound_sample_init(uint16_t sample_len, float hz);
void sound_setbuffer(uint8_t waveform, uint16_t index, int8_t value);
void sound_set_hz(uint8_t channel, float hz);
void sound_set_waveform(uint8_t channel, uint8_t waveform);
void sound_finetune(uint16_t tune);
void sound_set_volume(uint8_t channel, uint8_t volume);
void sound_set_pan(uint8_t channel, int8_t pan);
void sound_set_adsr(uint8_t channel, uint16_t attack, uint16_t decay,
                    uint8_t sustain, uint16_t release);
volatile uint8_t sound_process_one_sample(int8_t pan);
extern volatile int sound_fill_buffer();

extern volatile uint16_t sound_buffercursor[SOUND_MAX_CHANNELS];
extern volatile float sound_buffer_jump;
extern volatile uint16_t sound_buffer_speed[SOUND_MAX_CHANNELS];

extern volatile uint8_t sound_buffer_volume[SOUND_MAX_CHANNELS];
extern volatile uint8_t _sound_buffer_volume[SOUND_MAX_CHANNELS];
extern volatile int8_t sound_buffer_pan[SOUND_MAX_CHANNELS];
extern volatile uint32_t sound_channel_start_time[SOUND_MAX_CHANNELS];
extern volatile uint32_t sound_channel_release_time[SOUND_MAX_CHANNELS];
extern volatile uint8_t sound_channel_adsr_state[SOUND_MAX_CHANNELS];

extern volatile uint16_t sound_channel_adsr_attack[SOUND_MAX_CHANNELS];
extern volatile uint16_t sound_channel_adsr_decay[SOUND_MAX_CHANNELS];
extern volatile uint8_t sound_channel_adsr_sustain[SOUND_MAX_CHANNELS];
extern volatile uint16_t sound_channel_adsr_release[SOUND_MAX_CHANNELS];
extern volatile uint32_t sound_channel_adsr_attack_flip[SOUND_MAX_CHANNELS];
extern volatile uint32_t sound_channel_adsr_decay_flip[SOUND_MAX_CHANNELS];
extern volatile uint32_t sound_channel_adsr_release_flip[SOUND_MAX_CHANNELS];

extern volatile uint16_t sound_buffer_volume_sum;

extern volatile float sound_buffer_hz;
extern volatile uint16_t sound_buffer_len;
extern volatile int8_t sound_buffer[SOUND_MAX_CHANNELS]
                                   [SOUND_MAXIMUM_SAMPLE_LENGTH];
extern volatile float finetune;

extern volatile uint64_t sound_time;
extern volatile uint8_t current_waveform[SOUND_MAX_CHANNELS];

extern volatile int8_t delay_buffer[2][DELAY_BUFFER_LENGTH];

#endif /* __SOUND_LINUX_H__*/
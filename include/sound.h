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

#define FRAMES_PER_BUFFER (64)

#define SOUND_BITRATE SOUND_DEFAULT_BITRATE
#define DELAY_BUFFER_LENGTH 25000

#define ADSR_ATTACKING 0
#define ADSR_DECAYING 1
#define ADSR_SUSTAINING 2
#define ADSR_RELEASING 3
#define ADSR_OFF 4

#define SOUND_BUFFER_LENGTH 1024

void sound_sample_init(uint16_t sample_len, float hz);
void sound_setbuffer(uint8_t waveform, uint16_t index, int8_t value);
void sound_channel_set_hz(uint8_t channel, float hz);
void sound_set_waveform(uint8_t channel, uint8_t waveform);
void sound_finetune(uint16_t tune);
void sound_channel_set_volume(uint8_t channel, uint8_t volume);
void sound_channel_set_pan(uint8_t channel, int8_t pan);
void sound_set_adsr(uint8_t channel, uint16_t attack, uint16_t decay,
                    uint8_t sustain, uint16_t release);
int16_t sound_process_one_sample(int8_t pan);
extern int sound_fill_buffer();

extern uint16_t sound_buffercursor[SOUND_MAX_CHANNELS];
extern float sound_buffer_jump;
extern uint16_t sound_buffer_speed[SOUND_MAX_CHANNELS];

extern uint8_t sound_buffer_volume[SOUND_MAX_CHANNELS];
extern uint8_t _sound_buffer_volume[SOUND_MAX_CHANNELS];
extern int8_t sound_buffer_pan[SOUND_MAX_CHANNELS];
extern uint32_t sound_adsr_channel_start_time[SOUND_MAX_CHANNELS];
extern uint32_t sound_adsr_channel_release_time[SOUND_MAX_CHANNELS];
extern uint8_t sound_adsr_channel_state[SOUND_MAX_CHANNELS];

extern uint16_t sound_adsr_channel_attack[SOUND_MAX_CHANNELS];
extern uint16_t sound_adsr_channel_decay[SOUND_MAX_CHANNELS];
extern uint8_t sound_adsr_channel_sustain[SOUND_MAX_CHANNELS];
extern uint16_t sound_adsr_channel_release[SOUND_MAX_CHANNELS];
extern uint32_t sound_adsr_channel_attack_flip[SOUND_MAX_CHANNELS];
extern uint32_t sound_adsr_channel_decay_flip[SOUND_MAX_CHANNELS];
extern uint32_t sound_adsr_channel_release_flip[SOUND_MAX_CHANNELS];
uint16_t adsr_volume(uint8_t channel);

extern uint16_t sound_buffer_volume_sum;

extern float sound_buffer_hz;
extern uint16_t sound_buffer_len;
extern int16_t sound_buffer[SOUND_MAX_CHANNELS][SOUND_MAXIMUM_SAMPLE_LENGTH];
extern float finetune;

extern uint64_t sound_time;
extern int16_t current_waveform[SOUND_MAX_CHANNELS];

extern int16_t delay_buffer[2][DELAY_BUFFER_LENGTH];
extern float sound_delay_factor;

#endif /* __SOUND_LINUX_H__*/
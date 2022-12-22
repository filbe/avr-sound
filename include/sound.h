/*
 * Universal Synth
 * Authors:
 *  Ville-Pekka Lahti <ville-pekka.lahti@hotmail.com>
 */

#ifndef __SOUND_H__
#define __SOUND_H__

#define SOUND_MAX_CHANNELS 3

#include "sound-adsr.h"
#include "sound-channel.h"
#include "sound-fx-reverb.h"
#include "sound-init.h"
#include "sound-time.h"
#include "sound-waveform.h"
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

#define SOUND_ADRS_ENABLED 1

#define FRAMES_PER_BUFFER (64)

#define SOUND_BITRATE SOUND_DEFAULT_BITRATE

#define SOUND_BUFFER_LENGTH 256

void sound_set_waveform_sample(uint8_t waveform, uint16_t index, int8_t value);

void sound_finetune(uint16_t tune);

int16_t sound_process_one_sample(int8_t pan);
extern int sound_fill_buffer();

extern uint16_t sound_channel_waveform_cursor[SOUND_MAX_CHANNELS];
extern uint16_t sound_channel_waveform_sample_jump[SOUND_MAX_CHANNELS];

extern uint8_t sound_channel_volume[SOUND_MAX_CHANNELS];
extern uint8_t _sound_channel_volume[SOUND_MAX_CHANNELS];
extern int8_t sound_channel_pan[SOUND_MAX_CHANNELS];

extern int16_t sound_waveform[SOUND_MAX_CHANNELS][SOUND_WAVEFORM_MAX_LENGTH];
extern float finetune;

extern uint64_t sound_time;
extern int16_t sound_channel_waveform[SOUND_MAX_CHANNELS];

extern float sound_mixer_fx_master_reverb_factor;

#endif /* __SOUND_H__ */


#ifndef __SOUND_FX_PORTA_H__
#define __SOUND_FX_PORTA_H__

#include "sound.h"
#include <stdbool.h>

#define PORTA_FACTOR 8

void sound_fx_porta_tick();

extern uint16_t sound_channel_waveform_sample_jump_start[SOUND_MAX_CHANNELS];
extern uint16_t sound_channel_waveform_sample_jump_target[SOUND_MAX_CHANNELS];
extern uint32_t sound_porta_channel_start_time[SOUND_MAX_CHANNELS];

#endif
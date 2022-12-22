#ifndef __SOUND_ADSR_H__
#define __SOUND_ADSR_H__

#include "sound.h"
#include <stdint.h>
#define ADSR_ATTACKING 0
#define ADSR_DECAYING 1
#define ADSR_SUSTAINING 2
#define ADSR_RELEASING 3
#define ADSR_OFF 4

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

void sound_set_adsr(uint8_t channel, uint16_t attack, uint16_t decay,
                    uint8_t sustain, uint16_t release);

#endif
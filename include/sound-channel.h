
#ifndef __SOUND_CHANNEL_H__
#define __SOUND_CHANNEL_H__

#include "sound-channel-types.h"
#include "sound-fx-porta-types.h"
#include "sound-waveform-types.h"

void sound_channel_set_hz(uint8_t c, float hz);
void sound_channel_set_waveform(uint8_t c, uint8_t w);
void sound_channel_set_volume(uint8_t c, uint8_t volume);
void sound_channel_set_pan(uint8_t c, int8_t pan);
void sound_channel_set_delay(uint8_t c, uint8_t delay);
void sound_channel_set_porta(uint8_t c, uint16_t porta);
SoundChannel *sound_get_channel(uint8_t channel_num);

extern SoundChannel channels[SOUND_MAX_CHANNELS];

#endif
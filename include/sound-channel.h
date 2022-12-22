
#ifndef __SOUND_CHANNEL_H__
#define __SOUND_CHANNEL_H__

void sound_channel_set_hz(uint8_t channel, float hz);
void sound_channel_set_waveform(uint8_t channel, uint8_t waveform);
void sound_channel_set_volume(uint8_t channel, uint8_t volume);
void sound_channel_set_pan(uint8_t channel, int8_t pan);
void sound_channel_set_delay(uint8_t channel, uint8_t delay);

extern int16_t sound_channel_pan[SOUND_MAX_CHANNELS];

#endif
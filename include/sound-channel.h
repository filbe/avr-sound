
#ifndef __SOUND_CHANNEL_H__
#define __SOUND_CHANNEL_H__

#define SOUND_WAVEFORM_MAX_LENGTH 256
#define SOUND_MAX_WAVEFORMS 16

typedef struct SoundWaveform {
  uint8_t samples[SOUND_WAVEFORM_MAX_LENGTH];
} SoundWaveform;

extern SoundWaveform waveforms[SOUND_MAX_WAVEFORMS];

typedef struct SoundChannelWaveform {
  uint8_t *samples;
  float hz;
  // actual, current jump between 2 waveform samples at playing hz
  uint16_t jump;
  uint16_t cursor;
} SoundChannelWaveform;

typedef struct SoundFxPorta {
  uint16_t jump_start;
  uint16_t jump_target;
  uint16_t value;
  uint32_t start_time;
} SoundFxPorta;

typedef struct SoundChannelFx {
  SoundFxPorta porta;
} SoundChannelFx;

typedef struct SoundChannelMix {
  uint8_t volume;
  int8_t pan;
  uint8_t delay;
} SoundChannelMix;

typedef struct SoundChannelState {
  // channel actual volume, this is calculated with comparing all channel's
  // volumes to prevent audio overflows
  uint8_t volume;
  // porta state; OFF / ON
} SoundChannelState;

typedef struct SoundChannelAdsr {
  uint8_t state;
  uint16_t attack;
  uint16_t decay;
  uint8_t sustain;
  uint16_t release;

  uint32_t attack_flip;
  uint32_t decay_flip;
  uint32_t release_flip;
  uint32_t start_time;
  uint32_t release_time;
} SoundChannelAdsr;

typedef struct SoundChannel {
  SoundChannelState state;
  SoundChannelWaveform waveform;
  // channel volume setting
  SoundChannelMix mix;

  SoundChannelFx fx;
  SoundChannelAdsr adsr;
} SoundChannel;

extern SoundChannel channels[SOUND_MAX_CHANNELS];

void sound_channel_set_hz(uint8_t c, float hz);
void sound_channel_set_waveform(uint8_t c, uint8_t w);
void sound_channel_set_volume(uint8_t c, uint8_t volume);
void sound_channel_set_pan(uint8_t c, int8_t pan);
void sound_channel_set_delay(uint8_t c, uint8_t delay);
void sound_channel_set_porta(uint8_t c, uint16_t porta);
SoundChannel *sound_get_channel(uint8_t channel_num);

#endif
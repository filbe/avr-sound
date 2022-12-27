#ifndef __SOUND_CHANNEL_TYPES_H__
#define __SOUND_CHANNEL_TYPES_H__

#include "sound-fx-porta-types.h"

typedef struct SoundChannelWaveform {
  uint8_t *samples;
  float hz;
  // actual, current jump between 2 waveform samples at playing hz
  uint16_t jump;
  uint16_t cursor;
} SoundChannelWaveform;

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

#endif
#ifndef __SOUND_WAVEFORM_TYPES_H__
#define __SOUND_WAVEFORM_TYPES_H__

#define SOUND_WAVEFORM_MAX_LENGTH 256
#define SOUND_MAX_WAVEFORMS 16

typedef struct SoundWaveform {
  uint8_t samples[SOUND_WAVEFORM_MAX_LENGTH];
} SoundWaveform;

#endif
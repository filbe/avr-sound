#include "sound.h"

void sound_set_waveform(uint8_t channel, uint8_t waveform) {
  if (channel > SOUND_MAX_CHANNELS)
    return;
  current_waveform[channel] = waveform;
}
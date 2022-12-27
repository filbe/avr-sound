#include "sound.h"

SoundWaveform waveforms[SOUND_MAX_WAVEFORMS];

/* Write one sample at a time to waveform on the fly */
void sound_set_waveform_sample(uint8_t waveform, uint16_t index, int8_t value) {
  waveforms[waveform].samples[index] = value;
}

/** Write one whole waveform */
void sound_set_waveform(uint8_t waveform, uint16_t *samples, uint16_t length) {
  if (length > SOUND_WAVEFORM_MAX_LENGTH) {
    // prevent memory overflow
    length = SOUND_WAVEFORM_MAX_LENGTH;
  }

  memcpy(waveforms[waveform].samples, &samples, length * sizeof(int16_t));
}
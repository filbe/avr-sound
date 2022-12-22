#include "sound.h"

int16_t sound_waveform[SOUND_MAX_CHANNELS][SOUND_WAVEFORM_MAX_LENGTH];

/* Write one sample at a time to waveform on the fly */
void sound_set_waveform_sample(uint8_t waveform, uint16_t index, int8_t value) {
  sound_waveform[waveform][index] = value;
}

/** Write one whole waveform */
void sound_set_waveform(uint8_t waveform, uint16_t *samples, uint16_t length) {
  if (length > SOUND_WAVEFORM_MAX_LENGTH) {
    // prevent memory overflow
    length = SOUND_WAVEFORM_MAX_LENGTH;
  }

  memcpy(sound_waveform[waveform], &samples, length * sizeof(int16_t));
}
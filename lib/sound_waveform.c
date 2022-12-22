#include "sound.h"

int16_t sound_waveform[SOUND_MAX_CHANNELS][SOUND_MAXIMUM_SAMPLE_LENGTH];

void sound_set_waveform_sample(uint8_t waveform, uint16_t index, int8_t value) {
  sound_waveform[waveform][index] = value;
}

void sound_set_waveform(uint8_t waveform, uint16_t *samples, uint16_t length) {
  memcpy(sound_waveform[waveform], &samples, length * sizeof(int16_t));
}
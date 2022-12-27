#include "sound.h"
#include <math.h>
#include <portaudio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>

float sound_mixer_fx_master_delay_factor = 0;
uint32_t sound_truncate_count = 0;
float finetune = 1;

void sound_finetune(uint16_t tune) {
  finetune = pow(2, ((tune - 127.5) / 127.5));
}

int16_t sound_process_one_sample(int8_t pan) {

  int32_t _bufsum = 0;
  for (uint8_t i = 0; i < SOUND_MAX_CHANNELS; i++) {
    uint16_t bufspeed = sound_channel_waveform_sample_jump[i];

    int16_t waveform = sound_channel_waveform[i];
    uint16_t sample_cursor =
        (sound_channel_waveform_cursor[i] / 256) % SOUND_WAVEFORM_MAX_LENGTH;

    int16_t sample = sound_waveform[waveform][sample_cursor];
#if SOUND_ADRS_ENABLED == 1
    uint8_t volume = adsr_volume(i);
#else
    uint8_t volume = _sound_channel_volume[i];
#endif

    float pan_factor = (pan ? abs(-130 - sound_channel_pan[i])
                            : abs(130 - sound_channel_pan[i])) /
                       300.0;
    int16_t channel_dry = sample * pan_factor;

    int32_t channel_delay = sound_fx_delay_feed(i * 2 + pan, channel_dry);

    _bufsum += volume * (channel_dry + channel_delay);

    sound_channel_waveform_cursor[i] =
        sound_channel_waveform_cursor[i] + bufspeed;
  }

  int32_t sample = _bufsum;

  // master delay
  int16_t mixed_sample_with_delay =
      sound_fx_delay_feed(SOUND_MAX_CHANNELS + pan, sample);

  return sample;
}
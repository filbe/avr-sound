#include "sound.h"
#include <math.h>
#include <portaudio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>

float sound_mixer_fx_master_reverb_factor = 0;
uint32_t sound_truncate_count = 0;
float finetune = 1;

void sound_finetune(uint16_t tune) {
  finetune = pow(2, ((tune - 127.5) / 127.5));
}

int16_t sound_process_one_sample(int8_t pan) {

  int32_t _bufsum = 0;
  for (uint8_t i = 0; i < SOUND_MAX_CHANNELS; i++) {
    uint16_t bufspeed = sound_channel_waveform_sample_jump[i];
#if SOUND_ADRS_ENABLED == 1
    if (sound_adsr_channel_state[i] == ADSR_OFF || !bufspeed) {
      // continue;
    }
#endif

    int16_t waveform = sound_channel_waveform[i];
    uint16_t sample_cursor =
        (sound_channel_waveform_cursor[i] >> 8) % SOUND_WAVEFORM_MAX_LENGTH;

    int16_t sample = sound_waveform[waveform][sample_cursor];
#if SOUND_ADRS_ENABLED == 1
    uint8_t volume = sound_adsr_channel_state[i] == ADSR_OFF
                         ? _sound_channel_volume[i]
                         : adsr_volume(i);
#else
    uint8_t volume = _sound_channel_volume[i];
#endif

    float pan_factor = (pan ? abs(-127 - sound_channel_pan[i])
                            : abs(127 - sound_channel_pan[i])) /
                       254.0;
    int16_t channel_dry = sample * volume * pan_factor;

    _bufsum += channel_dry;

    int16_t channel_reverb = sound_fx_reverb_feed(i * 2 + pan, channel_dry);

    _bufsum += channel_reverb;

    sound_channel_waveform_cursor[i] = sound_channel_waveform_cursor[i] +
                                       sound_channel_waveform_sample_jump[i];
  }

  int32_t sample = _bufsum;

  // master reverb
  int16_t mixed_sample_with_reverb =
      sound_fx_reverb_feed(SOUND_MAX_CHANNELS + pan, sample);

  return sample;
}
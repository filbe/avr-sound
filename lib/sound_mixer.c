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

uint8_t delay_volume = 50;

/**
int16_t maxsample = 0;
int16_t minsample = 0;
 */

uint16_t sound_process_one_sample(int8_t pan) {

  int32_t _bufsum = 0;
  for (uint8_t i = 0; i < SOUND_MAX_CHANNELS; i++) {
    SoundChannel *ch = sound_get_channel(i);
    uint16_t bufspeed = ch->waveform.jump;

    uint8_t *samples = ch->waveform.samples;
    uint16_t sample_cursor =
        (ch->waveform.cursor / 256) % SOUND_WAVEFORM_MAX_LENGTH;

    int16_t sample = samples[sample_cursor];
#if SOUND_ADRS_ENABLED == 1
    uint8_t volume = adsr_volume(i);
#else
    uint8_t volume = ch->state.volume * 0.7;
#endif

    float pan_factor =
        (pan ? abs(-127 - ch->mix.pan) : abs(127 - ch->mix.pan)) / 256.0;
    int16_t channel_dry = sample * pan_factor;

    int16_t channel_delay = sound_fx_delay_feed(i * 2 + pan, channel_dry);

    _bufsum += (volume * channel_dry + ch->state.volume * channel_delay);

    ch->waveform.cursor = ch->waveform.cursor + bufspeed;
  }

  int16_t sample = _bufsum;

  // master delay
  int16_t mixed_sample_with_delay =
      sound_fx_delay_feed(SOUND_MAX_CHANNELS + pan, sample);

  /**
   * @brief
   * if (sample > maxsample) {
      maxsample = sample;
      printf("%hd...%hd\n", minsample, maxsample);
    }
    if (sample < minsample) {
      minsample = sample;
      printf("%hd...%hd\n", minsample, maxsample);
    }
   */

  return sample - 32768;
}
#include "sound.h"
#include <math.h>
#include <portaudio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>

uint16_t sound_mixer_delay_buffer_cursor = 0;
float sound_mixer_fx_master_delay_factor = 0;
uint32_t sound_truncate_count = 0;
float finetune = 1;

int16_t sound_mixer_delay_buffer[2][sound_mixer_delay_buffer_length];

void sound_finetune(uint16_t tune) {
  finetune = pow(2, ((tune - 127.5) / 127.5));
}

int16_t sound_process_one_sample(int8_t pan) {

  int32_t _bufsum = 0;
  for (uint8_t i = 0; i < SOUND_MAX_CHANNELS; i++) {
    uint16_t bufspeed = sound_channel_waveform_sample_jump[i];
#if SOUND_ADRS_ENABLED == 1
    if (sound_adsr_channel_state[i] == ADSR_OFF || !bufspeed) {
      continue;
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

    if (volume <= 2)
      volume = 0;
    // if (sample <= 2) sample = 0;

    float pan_factor = (pan ? abs(-127 - sound_channel_pan[i])
                            : abs(127 - sound_channel_pan[i])) /
                       254.0;
    _bufsum += sample * volume * pan_factor;
    sound_channel_waveform_cursor[i] =
        (sound_channel_waveform_cursor[i] +
         sound_channel_waveform_sample_jump[i] * (1 + sound_truncate_count));
  }
  sound_truncate_count = 0;

  int32_t sample = _bufsum;
  int32_t delay_sample =
      (sound_mixer_delay_buffer[pan][sound_mixer_delay_buffer_cursor] / 1.3) +
      (sound_mixer_delay_buffer[pan][(sound_mixer_delay_buffer_cursor +
                                      sound_mixer_delay_buffer_length + 1026) %
                                     sound_mixer_delay_buffer_length] /
           4 +
       96) +
      (sound_mixer_delay_buffer[pan][(sound_mixer_delay_buffer_cursor +
                                      sound_mixer_delay_buffer_length + 1226) %
                                     sound_mixer_delay_buffer_length] /
           8 +
       112) +
      (sound_mixer_delay_buffer[pan][(sound_mixer_delay_buffer_cursor +
                                      sound_mixer_delay_buffer_length + 626) %
                                     sound_mixer_delay_buffer_length] /
           4 +
       96) +
      (sound_mixer_delay_buffer[pan][(sound_mixer_delay_buffer_cursor +
                                      sound_mixer_delay_buffer_length + 777) %
                                     sound_mixer_delay_buffer_length] /
           8 +
       112) +
      (sound_mixer_delay_buffer[pan][(sound_mixer_delay_buffer_cursor +
                                      sound_mixer_delay_buffer_length + 274) %
                                     sound_mixer_delay_buffer_length] /
           8 +
       112) +
      (sound_mixer_delay_buffer[pan][(sound_mixer_delay_buffer_cursor +
                                      sound_mixer_delay_buffer_length + 626) %
                                     sound_mixer_delay_buffer_length] /
           4 +
       96) +
      (sound_mixer_delay_buffer[pan][(sound_mixer_delay_buffer_cursor +
                                      sound_mixer_delay_buffer_length + 1526) %
                                     sound_mixer_delay_buffer_length] /
           8 +
       112);

  sound_mixer_delay_buffer[pan][sound_mixer_delay_buffer_cursor] =
      ((sample + (delay_sample * sound_mixer_fx_master_delay_factor * 0.3)));
  sound_mixer_delay_buffer_cursor++;
  if (sound_mixer_delay_buffer_cursor % (sound_mixer_delay_buffer_length) ==
      0) {
    sound_mixer_delay_buffer_cursor = 0;
  }

  int16_t out =
      (int16_t)((sample + (delay_sample * sound_mixer_fx_master_delay_factor *
                           sound_mixer_fx_master_delay_factor)) /
                2);

  return out;
}
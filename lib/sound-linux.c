/*
 * Universal Synth
 * Authors:
 *  Ville-Pekka Lahti <ville-pekka.lahti@hotmail.com>
 */

#include "sound-linux.h"
#include <math.h>
#include <portaudio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>

volatile uint16_t sound_buffercursor[SOUND_MAX_CHANNELS];
volatile float sound_buffer_jump = 1 >> 8;
volatile uint16_t sound_buffer_speed[SOUND_MAX_CHANNELS];

volatile uint8_t sound_buffer_volume[SOUND_MAX_CHANNELS];
volatile uint8_t _sound_buffer_volume[SOUND_MAX_CHANNELS];
volatile int8_t sound_buffer_pan[SOUND_MAX_CHANNELS];
volatile uint32_t sound_channel_start_time[SOUND_MAX_CHANNELS];
volatile uint32_t sound_channel_release_time[SOUND_MAX_CHANNELS];
volatile uint8_t sound_channel_adsr_state[SOUND_MAX_CHANNELS];

volatile uint16_t sound_channel_adsr_attack[SOUND_MAX_CHANNELS];
volatile uint16_t sound_channel_adsr_decay[SOUND_MAX_CHANNELS];
volatile uint8_t sound_channel_adsr_sustain[SOUND_MAX_CHANNELS];
volatile uint16_t sound_channel_adsr_release[SOUND_MAX_CHANNELS];
volatile uint32_t sound_channel_adsr_attack_flip[SOUND_MAX_CHANNELS];
volatile uint32_t sound_channel_adsr_decay_flip[SOUND_MAX_CHANNELS];
volatile uint32_t sound_channel_adsr_release_flip[SOUND_MAX_CHANNELS];

volatile uint16_t sound_buffer_volume_sum = 0;

volatile float sound_buffer_hz = 440.0;
volatile uint16_t sound_buffer_len = SOUND_MAXIMUM_SAMPLE_LENGTH;
volatile int8_t sound_buffer[SOUND_MAX_CHANNELS][SOUND_MAXIMUM_SAMPLE_LENGTH];
volatile float finetune = 1;

volatile uint64_t sound_time;
volatile uint8_t current_waveform[SOUND_MAX_CHANNELS];

volatile int8_t delay_buffer[2][DELAY_BUFFER_LENGTH];

void sound_set_volume(uint8_t channel, uint8_t volume) {
  if (channel >= SOUND_MAX_CHANNELS)
    return;
  sound_buffer_volume[channel] = volume;

  // count total to be able to scale volumes in mixing
  sound_buffer_volume_sum = 0;
  for (uint8_t i = 0; i < SOUND_MAX_CHANNELS; i++) {
    sound_buffer_volume_sum += sound_buffer_volume[i];
  }
  if (sound_buffer_volume_sum < 255)
    sound_buffer_volume_sum = 255;
  for (uint8_t i = 0; i < SOUND_MAX_CHANNELS; i++) {
    _sound_buffer_volume[i] =
        255 * sound_buffer_volume[i] / sound_buffer_volume_sum;
  }
}

void sound_set_pan(uint8_t channel, int8_t pan) {
  if (channel >= SOUND_MAX_CHANNELS)
    return;
  sound_buffer_pan[channel] = pan;
}

void sound_sample_init(uint16_t sample_len, float hz) {
  // Forcing maximum sample length if trying bigger
  if (sample_len > SOUND_MAXIMUM_SAMPLE_LENGTH) {
    sample_len = SOUND_MAXIMUM_SAMPLE_LENGTH;
  }
  sound_buffer_hz = sample_len;
}

void sound_setbuffer(uint8_t waveform, uint16_t index, int8_t value) {
  sound_buffer[waveform][index] = value;
}

void sound_set_hz(uint8_t channel, float hz) {
  if (channel > SOUND_MAX_CHANNELS)
    return;
  uint32_t newspeed = sound_buffer_speed[channel];
  if (hz < 20) {
#if SOUND_ADRS_ENABLED == 1
    if (sound_channel_adsr_state[channel] != ADSR_RELEASING &&
        sound_channel_adsr_state[channel] != ADSR_OFF) {
      sound_channel_adsr_state[channel] = ADSR_RELEASING;
      sound_channel_release_time[channel] = sound_time;
    }
    sound_channel_start_time[channel] = sound_time;
#else
    sound_buffer_speed[channel] = 0;
#endif
  } else {
    newspeed = finetune * 256.0 * 256.0 * sound_buffer_len * hz /
               (SOUND_BITRATE * sound_buffer_hz);
#if SOUND_ADRS_ENABLED == 1
    if (sound_channel_adsr_state[channel] != ADSR_ATTACKING &&
        sound_channel_adsr_state[channel] != ADSR_DECAYING) {
      sound_channel_adsr_state[channel] = ADSR_ATTACKING;
      if (sound_buffer_speed[channel] != newspeed) {
        sound_channel_start_time[channel] = sound_time;
      }
    }

#endif
    sound_buffer_speed[channel] = newspeed;
  }
}

void sound_set_waveform(uint8_t channel, uint8_t waveform) {
  if (channel > SOUND_MAX_CHANNELS)
    return;
  current_waveform[channel] = waveform;
}

void sound_finetune(uint16_t tune) {
  finetune = pow(2, ((tune - 127.5) / 127.5));
}

void sound_set_adsr(uint8_t channel, uint16_t attack, uint16_t decay,
                    uint8_t sustain, uint16_t release) {
  if (channel > SOUND_MAX_CHANNELS)
    return;
#if SOUND_ADRS_ENABLED == 1
  sound_channel_adsr_attack[channel] = attack;
  sound_channel_adsr_decay[channel] = decay;
  sound_channel_adsr_sustain[channel] = sustain;
  sound_channel_adsr_release[channel] = release;

  sound_channel_adsr_attack_flip[channel] = 256.0 * 65536.0 / (float)(attack);
  sound_channel_adsr_decay_flip[channel] = 256.0 * 65536.0 / (float)(decay);
  sound_channel_adsr_release_flip[channel] = 256.0 * 65536.0 / (float)(release);
#endif
}

uint16_t adsr_volume(uint8_t channel) {
  if (channel > SOUND_MAX_CHANNELS)
    return 0;
  uint16_t max_volume = _sound_buffer_volume[channel];
  uint32_t start_time = sound_channel_start_time[channel];
  uint32_t release_time = sound_channel_release_time[channel];

  uint32_t adsr_a = sound_channel_adsr_attack[channel];
  uint32_t adsr_d = sound_channel_adsr_decay[channel];
  uint32_t adsr_s = sound_channel_adsr_sustain[channel];
  uint32_t adsr_r = sound_channel_adsr_release[channel];

  uint32_t adsr_a_flip = sound_channel_adsr_attack_flip[channel];
  uint32_t adsr_d_flip = sound_channel_adsr_decay_flip[channel];
  uint32_t adsr_r_flip = sound_channel_adsr_release_flip[channel];

  uint32_t time_diff = sound_time - start_time;
  uint32_t time_diff_release = sound_time - release_time;
  // uint32_t adsr_compare_val = adsr_a;
  uint32_t time_begin_part = 0;

  int16_t volume_out = 0;

  if (sound_channel_adsr_state[channel] != ADSR_RELEASING &&
      sound_channel_adsr_state[channel] != ADSR_OFF) {
    if (time_diff < adsr_a) {
      // attack
      volume_out = max_volume * time_diff * adsr_a_flip >> 24;
    } else if (time_diff < adsr_a + adsr_d) {
      // decay is falling line
      sound_channel_adsr_state[channel] = ADSR_DECAYING;
      time_begin_part = time_diff - adsr_a;
      volume_out =
          max_volume -
          ((max_volume - adsr_s) * time_begin_part * adsr_d_flip >> 24);
    } else {
      sound_channel_adsr_state[channel] = ADSR_SUSTAINING;
      volume_out = adsr_s;
    }
  } else if (sound_channel_adsr_state[channel] == ADSR_RELEASING) {
    if (time_diff_release < adsr_r) {
      // release
      volume_out = (adsr_r - time_diff_release) * adsr_s * adsr_r_flip >> 24;
    } else {
      sound_channel_adsr_state[channel] = ADSR_OFF;
      sound_buffer_speed[channel] = 0;
      volume_out = 0;
    }
  } else {
    volume_out = max_volume;
  }

  // sound_buffer_speed[channel] = 0;
  return volume_out > 0 ? (_sound_buffer_volume[channel] * volume_out) >> 8 : 0;
}

volatile uint16_t sound_truncate_count = 0;

volatile delay_buffer_cursor = 0;

volatile float sound_delay_factor = 0.52;

volatile uint8_t sound_process_one_sample(int8_t pan) {

  int16_t _bufsum = 0;
  for (uint8_t i = 0; i < SOUND_MAX_CHANNELS; i++) {
    uint8_t bufspeed = sound_buffer_speed[i];
#if SOUND_ADRS_ENABLED == 1
    if (sound_channel_adsr_state[i] == ADSR_OFF || !bufspeed) {
      continue;
    }
#endif

    int8_t waveform = current_waveform[i];
    uint8_t sample_cursor =
        (sound_buffercursor[i] >> 8) % SOUND_MAXIMUM_SAMPLE_LENGTH;

    int8_t sample = sound_buffer[waveform][sample_cursor];
#if SOUND_ADRS_ENABLED == 1
    uint8_t volume = sound_channel_adsr_state[i] == ADSR_OFF
                         ? _sound_buffer_volume[i]
                         : adsr_volume(i);
#else
    uint8_t volume = _sound_buffer_volume[i];
#endif

    if (volume <= 2)
      volume = 0;
    // if (sample <= 2) sample = 0;

    float pan_factor = (pan ? abs(-127 - sound_buffer_pan[i])
                            : abs(127 - sound_buffer_pan[i])) /
                       254.0;
    _bufsum += sample * volume * pan_factor;
    sound_buffercursor[i] =
        (sound_buffercursor[i] +
         sound_buffer_speed[i] * (1 + sound_truncate_count));
  }
  sound_truncate_count = 0;

  int8_t sample = _bufsum >> 6;
  int8_t delay_sample =
      (delay_buffer[pan][delay_buffer_cursor] / 1.3) +
      (delay_buffer[pan][(delay_buffer_cursor + DELAY_BUFFER_LENGTH + 1026) %
                         DELAY_BUFFER_LENGTH] /
           4 +
       96) +
      (delay_buffer[pan][(delay_buffer_cursor + DELAY_BUFFER_LENGTH + 1226) %
                         DELAY_BUFFER_LENGTH] /
           8 +
       112) +
      (delay_buffer[pan][(delay_buffer_cursor + DELAY_BUFFER_LENGTH + 626) %
                         DELAY_BUFFER_LENGTH] /
           4 +
       96) +
      (delay_buffer[pan][(delay_buffer_cursor + DELAY_BUFFER_LENGTH + 777) %
                         DELAY_BUFFER_LENGTH] /
           8 +
       112) +
      (delay_buffer[pan][(delay_buffer_cursor + DELAY_BUFFER_LENGTH + 274) %
                         DELAY_BUFFER_LENGTH] /
           8 +
       112) +
      (delay_buffer[pan][(delay_buffer_cursor + DELAY_BUFFER_LENGTH + 626) %
                         DELAY_BUFFER_LENGTH] /
           4 +
       96) +
      (delay_buffer[pan][(delay_buffer_cursor + DELAY_BUFFER_LENGTH + 1526) %
                         DELAY_BUFFER_LENGTH] /
           8 +
       112);

  delay_buffer[pan][delay_buffer_cursor] =
      ((sample + (delay_sample * sound_delay_factor * 0.3)));
  delay_buffer_cursor++;
  if (delay_buffer_cursor % (DELAY_BUFFER_LENGTH) == 0) {
    delay_buffer_cursor = 0;
  }

  return (int8_t)((sample +
                   (delay_sample * sound_delay_factor * sound_delay_factor)) /
                  2);
}
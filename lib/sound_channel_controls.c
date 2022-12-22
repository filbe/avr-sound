/**
 *
 * Channel Controls
 *
 *  __________________________________________________
 *  |                                                 |
 *  |   ░       ▒       ░       ░     ❏  ❏  ❏  ❏  ❏   |
 *  |   ▒       ▓       ░       ░     ▣  ❏  ▣  ❏  ▣   |
 *  |   ▓       ▓       ░       ▓     ❏  ▣  ▣  ❏  ❏   |
 *  |  ___     ___     ___     ___     ___     ___    |
 *  | /   \   /   \   /   \   /   \   /   \   /   \   |
 *  ||  O  | |  O  | |  O  | |  O  | |  O  | |  O  |  |
 *  | \__\/   \/__/   \__\/   \/__/   \__\/   \/__/   |
 *  |                                                 |
 *  |          universal synth channel ctrls          |
 *  |_________________________________________________|
 *
 */

#include "sound.h"

uint8_t sound_channel_volume[SOUND_MAX_CHANNELS];
uint8_t _sound_channel_volume[SOUND_MAX_CHANNELS];
int16_t sound_channel_pan[SOUND_MAX_CHANNELS];

uint8_t sound_channel_delay[SOUND_MAX_CHANNELS];

int16_t sound_channel_waveform[SOUND_MAX_CHANNELS];

void sound_channel_set_volume(uint8_t channel, uint8_t volume) {
  if (channel >= SOUND_MAX_CHANNELS)
    return;
  sound_channel_volume[channel] = volume;

  // count total to be able to scale volumes in mixing
  uint16_t sound_channel_volume_sum = 0;
  for (uint8_t i = 0; i < SOUND_MAX_CHANNELS; i++) {
    sound_channel_volume_sum += sound_channel_volume[i];
  }
  if (sound_channel_volume_sum < 255)
    sound_channel_volume_sum = 255;
  for (uint8_t i = 0; i < SOUND_MAX_CHANNELS; i++) {
    _sound_channel_volume[i] =
        255 * sound_channel_volume[i] / sound_channel_volume_sum;
  }
}

void sound_channel_set_pan(uint8_t channel, int8_t pan) {
  if (channel >= SOUND_MAX_CHANNELS)
    return;
  sound_channel_pan[channel] = pan;
}

void sound_channel_set_delay(uint8_t channel, uint8_t delay) {
  if (channel >= SOUND_MAX_CHANNELS)
    return;
  sound_channel_delay[channel] = delay;
}

void sound_channel_set_hz(uint8_t channel, float hz) {
  if (channel > SOUND_MAX_CHANNELS)
    return;
  uint32_t newspeed = sound_channel_waveform_sample_jump[channel];
  if (hz < 20) {
#if SOUND_ADRS_ENABLED == 1
    if (sound_adsr_channel_state[channel] != ADSR_RELEASING &&
        sound_adsr_channel_state[channel] != ADSR_OFF) {
      sound_adsr_channel_state[channel] = ADSR_RELEASING;
      sound_adsr_channel_release_time[channel] = sound_time;
    }
    sound_adsr_channel_start_time[channel] = sound_time;
#else
    sound_channel_waveform_sample_jump[channel] = 0;
#endif
  } else {
    newspeed = finetune * 256.0 * 256.0 * SOUND_WAVEFORM_MAX_LENGTH * hz /
               (SOUND_BITRATE * SOUND_WAVEFORM_MAX_LENGTH);
#if SOUND_ADRS_ENABLED == 1
    if (sound_adsr_channel_state[channel] != ADSR_ATTACKING &&
        sound_adsr_channel_state[channel] != ADSR_DECAYING) {
      sound_adsr_channel_state[channel] = ADSR_ATTACKING;
      if (sound_channel_waveform_sample_jump[channel] != newspeed) {
        sound_adsr_channel_start_time[channel] = sound_time;
      }
    }

#endif
    sound_channel_waveform_sample_jump[channel] = newspeed;
  }
}

void sound_channel_set_waveform(uint8_t channel, uint8_t waveform) {
  if (channel > SOUND_MAX_CHANNELS)
    return;
  sound_channel_waveform[channel] = waveform;
}
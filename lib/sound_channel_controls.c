
#include "sound.h"

void sound_channel_set_volume(uint8_t channel, uint8_t volume) {
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

void sound_channel_set_pan(uint8_t channel, int8_t pan) {
  if (channel >= SOUND_MAX_CHANNELS)
    return;
  sound_buffer_pan[channel] = pan;
}

void sound_channel_set_hz(uint8_t channel, float hz) {
  if (channel > SOUND_MAX_CHANNELS)
    return;
  uint32_t newspeed = sound_buffer_speed[channel];
  if (hz < 20) {
#if SOUND_ADRS_ENABLED == 1
    if (sound_adsr_channel_state[channel] != ADSR_RELEASING &&
        sound_adsr_channel_state[channel] != ADSR_OFF) {
      sound_adsr_channel_state[channel] = ADSR_RELEASING;
      sound_adsr_channel_release_time[channel] = sound_time;
    }
    sound_adsr_channel_start_time[channel] = sound_time;
#else
    sound_buffer_speed[channel] = 0;
#endif
  } else {
    newspeed = finetune * 256.0 * 256.0 * sound_buffer_len * hz /
               (SOUND_BITRATE * sound_buffer_hz);
#if SOUND_ADRS_ENABLED == 1
    if (sound_adsr_channel_state[channel] != ADSR_ATTACKING &&
        sound_adsr_channel_state[channel] != ADSR_DECAYING) {
      sound_adsr_channel_state[channel] = ADSR_ATTACKING;
      if (sound_buffer_speed[channel] != newspeed) {
        sound_adsr_channel_start_time[channel] = sound_time;
      }
    }

#endif
    sound_buffer_speed[channel] = newspeed;
  }
}
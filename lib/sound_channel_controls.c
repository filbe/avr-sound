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

#include "sound-fx-porta.h"
#include "sound.h"

uint8_t sound_channel_volume[SOUND_MAX_CHANNELS];
uint8_t _sound_channel_volume[SOUND_MAX_CHANNELS];
int8_t sound_channel_pan[SOUND_MAX_CHANNELS];
uint16_t sound_channel_porta[SOUND_MAX_CHANNELS];

uint8_t sound_channel_delay[SOUND_MAX_CHANNELS];

int16_t sound_channel_waveform[SOUND_MAX_CHANNELS];

uint16_t sound_channel_waveform_cursor[SOUND_MAX_CHANNELS];
float sound_channel_hz[SOUND_MAX_CHANNELS];
uint16_t sound_channel_waveform_sample_jump[SOUND_MAX_CHANNELS];
uint16_t sound_channel_waveform_sample_jump_start[SOUND_MAX_CHANNELS];
uint16_t sound_channel_waveform_sample_jump_target[SOUND_MAX_CHANNELS];

void sound_channel_set_volume(uint8_t ch, uint8_t volume) {
  if (ch >= SOUND_MAX_CHANNELS)
    return;
  sound_channel_volume[ch] = volume;

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

void sound_channel_set_pan(uint8_t ch, int8_t pan) {
  if (ch >= SOUND_MAX_CHANNELS)
    return;
  sound_channel_pan[ch] = pan;
}

void sound_channel_set_porta(uint8_t ch, uint16_t porta) {
  if (ch >= SOUND_MAX_CHANNELS)
    return;
  sound_channel_porta[ch] = porta;
}

void sound_channel_set_delay(uint8_t ch, uint8_t delay) {
  if (ch >= SOUND_MAX_CHANNELS)
    return;
  sound_channel_delay[ch] = delay;
}

void sound_channel_set_hz(uint8_t ch, float hz) {
  if (ch > SOUND_MAX_CHANNELS)
    return;
  uint32_t newspeed = sound_channel_waveform_sample_jump_target[ch];
  if (hz < 20) {
#if SOUND_ADRS_ENABLED == 1
    if (sound_adsr_channel_state[ch] != ADSR_RELEASING &&
        sound_adsr_channel_state[ch] != ADSR_OFF) {
      sound_adsr_channel_state[ch] = ADSR_RELEASING;
      sound_adsr_channel_release_time[ch] = sound_time;
    }
    // sound_adsr_channel_start_time[ch] = sound_time;
#else
    sound_channel_waveform_sample_jump[ch] = 0;
    sound_channel_waveform_sample_jump_target[ch] = 0;
#endif
  } else {
    newspeed = finetune * 256.0 * 256.0 * SOUND_WAVEFORM_MAX_LENGTH * hz /
               (SOUND_BITRATE * SOUND_WAVEFORM_MAX_LENGTH);
#if SOUND_ADRS_ENABLED == 1
    if (sound_adsr_channel_state[ch] != ADSR_ATTACKING &&
        sound_adsr_channel_state[ch] != ADSR_DECAYING) {
      sound_adsr_channel_state[ch] = ADSR_ATTACKING;
      // sound_adsr_channel_start_time[ch] = sound_time;
    }
#endif

    sound_channel_hz[ch] = hz;

    if (sound_channel_porta[ch]) {
      sound_channel_waveform_sample_jump_start[ch] =
          sound_channel_waveform_sample_jump[ch];
      sound_porta_channel_start_time[ch] = sound_time;
    }
    sound_channel_waveform_sample_jump_target[ch] = newspeed;
  }
}

void sound_channel_set_waveform(uint8_t ch, uint8_t waveform) {
  if (ch > SOUND_MAX_CHANNELS)
    return;
  sound_channel_waveform[ch] = waveform;
}
#include "sound.h"

uint32_t sound_adsr_channel_start_time[SOUND_MAX_CHANNELS];
uint32_t sound_adsr_channel_release_time[SOUND_MAX_CHANNELS];
uint8_t sound_adsr_channel_state[SOUND_MAX_CHANNELS];

uint16_t sound_adsr_channel_attack[SOUND_MAX_CHANNELS];
uint16_t sound_adsr_channel_decay[SOUND_MAX_CHANNELS];
uint8_t sound_adsr_channel_sustain[SOUND_MAX_CHANNELS];
uint16_t sound_adsr_channel_release[SOUND_MAX_CHANNELS];
uint32_t sound_adsr_channel_attack_flip[SOUND_MAX_CHANNELS];
uint32_t sound_adsr_channel_decay_flip[SOUND_MAX_CHANNELS];
uint32_t sound_adsr_channel_release_flip[SOUND_MAX_CHANNELS];

void sound_set_adsr(uint8_t channel, uint16_t attack, uint16_t decay,
                    uint8_t sustain, uint16_t release) {
  if (channel > SOUND_MAX_CHANNELS)
    return;
#if SOUND_ADRS_ENABLED == 1
  sound_adsr_channel_attack[channel] = attack;
  sound_adsr_channel_decay[channel] = decay;
  sound_adsr_channel_sustain[channel] = sustain;
  sound_adsr_channel_release[channel] = release;

  sound_adsr_channel_attack_flip[channel] = 256.0 * 65536.0 / (float)(attack);
  sound_adsr_channel_decay_flip[channel] = 256.0 * 65536.0 / (float)(decay);
  sound_adsr_channel_release_flip[channel] = 256.0 * 65536.0 / (float)(release);
#endif
}

uint16_t adsr_volume(uint8_t channel) {
  if (channel > SOUND_MAX_CHANNELS)
    return 0;
  uint16_t max_volume = _sound_buffer_volume[channel];
  uint32_t start_time = sound_adsr_channel_start_time[channel];
  uint32_t release_time = sound_adsr_channel_release_time[channel];

  uint32_t adsr_a = sound_adsr_channel_attack[channel];
  uint32_t adsr_d = sound_adsr_channel_decay[channel];
  uint32_t adsr_s = sound_adsr_channel_sustain[channel];
  uint32_t adsr_r = sound_adsr_channel_release[channel];

  uint32_t adsr_a_flip = sound_adsr_channel_attack_flip[channel];
  uint32_t adsr_d_flip = sound_adsr_channel_decay_flip[channel];
  uint32_t adsr_r_flip = sound_adsr_channel_release_flip[channel];

  uint32_t time_diff = sound_time - start_time;
  uint32_t time_diff_release = sound_time - release_time;
  // uint32_t adsr_compare_val = adsr_a;
  uint32_t time_begin_part = 0;

  int16_t volume_out = 0;

  if (sound_adsr_channel_state[channel] != ADSR_RELEASING &&
      sound_adsr_channel_state[channel] != ADSR_OFF) {
    if (time_diff < adsr_a) {
      // attack
      volume_out = max_volume * time_diff * adsr_a_flip >> 24;
    } else if (time_diff < adsr_a + adsr_d) {
      // decay is falling line
      sound_adsr_channel_state[channel] = ADSR_DECAYING;
      time_begin_part = time_diff - adsr_a;
      volume_out =
          max_volume -
          ((max_volume - adsr_s) * time_begin_part * adsr_d_flip >> 24);
    } else {
      sound_adsr_channel_state[channel] = ADSR_SUSTAINING;
      volume_out = adsr_s;
    }
  } else if (sound_adsr_channel_state[channel] == ADSR_RELEASING) {
    if (time_diff_release < adsr_r) {
      // release
      volume_out = (adsr_r - time_diff_release) * adsr_s * adsr_r_flip >> 24;
    } else {
      sound_adsr_channel_state[channel] = ADSR_OFF;
      sound_buffer_speed[channel] = 0;
      volume_out = 0;
    }
  } else {
    volume_out = max_volume;
  }

  // sound_buffer_speed[channel] = 0;
  return volume_out > 0 ? (_sound_buffer_volume[channel] * volume_out) >> 8 : 0;
}
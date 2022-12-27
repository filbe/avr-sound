#include "sound.h"
/**
 * ADSR -module
 *
 *      /\ DECAY
 *     /  \/     SUSTAIN
 *    /    \______/_____
 *   / \                 \ RELEASE
 *  / ATTACK              \ /
 * /                       \
 *
 *

 */

void sound_set_adsr(uint8_t c, uint16_t attack, uint16_t decay, uint8_t sustain,
                    uint16_t release) {
  SoundChannel *ch = sound_get_channel(c);

#if SOUND_ADRS_ENABLED == 1
  ch->adsr.attack = attack;
  ch->adsr.decay = decay;
  ch->adsr.sustain = sustain;
  ch->adsr.release = release;

  ch->adsr.attack_flip = 256.0 * 65536.0 / (float)(attack);
  ch->adsr.decay_flip = 256.0 * 65536.0 / (float)(decay);
  ch->adsr.release_flip = 256.0 * 65536.0 / (float)(release);
#endif
}

uint16_t adsr_volume(uint8_t c) {
  SoundChannel *ch = sound_get_channel(c);

  uint16_t max_volume = ch->state.volume;
  uint32_t start_time = ch->adsr.start_time;
  uint32_t release_time = ch->adsr.release_time;

  uint32_t adsr_a = ch->adsr.attack;
  uint32_t adsr_d = ch->adsr.decay;
  uint32_t adsr_s = ch->adsr.sustain;
  uint32_t adsr_r = ch->adsr.release;

  uint32_t adsr_a_flip = ch->adsr.attack_flip;
  uint32_t adsr_d_flip = ch->adsr.decay_flip;
  uint32_t adsr_r_flip = ch->adsr.release_flip;

  uint32_t time_diff = sound_time - start_time;
  uint32_t time_diff_release = sound_time - release_time;
  // uint32_t adsr_compare_val = adsr_a;
  uint32_t time_begin_part = 0;

  int16_t volume_out = 0;

  if (ch->adsr.state != ADSR_RELEASING && ch->adsr.state != ADSR_OFF) {
    if (time_diff < adsr_a) {
      // attack
      volume_out = max_volume * time_diff * adsr_a_flip >> 24;
    } else if (time_diff < adsr_a + adsr_d) {
      // decay is falling line
      ch->adsr.state = ADSR_DECAYING;
      time_begin_part = time_diff - adsr_a;
      volume_out =
          max_volume -
          ((max_volume - adsr_s) * time_begin_part * adsr_d_flip >> 24);
    } else {
      ch->adsr.state = ADSR_SUSTAINING;
      volume_out = adsr_s;
    }
  } else if (ch->adsr.state == ADSR_RELEASING) {
    if (time_diff_release < adsr_r) {
      // release
      volume_out = (adsr_r - time_diff_release) * adsr_s * adsr_r_flip >> 24;
    } else {
      ch->adsr.state = ADSR_OFF;
      ch->fx.porta.jump_target = 0;
      volume_out = 0;
    }
  } else {
    volume_out = max_volume;
  }

  // sound_channel_waveform_sample_jump[channel] = 0;
  return volume_out > 0 ? (ch->state.volume * volume_out) >> 8 : 0;
}
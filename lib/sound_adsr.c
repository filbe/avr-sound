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
  // set ADSR settings
  ch->adsr.attack = attack;
  ch->adsr.decay = decay;
  ch->adsr.sustain = sustain;
  ch->adsr.release = release;

  /** Pre-calculate some useful values
   * We want to prevent float operations and divisions, so instead we can do
   * multiplication with inverse numbers that are bit-shifted to left to have
   * precise value (as we might be losing precision if we calculated the values
   * without multiplying them first). When multiplying with 256, we have to >> 8
   * at the end; The same applies for 65536 ( >> 16) and 16777216 ( >> 24 )
   *
   */
  ch->adsr.attack_flip = 16777216.0 / (float)(attack);
  ch->adsr.decay_flip = 16777216.0 / (float)(decay);
  ch->adsr.release_flip = 16777216.0 / (float)(release);
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
      // attack, volume is going from zero to maximum

      // remember that we have multiplied the value by 2^24, so let's revert
      // that effect after calculation
      volume_out = max_volume * time_diff * adsr_a_flip >> 24;
    } else if (time_diff < adsr_a + adsr_d) {
      // decay, volume is falling to sustain level
      ch->adsr.state = ADSR_DECAYING;
      time_begin_part = time_diff - adsr_a;
      // remember that we have multiplied the value by 2^24, so let's revert
      // that effect after calculation
      volume_out =
          max_volume -
          ((max_volume - adsr_s) * time_begin_part * adsr_d_flip >> 24);
    } else {
      // we have run out of decay time, so switch to sustain mode
      ch->adsr.state = ADSR_SUSTAINING;
      volume_out = adsr_s;
    }
  } else if (ch->adsr.state == ADSR_RELEASING) {
    // release triggered, volume is falling from sustain level to zero
    if (time_diff_release < adsr_r) {
      // release time left
      // remember that we have multiplied the value by 2^24, so let's revert
      // that effect after calculation
      volume_out = (adsr_r - time_diff_release) * adsr_s * adsr_r_flip >> 24;
    } else {
      // release time is out, turn ADSR OFF
      ch->adsr.state = ADSR_OFF;
      ch->fx.porta.jump_target = 0;
      volume_out = 0;
    }
  } else {
    // play without ADSR when the ADSR cycle is not
    volume_out = max_volume;
  }

  // as we are multiplying the sample (0...255) with volume (0...255), we want
  // to ensure that we get a value between 0...255 so we have to bitshitf 8
  // (same as dividing with 256). By doing that we can get rid of one float op
  return volume_out > 0 ? (ch->state.volume * volume_out) >> 8 : 0;
}
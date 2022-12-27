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

SoundChannel channels[SOUND_MAX_CHANNELS];

SoundChannel *sound_get_channel(uint8_t channel_num) {
  return &channels[channel_num];
}

void sound_channel_set_volume(uint8_t c, uint8_t volume) {
  SoundChannel *ch = sound_get_channel(c);
  ch->mix.volume = volume;

  // count total to be able to scale volumes in mixing
  uint16_t sound_channel_volume_sum = 0;
  for (uint8_t i = 0; i < SOUND_MAX_CHANNELS; i++) {
    ch = sound_get_channel(c);
    sound_channel_volume_sum += ch->mix.volume;
  }
  if (sound_channel_volume_sum < 255)
    sound_channel_volume_sum = 255;
  for (uint8_t i = 0; i < SOUND_MAX_CHANNELS; i++) {
    ch->state.volume = 255 * ch->mix.volume / sound_channel_volume_sum;
  }
}

void sound_channel_set_pan(uint8_t c, int8_t pan) {
  SoundChannel *ch = sound_get_channel(c);
  ch->mix.pan = pan;
}

void sound_channel_set_porta(uint8_t c, uint16_t porta) {
  SoundChannel *ch = sound_get_channel(c);
  ch->fx.porta.value = porta;
}

void sound_channel_set_delay(uint8_t c, uint8_t delay) {
  SoundChannel *ch = sound_get_channel(c);
  ch->mix.delay = delay;
}

void sound_channel_set_hz(uint8_t c, float hz) {
  SoundChannel *ch = sound_get_channel(c);
  uint32_t newspeed = ch->fx.porta.jump_target;
  if (hz < 20) {
#if SOUND_ADRS_ENABLED == 1
    if (ch->adsr.state != ADSR_RELEASING && ch->adsr.state != ADSR_OFF) {
      ch->adsr.state = ADSR_RELEASING;
      ch->adsr.release_time = sound_time;
    }
    // sound_adsr_channel_start_time[ch] = sound_time;
#else
    ch->waveform.jump = 0;
    ch->fx.porta.jump_target = 0;
#endif
  } else {
    newspeed = finetune * 256.0 * 256.0 * SOUND_WAVEFORM_MAX_LENGTH * hz /
               (SOUND_BITRATE * SOUND_WAVEFORM_MAX_LENGTH);
#if SOUND_ADRS_ENABLED == 1
    if (ch->adsr.state != ADSR_ATTACKING && ch->adsr.state != ADSR_DECAYING) {
      ch->adsr.state = ADSR_ATTACKING;
    }
#endif

    ch->waveform.hz = hz;

    if (ch->fx.porta.value) {
      ch->fx.porta.jump_start = ch->waveform.jump;
      ch->fx.porta.start_time = sound_time;
    }
    ch->fx.porta.jump_target = newspeed;
  }
}

void sound_channel_set_waveform(uint8_t c, uint8_t w) {
  SoundChannel *ch = sound_get_channel(c);
  SoundWaveform *waveform = &waveforms[w];
  ch->waveform.samples = waveform->samples;
}
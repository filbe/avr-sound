

#include "sound.h"
#include <stdio.h>

#define PORTA_OFF 0
#define PORTA_ON 1

uint8_t sound_channel_porta_state[SOUND_MAX_CHANNELS];
extern uint16_t sound_channel_waveform_sample_jump_start[SOUND_MAX_CHANNELS];
extern uint16_t sound_channel_waveform_sample_jump_target[SOUND_MAX_CHANNELS];
uint32_t sound_porta_channel_start_time[SOUND_MAX_CHANNELS];

uint32_t last_d = 0;

void sound_fx_porta_tick() {

  for (uint8_t ch = 0; ch < SOUND_MAX_CHANNELS; ch++) {
    // porta notes
    if (sound_channel_porta[ch]) {
      float time_factor =
          (float)(sound_time - sound_porta_channel_start_time[ch]) /
          ((float)(sound_channel_porta[ch]));

      if (time_factor < 1.0 && time_factor > 0.0) {
        sound_channel_waveform_sample_jump[ch] =
            sound_channel_waveform_sample_jump_start[ch] * (1.0 - time_factor) +
            sound_channel_waveform_sample_jump_target[ch] * time_factor;
      } else {
        if (sound_channel_waveform_sample_jump_start[ch] ==
            sound_channel_waveform_sample_jump_target[ch]) {
          sound_porta_channel_start_time[ch] = sound_time;
        }

        //  default, no porta
        sound_channel_waveform_sample_jump[ch] =
            sound_channel_waveform_sample_jump_target[ch];
      }

    } else {
      //  non-porta notes
      sound_channel_waveform_sample_jump[ch] =
          sound_channel_waveform_sample_jump_target[ch];
      sound_porta_channel_start_time[ch] = sound_time;
    }
  }
}

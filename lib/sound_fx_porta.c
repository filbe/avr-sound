

#include "sound.h"
#include <stdio.h>

#define PORTA_OFF 0
#define PORTA_ON 1

void sound_fx_porta_tick() {

  for (uint8_t ch_num = 0; ch_num < SOUND_MAX_CHANNELS; ch_num++) {
    SoundChannel *ch = &channels[ch_num];
    // porta notes
    if (ch->fx.porta.value) {
      float time_factor = (float)(sound_time - ch->fx.porta.start_time) /
                          ((float)(ch->fx.porta.value));

      if (time_factor < 1.0 && time_factor > 0.0) {
        ch->waveform.jump = ch->fx.porta.jump_start * (1.0 - time_factor) +
                            ch->fx.porta.jump_target * time_factor;
      } else {
        if (ch->fx.porta.jump_start == ch->fx.porta.jump_target) {
          ch->fx.porta.start_time = sound_time;
        }

        //  default, no porta
        ch->waveform.jump = ch->fx.porta.jump_target;
      }

    } else {
      //  non-porta notes
      ch->waveform.jump = ch->fx.porta.jump_target;
      ch->fx.porta.start_time = sound_time;
    }
  }
}

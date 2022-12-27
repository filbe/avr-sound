#include "sound.h"
#include <math.h>
#include <portaudio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>

void sound_init() {

  for (uint8_t i = 0; i < SOUND_MAX_CHANNELS; i++) {
    SoundChannel *ch = sound_get_channel(i);
    sound_channel_set_waveform(i, 0);
    sound_channel_set_volume(i, 0);
    sound_channel_set_pan(i, 0);
    sound_channel_set_delay(i, 0);
    ch->adsr.state = ADSR_OFF;
  }

  sound_fx_delay_init();
  sound_buffer_external_init();
}
#include "sound.h"
#include <math.h>
#include <portaudio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>

void sound_init() {

  for (uint8_t i = 0; i < SOUND_MAX_CHANNELS; i++) {
    sound_channel_set_waveform(i, 0);
    sound_channel_set_volume(i, 255);
    sound_channel_set_pan(i, 0);
    sound_adsr_channel_state[i] = ADSR_OFF;
  }

  for (uint32_t i = 0; i < sound_mixer_delay_buffer_length; i++) {
    sound_mixer_delay_buffer[0][i] = 0;
    sound_mixer_delay_buffer[1][i] = 0;
  }
  sound_buffer_external_init();
}
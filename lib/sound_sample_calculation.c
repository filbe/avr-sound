#include "sound.h"
#include <math.h>
#include <portaudio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>

volatile uint16_t delay_buffer_cursor = 0;
volatile float sound_delay_factor = 0.52;
volatile uint32_t sound_truncate_count = 0;

volatile int16_t sound_process_one_sample(int8_t pan) {

  int32_t _bufsum = 0;
  for (uint8_t i = 0; i < SOUND_MAX_CHANNELS; i++) {
    uint16_t bufspeed = sound_buffer_speed[i];
#if SOUND_ADRS_ENABLED == 1
    if (sound_adsr_channel_state[i] == ADSR_OFF || !bufspeed) {
      continue;
    }
#endif

    int16_t waveform = current_waveform[i];
    uint16_t sample_cursor =
        (sound_buffercursor[i] >> 8) % SOUND_MAXIMUM_SAMPLE_LENGTH;

    int16_t sample = sound_buffer[waveform][sample_cursor];
#if SOUND_ADRS_ENABLED == 1
    uint8_t volume = sound_adsr_channel_state[i] == ADSR_OFF
                         ? _sound_buffer_volume[i]
                         : adsr_volume(i);
#else
    uint8_t volume = _sound_buffer_volume[i];
#endif

    if (volume <= 2)
      volume = 0;
    // if (sample <= 2) sample = 0;

    float pan_factor = (pan ? abs(-127 - sound_buffer_pan[i])
                            : abs(127 - sound_buffer_pan[i])) /
                       254.0;
    _bufsum += sample * volume * pan_factor;
    sound_buffercursor[i] =
        (sound_buffercursor[i] +
         sound_buffer_speed[i] * (1 + sound_truncate_count));
  }
  sound_truncate_count = 0;

  int32_t sample = _bufsum;
  int32_t delay_sample =
      (delay_buffer[pan][delay_buffer_cursor] / 1.3) +
      (delay_buffer[pan][(delay_buffer_cursor + DELAY_BUFFER_LENGTH + 1026) %
                         DELAY_BUFFER_LENGTH] /
           4 +
       96) +
      (delay_buffer[pan][(delay_buffer_cursor + DELAY_BUFFER_LENGTH + 1226) %
                         DELAY_BUFFER_LENGTH] /
           8 +
       112) +
      (delay_buffer[pan][(delay_buffer_cursor + DELAY_BUFFER_LENGTH + 626) %
                         DELAY_BUFFER_LENGTH] /
           4 +
       96) +
      (delay_buffer[pan][(delay_buffer_cursor + DELAY_BUFFER_LENGTH + 777) %
                         DELAY_BUFFER_LENGTH] /
           8 +
       112) +
      (delay_buffer[pan][(delay_buffer_cursor + DELAY_BUFFER_LENGTH + 274) %
                         DELAY_BUFFER_LENGTH] /
           8 +
       112) +
      (delay_buffer[pan][(delay_buffer_cursor + DELAY_BUFFER_LENGTH + 626) %
                         DELAY_BUFFER_LENGTH] /
           4 +
       96) +
      (delay_buffer[pan][(delay_buffer_cursor + DELAY_BUFFER_LENGTH + 1526) %
                         DELAY_BUFFER_LENGTH] /
           8 +
       112);

  delay_buffer[pan][delay_buffer_cursor] =
      ((sample + (delay_sample * sound_delay_factor * 0.3)));
  delay_buffer_cursor++;
  if (delay_buffer_cursor % (DELAY_BUFFER_LENGTH) == 0) {
    delay_buffer_cursor = 0;
  }

  int16_t out = (int16_t)((sample + (delay_sample * sound_delay_factor *
                                     sound_delay_factor)) /
                          2);

  return out;
}
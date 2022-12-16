#include "sound-linux.h"
#include <math.h>
#include <portaudio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>

volatile int sound_fill_buffer(const void *inputBuffer, void *outputBuffer,
                               unsigned long framesPerBuffer,
                               const PaStreamCallbackTimeInfo *timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void *userData) {
  int8_t *out = (float *)outputBuffer;
  unsigned long i;

  (void)timeInfo; /* Prevent unused variable warnings. */
  (void)statusFlags;
  (void)inputBuffer;

  for (i = 0; i < framesPerBuffer; i++) {
    *out++ = sound_process_one_sample(0); /* left */
    *out++ = sound_process_one_sample(1); /* right */
    sound_time++;
  }
  return paContinue;
}
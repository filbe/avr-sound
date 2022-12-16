#include "sound-linux.h"
#include <math.h>
#include <portaudio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>

volatile int8_t pabuf;

static void StreamFinished() { printf("Stream Completed!\n"); }

#define FRAMES_PER_BUFFER (256)

void init_buffer_library() {
  PaStreamParameters outputParameters;
  PaStream *stream;
  PaError err;
  int i;
  err = Pa_Initialize();
  if (err != paNoError)
    goto error;

  outputParameters.device =
      Pa_GetDefaultOutputDevice(); /* default output device */
  if (outputParameters.device == paNoDevice) {
    fprintf(stderr, "Error: No default output device.\n");
    goto error;
  }
  outputParameters.channelCount = 2; /* stereo output */
  outputParameters.sampleFormat = paInt8;
  outputParameters.suggestedLatency =
      Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(&stream, NULL, /* no input */
                      &outputParameters, SOUND_BITRATE, FRAMES_PER_BUFFER,
                      paClipOff, /* we won't output out of range samples so
                                    don't bother clipping them */
                      sound_fill_buffer, &pabuf);
  if (err != paNoError)
    goto error;

  err = Pa_SetStreamFinishedCallback(stream, &StreamFinished);
  if (err != paNoError)
    goto error;

  err = Pa_StartStream(stream);
  if (err != paNoError)
    goto error;

  return;
error:
  Pa_Terminate();
  fprintf(stderr, "An error occured while using the portaudio stream\n");
  fprintf(stderr, "Error number: %d\n", err);
  fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
  return;
}

void sound_init() {

  for (uint8_t i = 0; i < SOUND_MAX_CHANNELS; i++) {
    sound_set_waveform(i, 0);
    sound_set_volume(i, 255);
    sound_set_pan(i, 0);
    sound_channel_adsr_state[i] = ADSR_OFF;
  }

  for (uint32_t i = 0; i < DELAY_BUFFER_LENGTH; i++) {
    delay_buffer[0][i] = 0;
    delay_buffer[1][i] = 0;
  }
  init_buffer_library();
}
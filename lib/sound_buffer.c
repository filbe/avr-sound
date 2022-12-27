#include "sound.h"
#include <math.h>
#include <portaudio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>

int16_t pabuf;

static void StreamFinished() { printf("Stream Completed!\n"); }
#define FRAMES_PER_BUFFER (64)
void sound_buffer_external_init() {
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
  outputParameters.sampleFormat = paInt16;
  outputParameters.suggestedLatency =
      Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency / 4;
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

int sound_fill_buffer(const void *inputBuffer, int16_t *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo *timeInfo,
                      PaStreamCallbackFlags statusFlags, void *userData) {
  int16_t *out = (int16_t *)outputBuffer;
  unsigned long i;

  (void)timeInfo; /* Prevent unused variable warnings. */
  (void)statusFlags;
  (void)inputBuffer;

  for (i = 0; i < framesPerBuffer; i++) {
    *out++ = sound_process_one_sample(0); /* left */
    *out++ = sound_process_one_sample(1); /* right */
    sound_time_tick();
  }
  return paContinue;
}
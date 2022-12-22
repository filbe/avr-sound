/*
 * Universal Synth
 * Authors:
 *  Ville-Pekka Lahti <ville-pekka.lahti@hotmail.com>
 */

#include "sound.h"
#include <math.h>
#include <portaudio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>

uint16_t sound_buffercursor[SOUND_MAX_CHANNELS];
float sound_buffer_jump = 1 >> 8;
uint16_t sound_buffer_speed[SOUND_MAX_CHANNELS];

uint8_t sound_buffer_volume[SOUND_MAX_CHANNELS];
uint8_t _sound_buffer_volume[SOUND_MAX_CHANNELS];
int8_t sound_buffer_pan[SOUND_MAX_CHANNELS];

uint16_t sound_buffer_volume_sum = 0;

float sound_buffer_hz = 440.0;
uint16_t sound_buffer_len = SOUND_MAXIMUM_SAMPLE_LENGTH;
int16_t sound_buffer[SOUND_MAX_CHANNELS][SOUND_MAXIMUM_SAMPLE_LENGTH];
float finetune = 1;

uint64_t sound_time;
int16_t current_waveform[SOUND_MAX_CHANNELS];
int16_t delay_buffer[2][DELAY_BUFFER_LENGTH];

int16_t sound_buffer_out[SOUND_BUFFER_LENGTH];
uint16_t sound_buffer_out_cursor = 0;

void sound_sample_init(uint16_t sample_len, float hz) {
  // Forcing maximum sample length if trying bigger
  if (sample_len > SOUND_MAXIMUM_SAMPLE_LENGTH) {
    sample_len = SOUND_MAXIMUM_SAMPLE_LENGTH;
  }
  sound_buffer_hz = sample_len;
}

void sound_setbuffer(uint8_t waveform, uint16_t index, int8_t value) {
  sound_buffer[waveform][index] = value;
}

void sound_finetune(uint16_t tune) {
  finetune = pow(2, ((tune - 127.5) / 127.5));
}

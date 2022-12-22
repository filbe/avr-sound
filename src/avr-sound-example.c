/*
 * Universal Synth
 * Authors:
 *  Ville-Pekka Lahti <ville-pekka.lahti@hotmail.com>
 */

#include "sound.h"
#include <math.h>
#include <time.h>
#include <unistd.h>

uint8_t song[] = {
    64, 65, 0,  64, 0,  62, 69, 69, 69, 69, 69, 69, 69, 0,  67, 65,
    67, 67, 0,  72, 72, 67, 76, 76, 76, 76, 76, 76, 76, 0,  74, 76,

    76, 77, 77, 76, 76, 74, 69, 69, 69, 69, 69, 69, 69, 0,  67, 65,
    67, 67, 67, 67, 67, 0,  74, 76, 77, 76, 74, 69, 67, 65, 64, 60,
};

uint8_t song2[] = {
    50, 0, 0, 50, 0, 50, 50, 0, 0,  0, 62, 64, 65, 69, 74, 77,
    48, 0, 0, 48, 0, 48, 48, 0, 0,  0, 60, 62, 64, 69, 72, 76,

    46, 0, 0, 46, 0, 46, 46, 0, 0,  0, 58, 60, 62, 65, 70, 74,
    48, 0, 0, 48, 0, 48, 48, 0, 60, 0, 57, 0,  55, 0,  48, 0,
};

uint8_t song3[] = {
    86, 88, 89, 93, 86, 88, 89, 93, 86, 88, 89, 93, 86, 88, 89, 93,
    84, 86, 88, 91, 84, 86, 88, 91, 84, 86, 88, 91, 84, 86, 88, 91,

    82, 84, 86, 89, 82, 84, 86, 89, 82, 84, 86, 89, 82, 84, 86, 89,
    84, 86, 88, 91, 84, 86, 88, 91, 84, 86, 88, 91, 84, 86, 88, 91,
};

float midi[110];

int main() {

  for (float i = 0; i < 110; i += 1.0) {
    midi[(uint8_t)(i)] = pow(2.0, (i - 12.0 - 69.0) * 0.083333) * 440.0;
  }

  sound_init();
  uint16_t samlen = SOUND_WAVEFORM_MAX_LENGTH;

  for (int16_t b = 0; b < samlen; b++) {
    // Buffer range is -128....127
    sound_set_waveform_sample(
        0, b,
        (sin(b * M_PI / (float)(samlen)) * 30.0 +
         sin(2 * b * M_PI / (float)(samlen)) * 32.0 +
         sin(4 * b * M_PI / (float)(samlen)) * 20.0 +
         sin(8 * b * M_PI / (float)(samlen)) * 15.0 +
         sin(16 * b * M_PI / (float)(samlen)) * 13.0)); // organ

    sound_set_waveform_sample(1, b, b + (b / 4)); // SAWTOOTH

    sound_set_waveform_sample(2, b, sin(b / (float)(samlen)*M_PI) * 127);

    // sound_set_waveform_sample(1, b, b < samlen / 2 ? -128 : 127); // SQUARE
    // WAVE
  }

  uint32_t c = 0;
  sound_channel_set_waveform(0, 0);
  sound_channel_set_waveform(1, 1);
  sound_channel_set_waveform(2, 2);

  sound_channel_set_volume(0, 205);
  sound_channel_set_volume(1, 255);
  sound_channel_set_volume(2, 255);

  sound_set_adsr(0, 400, 400, 255, 10000);
  sound_set_adsr(1, 100, 1600, 70, 5500);
  sound_set_adsr(2, 50, 50, 64, 2500);

  sound_channel_set_pan(0, 0);
  sound_channel_set_pan(1, 0);
  sound_channel_set_pan(2, 0);

  while (1) {

    float hz1 = midi[song[(c / 2) % sizeof(song)]];
    float hz2 = midi[song2[(c / 2) % sizeof(song2)]];
    float hz3 = midi[song3[(c / 2) % sizeof(song3)]];
    // float hz4 = midi[song4[c % sizeof(song4)]];

    if (c % 2 == 0) {
      sound_channel_set_hz(0, hz1);
      sound_channel_set_hz(1, hz2 / 2);
      sound_channel_set_hz(2, hz3);
    } else {
      sound_channel_set_hz(0, hz1);
      sound_channel_set_hz(1, 0);
      sound_channel_set_hz(2, 0);
    }

    sound_channel_set_pan(2, sin(c * M_PI / sizeof(song)) * 126);

    sound_set_adsr(2, cos(c * M_PI / sizeof(song)) * 126 + 150,
                   cos(c * M_PI / sizeof(song)) * 126 + 150, 64,
                   2500 + sin(2 * c * M_PI / sizeof(song)) * 1026);

    sound_mixer_fx_master_delay_factor =
        0.60 + sin(-M_PI / 3 + (c * M_PI / sizeof(song))) * 0.2;

    c++;
    usleep(55000);

    if ((c) % (sizeof(song) * 2 * 4) == 0) {
      sound_channel_set_hz(0, 0);
      sound_channel_set_hz(1, 0);
      sound_channel_set_hz(2, 0);
      // sound_mixer_fx_master_delay_factor = 0.75;
      usleep(2000000);
    }
  }
  return 0;
}
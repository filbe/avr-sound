#include "sound.h"

#define SOUND_FX_CHS ((SOUND_MAX_CHANNELS + 1) * 2)
#define BUF_LEN 20000

int16_t fx_buffer[SOUND_FX_CHS][BUF_LEN];
float feedback_factor[SOUND_FX_CHS];

int16_t fx_delay_buf_cur[SOUND_FX_CHS];

void sound_fx_reverb_init() {
  memset(&fx_buffer, 0, sizeof(int16_t) * SOUND_FX_CHS * BUF_LEN);
  for (uint8_t i = 0; i < SOUND_FX_CHS; i++) {
    feedback_factor[i] = 0;
    fx_delay_buf_cur[i] = 0;
  }
}

void sound_fx_reverb_init_ch(uint8_t ch) {
  memset(&fx_buffer[ch], 0, sizeof(int16_t) * BUF_LEN);
}

void sound_fx_reverb_set_feedback_factor(uint8_t ch, float f) {
  sound_fx_reverb_init_ch(ch);
  feedback_factor[ch] = f;
}

#define REVERB_COMPLEXITY 5
#define COMPONENT_FACTOR (1.0 / REVERB_COMPLEXITY)

int16_t sound_fx_reverb_feed(uint8_t ch, int16_t sample) {
  int16_t wet = sample;
  for (uint8_t i = 0; i < REVERB_COMPLEXITY; i++) {
    wet += fx_buffer[ch][(uint16_t)((fx_delay_buf_cur[ch] +
                                     (i * BUF_LEN / COMPONENT_FACTOR))) %
                         BUF_LEN] *
           COMPONENT_FACTOR;
  }
  fx_buffer[ch][fx_delay_buf_cur[ch]] = (wet * feedback_factor[ch]);
  // printf("%d\n", fx_buffer[ch][fx_delay_buf_cur[ch]]);
  //  mix feedback and save back to buffer
  int16_t mix_feedback = fx_buffer[ch][fx_delay_buf_cur[ch]];
  // increase buffer cursor
  fx_delay_buf_cur[ch] = (fx_delay_buf_cur[ch] + 1) % BUF_LEN;
  // return mix
  return mix_feedback;
}

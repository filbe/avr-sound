
#ifndef __SOUND_FX_DELAY_H__
#define __SOUND_FX_DELAY_H__

void sound_fx_delay_init();
int16_t sound_fx_delay_feed(uint8_t ch, int16_t sample);

void sound_fx_delay_set_feedback_factor(uint8_t ch, float f);

#endif
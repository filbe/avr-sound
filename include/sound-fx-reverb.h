
#ifndef __SOUND_FX_REVERB_H__
#define __SOUND_FX_REVERB_H__

void sound_fx_reverb_init();
int16_t sound_fx_reverb_feed(uint8_t ch, int16_t sample);

void sound_fx_reverb_set_feedback_factor(uint8_t ch, float f);

#endif
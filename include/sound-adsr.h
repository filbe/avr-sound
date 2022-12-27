#ifndef __SOUND_ADSR_H__
#define __SOUND_ADSR_H__

#include "sound.h"
#include <stdint.h>
#define ADSR_ATTACKING 0
#define ADSR_DECAYING 1
#define ADSR_SUSTAINING 2
#define ADSR_RELEASING 3
#define ADSR_OFF 4

uint16_t adsr_volume(uint8_t channel);

void sound_set_adsr(uint8_t c, uint16_t attack, uint16_t decay,
                    uint8_t sustain, uint16_t release);

#endif
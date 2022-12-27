#ifndef __SOUND_FX_PORTA_TYPES_H__
#define __SOUND_FX_PORTA_TYPES_H__

typedef struct SoundFxPorta {
  uint16_t jump_start;
  uint16_t jump_target;
  uint16_t value;
  uint32_t start_time;
} SoundFxPorta;

#endif
/**
 *          .--.
    .-._;.--.;_.-.
   (_.'_..--.._'._)
    /.' .  . . '.\
   // .      / . \\
  |; .      /   . |;
  || -    ()    - ||
  |; .          . |;
   \\ .        . //
    \'._'    '_.'/
     '-._'--'_.-'
         `""`
 *
 */

#include "sound-fx-porta.h"
#include "sound-time.h"
#include "sound.h"
#include <stdbool.h>

uint64_t sound_time;

void sound_time_tick() {
  sound_fx_porta_tick();
  sound_time++;
}
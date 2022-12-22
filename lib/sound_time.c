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

#include "sound-time.h"
#include "sound.h"

uint64_t sound_time;

void sound_time_tick() { sound_time++; }
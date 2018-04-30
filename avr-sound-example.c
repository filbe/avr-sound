#include "avr-sound.h"
#include <math.h>
#include <util/delay.h>
int main() {
	avrsound_init();

	
	sei();

	avrsound_set_pcm_length(1024);

	float a=0;

	avrsound_buffer_clean();

	float c = 0;
	DDRB = 0;
	while(1) {
		a=0;
		for (uint16_t b=0;b<1024;b++) {
			avrsound_buffer_write_ring((sin(a)*127.5+127.5));
			a+=0.1;
		}
		c = c + 3.1;
		_delay_ms(50);
	}
	return 0;
}
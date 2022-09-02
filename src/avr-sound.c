/*
 * AVR Sound library
 * avr-sound-example.c
 * 
 * Simple waveform generation example. You will need a R-R2 Resistor ladder network to get n-bit sound.
 * Tested on AtMega328P-PU with bitrates of 1kHz and 40kHz @ 16MHz external crystal oscillator
 * If you want to use this with internal 8MHz oscillator, remember to change fuses in Makefile.
 * Fuses works straight on Arduino Uno board, but feel free to test it standalone as well (with custom fuses).
 * 
 * Authors:
 *  Ville-Pekka Lahti <ville-pekka.lahti@hotmail.com>
 */

#include <avr-sound.h>

volatile uint16_t avrsound_buffercursor[AVRSOUND_MAX_CHANNELS];
volatile float avrsound_buffer_jump = 1 >> 8;
volatile uint16_t avrsound_buffer_speed[AVRSOUND_MAX_CHANNELS];

volatile uint8_t avrsound_buffer_volume[AVRSOUND_MAX_CHANNELS];
volatile uint8_t _avrsound_buffer_volume[AVRSOUND_MAX_CHANNELS];

volatile uint16_t avrsound_buffer_volume_sum = 0;

volatile float avrsound_buffer_hz = 440.0;
volatile uint16_t avrsound_buffer_len = AVRSOUND_MAXIMUM_SAMPLE_LENGTH;
volatile int8_t avrsound_buffer[AVRSOUND_MAX_CHANNELS][AVRSOUND_MAXIMUM_SAMPLE_LENGTH];
volatile float finetune = 1;

volatile uint32_t time = 0;
volatile uint8_t current_waveform[AVRSOUND_MAX_CHANNELS];

void avrsound_init() {

	for (uint8_t i=0;i<AVRSOUND_MAX_CHANNELS;i++) {
		avrsound_set_waveform(i,0);
		avrsound_set_volume(i,255);
	}

	if (AVRSOUND_ENDIANESS == AVRSOUND_BIG_ENDIAN) {
		AVRSOUND_DDR |= AVRSOUND_PINMASK_BE;
	} else {
		AVRSOUND_DDR |= AVRSOUND_PINMASK_LE;
	}

	TCCR1B |= (1 << WGM13) | (1 << WGM12);
	ICR1 = F_CPU / AVRSOUND_BITRATE / AVRSOUND_PCM_SPEED_SCALE - 1;
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << CS11);
	sei();
}

void avrsound_set_volume(uint8_t channel, uint8_t volume) {
	avrsound_buffer_volume[channel] = volume;

	// count total to be able to scale volumes in mixing
	avrsound_buffer_volume_sum = 0;
	for (uint8_t i=0;i<AVRSOUND_MAX_CHANNELS;i++) {
		avrsound_buffer_volume_sum += avrsound_buffer_volume[i];
	}
	if (avrsound_buffer_volume_sum < 255) avrsound_buffer_volume_sum = 255;
	for (uint8_t i=0;i<AVRSOUND_MAX_CHANNELS;i++) {
		_avrsound_buffer_volume[i] = 255 * avrsound_buffer_volume[i] / avrsound_buffer_volume_sum;
	}
}

void avrsound_sample_init(uint16_t sample_len, float hz) {
	// Forcing maximum sample length if trying bigger
	if (sample_len > AVRSOUND_MAXIMUM_SAMPLE_LENGTH) {
		sample_len = AVRSOUND_MAXIMUM_SAMPLE_LENGTH;
	}
	avrsound_buffer_len = sample_len;

	//avrsound_buffer_jump = hz*1080.17/sample_len;
	avrsound_buffer_hz = sample_len;
}

void avrsound_setbuffer(uint8_t waveform, uint16_t index, int8_t value) {
	avrsound_buffer[waveform][index] = value;
}

void avrsound_set_hz(uint8_t channel, float hz) {
	if (hz < 20) 
		avrsound_buffer_speed[channel] = 0;
	else
		avrsound_buffer_speed[channel] = finetune*256.0 * 256.0 * avrsound_buffer_len * hz / (AVRSOUND_BITRATE * avrsound_buffer_hz);
}

void avrsound_set_waveform(uint8_t channel, uint8_t waveform) {
	current_waveform[channel] = waveform;
}

void avrsound_finetune(uint16_t tune) {
	finetune = pow(2,((tune-127.5) / 127.5));
}

ISR (TIMER1_COMPA_vect) 
{
	int16_t _bufsum = 0;
	for (uint8_t i=0;i<AVRSOUND_MAX_CHANNELS;i++) {
		if (avrsound_buffer_speed[i] < 10) continue;
		int8_t sample = avrsound_buffer[current_waveform[i]][(avrsound_buffercursor[i] >> 8) % AVRSOUND_MAXIMUM_SAMPLE_LENGTH];
		uint8_t volume = _avrsound_buffer_volume[i];
		_bufsum += (int16_t)((sample + 128) * volume);
		avrsound_buffercursor[i] = (avrsound_buffercursor[i] + avrsound_buffer_speed[i]);
	}

	AVRSOUND_PORT = (uint8_t)(_bufsum >> 8);
	time++;
}
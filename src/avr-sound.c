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

#define ADSR_UNRELEASED 0
#define ADSR_RELEASED   1
#define ADSR_OFF        2

volatile uint16_t avrsound_buffercursor[AVRSOUND_MAX_CHANNELS];
volatile float avrsound_buffer_jump = 1 >> 8;
volatile uint16_t avrsound_buffer_speed[AVRSOUND_MAX_CHANNELS];

volatile uint8_t avrsound_buffer_volume[AVRSOUND_MAX_CHANNELS];
volatile uint8_t _avrsound_buffer_volume[AVRSOUND_MAX_CHANNELS];
volatile uint32_t avrsound_channel_start_time[AVRSOUND_MAX_CHANNELS];
volatile uint32_t avrsound_channel_release_time[AVRSOUND_MAX_CHANNELS];
volatile uint8_t avrsound_channel_adsr_state[AVRSOUND_MAX_CHANNELS];

volatile uint16_t avrsound_channel_adsr_attack[AVRSOUND_MAX_CHANNELS];
volatile uint16_t avrsound_channel_adsr_decay[AVRSOUND_MAX_CHANNELS];
volatile uint8_t avrsound_channel_adsr_sustain[AVRSOUND_MAX_CHANNELS];
volatile uint16_t avrsound_channel_adsr_release[AVRSOUND_MAX_CHANNELS];
volatile uint32_t avrsound_channel_adsr_attack_flip[AVRSOUND_MAX_CHANNELS];
volatile uint32_t avrsound_channel_adsr_decay_flip[AVRSOUND_MAX_CHANNELS];
volatile uint32_t avrsound_channel_adsr_release_flip[AVRSOUND_MAX_CHANNELS];

volatile uint16_t avrsound_buffer_volume_sum = 0;

volatile float avrsound_buffer_hz = 440.0;
volatile uint16_t avrsound_buffer_len = AVRSOUND_MAXIMUM_SAMPLE_LENGTH;
volatile int8_t avrsound_buffer[AVRSOUND_MAX_CHANNELS][AVRSOUND_MAXIMUM_SAMPLE_LENGTH];
volatile float finetune = 1;

volatile uint64_t time = 0;
volatile uint8_t current_waveform[AVRSOUND_MAX_CHANNELS];

void avrsound_init() {

	for (uint8_t i=0;i<AVRSOUND_MAX_CHANNELS;i++) {
		avrsound_set_waveform(i,0);
		avrsound_set_volume(i,255);
		avrsound_channel_adsr_state[i] = ADSR_OFF;
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
	avrsound_buffer_hz = sample_len;
}

void avrsound_setbuffer(uint8_t waveform, uint16_t index, int8_t value) {
	avrsound_buffer[waveform][index] = value;
}

void avrsound_set_hz(uint8_t channel, float hz) {
	uint32_t newspeed = avrsound_buffer_speed[channel];
	if (hz < 20) {
		#if AVRSOUND_ADRS_ENABLED == 1
			if (avrsound_channel_adsr_state[channel] == ADSR_UNRELEASED) {
				avrsound_channel_adsr_state[channel] = ADSR_RELEASED;
				avrsound_channel_release_time[channel] = time;
			}
			avrsound_channel_start_time[channel] = time;
		#else
			avrsound_buffer_speed[channel] = 0;
		#endif
	}
	else {
		#if AVRSOUND_ADRS_ENABLED == 1
		avrsound_channel_adsr_state[channel] = ADSR_UNRELEASED;
		#endif
		newspeed = finetune*256.0 * 256.0 * avrsound_buffer_len * hz / (AVRSOUND_BITRATE * avrsound_buffer_hz);
		if (avrsound_buffer_speed[channel] != newspeed) {
			avrsound_channel_start_time[channel] = time;
		}
		avrsound_buffer_speed[channel] = newspeed;
	}

}

void avrsound_set_waveform(uint8_t channel, uint8_t waveform) {
	current_waveform[channel] = waveform;
}

void avrsound_finetune(uint16_t tune) {
	finetune = pow(2,((tune-127.5) / 127.5));
}

void avrsound_set_adsr(uint8_t channel, uint16_t attack, uint16_t decay, uint8_t sustain, uint16_t release) {
	#if AVRSOUND_ADRS_ENABLED == 1
	avrsound_channel_adsr_attack[channel] = attack;
	avrsound_channel_adsr_decay[channel] = decay;
	avrsound_channel_adsr_sustain[channel] = sustain;
	avrsound_channel_adsr_release[channel] = release;

	avrsound_channel_adsr_attack_flip[channel] = 256.0 * 65536.0 / (float)(attack);
	avrsound_channel_adsr_decay_flip[channel] = 256.0 * 65536.0 / (float)(decay);
	avrsound_channel_adsr_release_flip[channel] = 256.0 * 65536.0 / (float)(release);
	#endif
}


uint16_t adsr_volume(uint8_t channel) {
	uint16_t max_volume = _avrsound_buffer_volume[channel];
	uint32_t start_time = avrsound_channel_start_time[channel];
	uint32_t release_time = avrsound_channel_release_time[channel];


	uint32_t adsr_a = avrsound_channel_adsr_attack[channel];
	uint32_t adsr_d = avrsound_channel_adsr_decay[channel];
	uint32_t adsr_s = avrsound_channel_adsr_sustain[channel];
	uint32_t adsr_r = avrsound_channel_adsr_release[channel];

	uint32_t adsr_a_flip = avrsound_channel_adsr_attack_flip[channel];
	uint32_t adsr_d_flip = avrsound_channel_adsr_decay_flip[channel];
	uint32_t adsr_r_flip = avrsound_channel_adsr_release_flip[channel];

	uint32_t time_diff = time - start_time;
	uint32_t time_diff_release = time - release_time;
	uint32_t adsr_compare_val = adsr_a;
	uint32_t time_begin_part = 0;

	if (avrsound_channel_adsr_state[channel] == ADSR_UNRELEASED) {
		if (time_diff < adsr_a) {
			// attack
			return max_volume * time_diff * adsr_a_flip >> 24;
		} else if (time_diff < adsr_a + adsr_d) {
			// decay is falling line
			time_begin_part = time_diff - adsr_a;
			return max_volume - ((max_volume - adsr_s) * time_begin_part * adsr_d_flip >> 24);
		} else {
			return adsr_s;
		}
	} else if (avrsound_channel_adsr_state[channel] == ADSR_RELEASED) {
		if (time_diff_release < adsr_r) {
			// release
			return (adsr_r - time_diff_release) * adsr_s * adsr_r_flip >> 24;
		} else {
			avrsound_channel_adsr_state[channel] = ADSR_OFF;
			avrsound_buffer_speed[channel] = 0;
			return 0;
		}
	}
	
	//avrsound_buffer_speed[channel] = 0;
	return max_volume;
}

ISR (TIMER1_COMPA_vect) 
{
	PORTB = 255;
	int16_t _bufsum = 0;
	for (uint8_t i=0;i<AVRSOUND_MAX_CHANNELS;i++) {
		uint8_t bufspeed = avrsound_buffer_speed[i];
		if (avrsound_channel_adsr_state[i] == ADSR_OFF || !bufspeed) {
			continue;
		}

		int8_t waveform = current_waveform[i];
		uint8_t sample_cursor = (avrsound_buffercursor[i] >> 8) % AVRSOUND_MAXIMUM_SAMPLE_LENGTH;

		int8_t sample = avrsound_buffer[waveform][sample_cursor];
		#if AVRSOUND_ADRS_ENABLED == 1
		uint8_t volume = avrsound_channel_adsr_state[i] == ADSR_OFF ? _avrsound_buffer_volume[i] : adsr_volume(i);
		#else
		uint8_t volume = _avrsound_buffer_volume[i];
		#endif
		_bufsum += sample * volume;
		avrsound_buffercursor[i] = (avrsound_buffercursor[i] + avrsound_buffer_speed[i]);
	}

	AVRSOUND_PORT = (_bufsum >> 8) - 128;
	
	time++;
	PORTB = 0;
}
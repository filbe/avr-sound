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
#include <avr/pgmspace.h>
//#include "compress_table.h"

volatile uint16_t avrsound_buffercursor[AVRSOUND_MAX_CHANNELS];
volatile float avrsound_buffer_jump = 1 >> 8;
volatile uint16_t avrsound_buffer_speed[AVRSOUND_MAX_CHANNELS];
volatile uint16_t avrsound_buffer_speed_target[AVRSOUND_MAX_CHANNELS];
volatile float avrsound_buffer_hz = 440.0;
volatile uint16_t avrsound_buffer_len = 256;
volatile int16_t avrsound_buffer[AVRSOUND_MAXIMUM_SAMPLE_LENGTH];
volatile float finetune = 1;
volatile uint16_t _avrsound_samplerate;
volatile uint16_t avrsound_buffer_volume[AVRSOUND_MAX_CHANNELS];
volatile uint8_t avrsound_buffer_volume_additive[AVRSOUND_MAX_CHANNELS];
volatile uint8_t avrsound_using_volume = 0;

void avrsound_init()
{

	if (AVRSOUND_ENDIANESS == AVRSOUND_BIG_ENDIAN) {
		AVRSOUND_DDR |= AVRSOUND_PINMASK_BE;
	} else {
		AVRSOUND_DDR |= AVRSOUND_PINMASK_LE;
	}

	TCCR1B |= (1 << WGM13) | (1 << WGM12);
	ICR1 = F_CPU / AVRSOUND_BITRATE / AVRSOUND_PCM_SPEED_SCALE - 1;
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << CS11);

	OCR2A |= 190; // about 100 times per sec
	TCCR2A |= WGM21;
	TCCR2B |= CS22 | CS21 | CS20;
	TIMSK2 |= (1 << OCIE2A);


	sei();

	for (uint8_t i = 0; i < AVRSOUND_MAX_CHANNELS; i++) {
		avrsound_buffer_volume[i] = 255;
	}
}

void avrsound_sample_init(uint16_t sample_len, float hz)
{
	// Forcing maximum sample length if trying bigger
	if (sample_len > AVRSOUND_MAXIMUM_SAMPLE_LENGTH) {
		sample_len = AVRSOUND_MAXIMUM_SAMPLE_LENGTH;
	}
	avrsound_buffer_len = sample_len;

	//avrsound_buffer_jump = hz*1080.17/sample_len;
	avrsound_buffer_hz = sample_len;
}

void avrsound_setbuffer(uint16_t index, uint8_t value)
{
	avrsound_buffer[index] = value;
}

void avrsound_set_volume(uint8_t channel, uint8_t volume)
{
	avrsound_buffer_volume[channel] = volume;
	avrsound_buffer_volume_additive[channel] = (128 - (128 * volume / 255));
	if (volume == 0) {
		avrsound_using_volume = 0;
	} else {
		avrsound_using_volume = 1;
	}
}

uint8_t avrsound_get_volume(uint8_t channel) {
	return avrsound_buffer_volume[channel];
}

void avrsound_set_hz(uint8_t channel, float hz)
{
	if (hz < 8.5) {
		avrsound_buffer_speed_target[channel] = 0;

	} else {
		avrsound_buffer_speed_target[channel] = finetune * 256.0 * 256.0 * avrsound_buffer_len * hz / (_avrsound_samplerate * avrsound_buffer_hz);
	}

}

void avrsound_finetune(uint16_t tune)
{
	finetune = pow(2, ((tune - 127.5) / 127.5));
}

void avrsound_set_samplerate(uint16_t samplerate)
{
	cli();
	TCCR1B |= (1 << WGM13) | (1 << WGM12);
	ICR1 = F_CPU / samplerate / AVRSOUND_PCM_SPEED_SCALE - 1;
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << CS11);
	_avrsound_samplerate = samplerate;
	sei();
}

uint8_t avrsound_compress(uint16_t sample, uint8_t channels) {
	// compressing and normalizing the result
	return sample;// << channels;//pgm_read_byte(&compress_table[sample+((3-channels) << 7)]);
}






uint16_t abx16(uint16_t a, uint16_t b, uint16_t x) {
	if (b == 65535)
		return 65535;
	if (x < b)
		return (uint32_t)a * (uint32_t)x / b;
	return (((x - b) * (65536 - a)) / (65536 - b)) + a;
}

uint8_t bitcrush(uint8_t sample, uint8_t bits) {
	if (bits >= 8)
		return sample;
	return sample & (0xff << (8 - bits));
}

volatile uint16_t porta_to = 0;
volatile float porta_step = 0;
volatile uint16_t portamod = 1;
volatile uint16_t portacounter = 0;

uint16_t porta(uint16_t from, uint16_t to) {
	portacounter++;
	if (portacounter > portamod) {
		portacounter -= portamod;
		return (from+from+from+to) >> 2;
	}
	return from;
}




uint16_t fuzzyX = 32767;
uint16_t fuzzyY = 32767;
uint16_t fuzzyZ = 0;
uint16_t fuzzyAngle = 0;

uint8_t bitcrush_amount = 8;



ISR (TIMER1_COMPA_vect)
{
	int16_t bufsum = 0;
	uint8_t active_channels = 0;
	for (uint8_t i = 0; i < AVRSOUND_MAX_CHANNELS; i++) {
		avrsound_buffer_speed[i] = porta(avrsound_buffer_speed[i], avrsound_buffer_speed_target[i]);
		if (avrsound_buffer_speed[i] > 0) {
			if (avrsound_using_volume == 1) {
				bufsum += (avrsound_buffer[abx16(fuzzyX, fuzzyY, avrsound_buffercursor[i]) >> 8] * avrsound_buffer_volume[i] >> 8) + avrsound_buffer_volume_additive[i];
			} else {
				bufsum += avrsound_buffer[abx16(fuzzyX, fuzzyY, avrsound_buffercursor[i]) >> 8];
			}

			avrsound_buffercursor[i] = (avrsound_buffercursor[i] + avrsound_buffer_speed[i]);




			active_channels++;
		}
	}

	if (bufsum != 0) {
		AVRSOUND_PORT_LED |= (1 << AVRSOUND_PIN_LED);

		bufsum = bitcrush(avrsound_compress(bufsum, active_channels), bitcrush_amount);

		AVRSOUND_PORT = bufsum;

	} else {
		AVRSOUND_PORT_LED &= ~(1 << AVRSOUND_PIN_LED);
		AVRSOUND_PORT = 128;
	}


}

uint16_t adc_read(uint8_t adcx);

uint16_t adcprescale = 0;
int16_t fuzzydeltaY = 0, fuzzydeltaX = 0;

ISR (TIMER2_COMPA_vect) {
	if (adcprescale % 64 == 0) {
		bitcrush_amount = 8 - ((adc_read(0) >> 7));
		fuzzyX = (adc_read(1)) << 6;
		fuzzyY = (adc_read(2)) << 6;
		portamod = (adc_read(3) >> 2) + 1;
	}
	//fuzzyY += fuzzydeltaY;
	//fuzzyX += fuzzydeltaX;
	adcprescale++;
}

void adc_init()
{
	DDRC &= ~(0x7);
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADMUX = 0;
	ADMUX  |= (1 << REFS0);
	DIDR0 = 0b00011111;
}

uint16_t adc_read(uint8_t ch)
{
	ADMUX = (ch);

	ADCSRA |= (1 << ADSC);

	while (ADCSRA & (1 << ADSC)); {}

	return ADCW;
}
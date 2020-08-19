#include <stdio.h>
#include <string.h>

#include "adpcm_driver.h"
#include "tools.h"

void adpcm_mixer_init(
	struct adpcm_mixer *mixer, int target_rate,
	int16_t *decode_buffer, int decode_buffer_len,
	int16_t *resample_buffer, int resample_buffer_len
) {
	mixer->target_rate = target_rate;
	mixer->decode_buffer = decode_buffer;
	mixer->decode_buffer_len = decode_buffer_len;
	mixer->resample_buffer = resample_buffer;
	mixer->resample_buffer_len = resample_buffer_len;

	for(int i = 0; i < 8; i++) {
		adpcm_init(&mixer->channels[i].decoder_status);
		mixer->channels[i].resampler_state =
			speex_resampler_init(1, 15625, target_rate, 5, 0);
		mixer->channels[i].cur_sample = 0;
		mixer->channels[i].data = 0;
	}
}

static uint16_t adpcm_mixer_calc_vol(uint8_t vol) {
	const uint8_t vol_00_0f[] = {
		0x6B, 0x6F, 0x71, 0x74, 0x76, 0x79, 0x7B, 0x7D,
		0x80, 0x82, 0x84, 0x87, 0x8A, 0x8C, 0x8F, 0x91,
	};
	const uint16_t vol_40_a0[] = {
		5, 6, 6, 7, 7, 8, 9, 10, 10, 11, 12, 14, 15, 16, 18, 20, 21,
		23, 25, 29, 31, 33, 37, 41, 46, 50, 54, 60, 66, 72, 80, 89,
		97, 107, 117, 130, 142, 156, 173, 189, 205, 226, 246, 267,
		308, 328, 369, 410, 431, 492, 533, 594, 656, 717, 799, 861,
		963, 1045, 1147, 1270, 1393, 1536, 1700, 1864, 2048, 2253,
		2479, 2724, 2991, 3298, 3625, 3994, 4383, 4834, 5325, 5837,
		6431, 7087, 7783, 8561, 9442, 10363, 11387, 12555, 13824,
		15217, 16733, 18371, 20255, 22221, 24454, 26932, 29696,
		32768, 36127, 39732, 43541
	};

	if(vol <= 15)
		return vol_40_a0[vol_00_0f[vol] - 0x40];
	if(vol >= 0x40 && vol <= 0xa0)
		return vol_40_a0[vol - 0x40];

	return 0;
}

static int oki_clocks[5] = {
	4000000, 4000000, 4000000, 8000000, 8000000,
};

static int oki_divs[5] = {
	1024, 768, 512, 768, 512
};

int  adpcm_mixer_play(
	struct adpcm_mixer *m, int channel, int16_t *data,
	int len, int freq, int vol, int pan
) {
	if(channel >= 8 || len == 0)
		return -1;

	// printf("adpcm play channel=%d data=%p len=%d freq=%d vol=%d pan=%d\n", channel, data, len, freq, vol, pan);

	struct adpcm_mixer_channel *chan = &m->channels[channel];
	chan->data = data;
	chan->num_samples = len * 2;
	chan->cur_sample = 0;
	chan->volume = adpcm_mixer_calc_vol(vol);
	if(freq > 4) freq = 4;

	int clock = oki_clocks[freq];
	int div = oki_divs[freq];
	int denominator = div * m->target_rate;
	int g = gcd(clock, denominator);
	chan->resample_remainder = 0;
	speex_resampler_set_rate_frac(
		chan->resampler_state, clock / g, denominator / g,
		clock / div, m->target_rate
	);

	adpcm_init(&chan->decoder_status);

	return 0;
}

int adpcm_mixer_stop(struct adpcm_mixer *m, int channel) {
	if(channel >= 8) return -1;

	struct adpcm_mixer_channel *chan = &m->channels[channel];
	chan->data = 0;
	chan->cur_sample = 0;
	chan->num_samples = 0;

	return 0;
}

int adpcm_mixer_set_volume(struct adpcm_mixer *m, int channel, uint8_t vol) {
	if(channel >= 8)
		return -1;
	struct adpcm_mixer_channel *chan = &m->channels[channel];
	chan->volume = adpcm_mixer_calc_vol(vol);

	return 0;
}

static int16_t adpcm_mixer_channel_decode_sample(struct adpcm_mixer_channel *chan) {
	if(chan->cur_sample >= chan->num_samples)
		chan->data = 0;
	if(!chan->data)
		return 0;

	int32_t ret = chan->data[chan->cur_sample++];
//	ret *= chan->volume;
//	ret /= 32767;
	if(ret > 32767) ret = 32767;
	if(ret < -32768) ret = -32768;

	// int shift = chan->cur_sample & 1;
	// uint8_t c = chan->data[chan->cur_sample >> 1];
	// uint8_t in = shift ? (c >> 4) : (c & 0x0f);
	// chan->cur_sample++;
	// return chan->volume * adpcm_decode(in, &chan->decoder_status) / 4096;

	return ret;
}

static int adpcm_channel_resample(
	struct adpcm_mixer_channel *chan,
	int16_t *decode_buffer, int decode_buffer_len,
	int16_t *resample_buffer, int resample_buffer_len,
	uint32_t num_samples
) {
	(void)resample_buffer_len;

	//uint32_t numerator, denominator;
	//speex_resampler_get_ratio(chan->resampler_state, &numerator, &denominator);
	//uint32_t local_numerator = chan->resample_remainder + num_samples * numerator;
	//uint32_t decode_samples = local_numerator / denominator;
//	printf("adpcm_channel_resample num_samples=%d numerator=%d denominator=%d decode_samples=%d\n", num_samples, numerator, denominator, decode_samples);
	for(int i = 0; i < decode_buffer_len; i++) {
		int smpl = adpcm_mixer_channel_decode_sample(chan);
		decode_buffer[i] = (smpl << 3) | (smpl & 0x07);
		//resample_buffer[i] = decode_buffer[i];
//		printf("%d ", decode_buffer[i]);
	}
//	printf("\n");
	uint32_t samples_written = num_samples;
	uint32_t samples_read = decode_buffer_len;
	speex_resampler_process_int(
		chan->resampler_state, 0,
		decode_buffer, &samples_read,
		resample_buffer, &samples_written
	);
	//chan->resample_remainder = local_numerator - samples_read * denominator;
	chan->cur_sample -= decode_buffer_len - samples_read;
	if(chan->cur_sample < 0) chan->cur_sample = 0;
//	printf("num_samples=%d samples_written=%d decode_samples=%d samples_read=%d\n", num_samples, samples_written, decode_samples, samples_read);
	return samples_written;
}

void adpcm_mixer_mix(struct adpcm_mixer *mixer, stream_sample_t *out, int num_samples) {
	// printf("adpcm_mixer_mix num_samples=%d\n", num_samples);
	memset(out, 0, num_samples * sizeof(*out));
	for(int i = 0; i < 8; i++) {
		// if(!mixer->channels[i].data) continue;
		int remaining_samples = num_samples;
		while(remaining_samples > 0) {
			int mix_samples = MIN(remaining_samples, mixer->resample_buffer_len);
			int resampled = adpcm_channel_resample(
				&mixer->channels[i],
				mixer->decode_buffer, mixer->decode_buffer_len,
				mixer->resample_buffer, mixer->resample_buffer_len,
				mix_samples
			);
			// printf("i=%d remaining_samples=%d resampled=%d\n", i, remaining_samples, resampled);
			remaining_samples -= resampled;
			for(int j = 0; j < resampled; j++) {
				// printf("%d ", mixer->resample_buffer[j]);
				out[j] += mixer->channels[i].volume * mixer->resample_buffer[j] / 4096;
				if(out[j] > 32767) out[j] = 32767;
				if(out[j] < -32768) out[j] = -32768;
			}
			// printf("\n");
		}
	}
}

void adpcm_driver_init(struct adpcm_driver *driver, int sample_rate) {
	driver->sample_rate = sample_rate;
	driver->pan = 3;
}

void adpcm_driver_run(
	struct adpcm_driver *d,
	stream_sample_t *outL, stream_sample_t *outR, int num_samples
) {
	d->process_mix(d, outL, outR, num_samples, d->data_ptr);
}

static void adpcm_pcm_driver_process_mix(struct adpcm_driver *driver, stream_sample_t *mixL, stream_sample_t *mixR, int num_samples, void *data_ptr) {
	// printf("adpcm_pcm_driver_process_mix num_samples=%d pan=%d\n", num_samples, driver->pan);
	adpcm_mixer_mix(&driver->mixer, mixL, num_samples);
	for(int i = 0; i < num_samples; i++) {
		if(driver->pan & 2) {
			mixR[i] = mixL[i];
		} else mixR[i] = 0;
		if(!(driver->pan & 1)) {
			mixL[i] = 0;
		}
	}
}

void adpcm_pcm_driver_init(
	struct adpcm_pcm_driver *driver,
	int sample_rate, int16_t *decode_buf, int decode_buf_len,
	int16_t *resample_buf, int resample_buf_len
) {
	adpcm_driver_init(&driver->parent, sample_rate);
	adpcm_mixer_init(
		&driver->parent.mixer, sample_rate,
		decode_buf, decode_buf_len, resample_buf, resample_buf_len
	);
	driver->parent.data_ptr = driver;
	driver->parent.process_mix = adpcm_pcm_driver_process_mix;
}


// void adpcm_driver_init(struct adpcm_driver *d) {
// 	adpcm_init(&d->encoder_status);

// 	adpcm_driver_set_freq(d, 4);

// 	d->oki_write(d, 0x0C, 0x02, d->data_ptr); // OKIM6258: Set Clock Divider to 512
// 	d->oki_write(d, 0x02, 0x00, d->data_ptr); // OKIM6258: Pan Write: LR
// 	d->oki_write(d, 0x00, 0x02, d->data_ptr); // OKIM6258: Control Write: Play: On, Record: Off
// }

// void adpcm_driver_end(struct adpcm_driver *d) {
// }

// void adpcm_driver_set_freq(struct adpcm_driver *d, int f) {
// 	if(f > 4) f = 4;

// 	if(d->rate_changed)
// 		d->rate_changed(d, oki_clocks[f], oki_divs[f], d->data_ptr);
// }

// #define MAX_16BIT (1 << 15 - 1)
// #define MIN_16BIT (- (1 << 15))
// static uint8_t adpcm_driver_mix(struct adpcm_driver *d) {
// 	int16_t mix = 0; // mix into this variable
// 	int playing = 0; // are any channels still playing?
// 	for(int j = 0; j < 8; j++) {
// 		struct adpcm_driver_channel *chan = &d->channels[j];

// 		if(chan->cur_sample >= chan->num_samples)
// 			chan->playing = 0;
// 		if(!chan->playing)
// 			continue;

// 		playing = 1;

// 		int r = chan->resample_remainder + chan->resample_denominator;
// 		// printf("%d remainder=%d numerator=%d denominator=%d\n", chan->resample_remainder, chan->resample_numerator, chan->resample_denominator);
// 		while(r >= chan->resample_numerator) {
// 			chan->last_sample = adpcm_driver_channel_decode_sample(chan);
// 			r -= chan->resample_numerator;
// 		}
// 		chan->resample_remainder = r;
// 		mix += chan->last_sample;
// 	}

// 	if(mix > MAX_16BIT) mix = MAX_16BIT;
// 	if(mix < MIN_16BIT) mix = MIN_16BIT;

// 	// if none of the channels are playing, zero the signal with 48 alternating 0x08 0x00 samples
// 	if(playing == 0) {
// 		if(d->playing != 0) {
// 			d->playing = 0;
// 			d->zeroing_counter = 48;
// 		} else {
// 			d->zeroing_counter--;
// 			if(d->zeroing_counter == 0) {
// 				//d->running = 0;
// 			}
// 		}
// 	}

// 	return adpcm_encode(mix, &d->encoder_status);
// }

// void adpcm_driver_tick(struct adpcm_driver *d) {
// 	if(!d->running) return;

// 	uint8_t s1 = adpcm_driver_mix(d);
// 	uint8_t s2 = adpcm_driver_mix(d);
// 	uint8_t s = (s2 << 4) | (s1 & 0x0f);
// 	d->oki_write(d, 0x01, s, d->data_ptr);
// }


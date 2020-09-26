#include <string.h>

#include "adpcm_driver.h"
#include "speex_resampler.h"
#include "fixed_resampler.h"
#include "mamedef.h"

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

void adpcm_driver_init(struct adpcm_driver *driver) {
	driver->pan = 3;
}

int adpcm_driver_play(struct adpcm_driver *d, uint8_t channel, uint8_t *data, int len, uint8_t freq, uint8_t vol) {
	if(d->play)
		return d->play(d, channel, data, len, freq, vol);
	return 0;
}

int adpcm_driver_stop(struct adpcm_driver *d, uint8_t channel) {
	if(d->stop)
		d->stop(d, channel);
	return 0;
}

int adpcm_driver_set_freq(struct adpcm_driver *d, uint8_t channel, uint8_t freq_num) {
	if(d->set_volume)
		d->set_freq(d, channel, freq_num);
	return 0;
}

int adpcm_driver_set_volume(struct adpcm_driver *d, uint8_t channel, uint8_t vol) {
	if(d->set_volume)
		d->set_volume(d, channel, vol);
	return 0;
}

int adpcm_driver_set_pan(struct adpcm_driver *d, uint8_t pan) {
	if(d->set_pan)
		d->set_pan(d, pan);
	return 0;
}


stream_sample_t sinctbl4[] = {
#include "sinctbl4.h"
};

stream_sample_t sinctbl3[] = {
#include "sinctbl3.h"
};

static int adpcm_mix_driver_channel_init(struct adpcm_mix_driver_channel *channel) {
	channel->data = NULL;
	channel->data_len = 0;
	adpcm_init(&channel->decoder_status);

	return 0;
}
static void adpcm_mix_driver_channel_deinit(struct adpcm_mix_driver_channel *channel) {
	channel->data = NULL;
	channel->data_len = 0;
}

static int adpcm_mix_driver_channel_is_active(struct adpcm_mix_driver_channel *channel) {
	return channel->data && channel->data_pos < channel->data_len;
}

static stream_sample_t adpcm_mix_driver_channel_get_sample(struct adpcm_mix_driver_channel *channel) {
	if(!adpcm_mix_driver_channel_is_active(channel))
		return 0;

	uint8_t b = channel->data[channel->data_pos];
	if(channel->nybble) {
		b >>= 4;
	} else {
		b &= 0x0f;
	}

	stream_sample_t sample = adpcm_decode(b, &channel->decoder_status);
	sample *= 16;
	if(sample > 32767) sample = 32767;
	if(sample < -32767) sample = -32767;

	if(channel->nybble) {
		channel->data_pos++;
		channel->nybble = 0;
	} else {
		channel->nybble = 1;
	}

	return sample;
}

static int adpcm_mix_driver_channel_play(struct adpcm_mix_driver_channel *chan, uint8_t *data, int data_len, uint8_t freq_num, uint8_t volume) {
	adpcm_init(&chan->decoder_status);

	chan->data = data;
	chan->data_len = data_len;
	chan->freq_num = freq_num;
	chan->volume = volume;

	chan->data_pos = 0;
	chan->nybble = 0;

	return 0;
}

static int adpcm_mix_driver_channel_stop(struct adpcm_mix_driver_channel *chan) {
	chan->data = 0;
	chan->data_len = 0;
	chan->volume = 0;
	chan->data_pos = 0;
	chan->nybble = 0;

	return 0;
}

static int adpcm_mix_driver_channel_set_volume(struct adpcm_mix_driver_channel *chan, uint8_t volume) {
	chan->volume = volume;

	return 0;
}

static int adpcm_mix_driver_channel_set_freq(struct adpcm_mix_driver_channel *chan, uint8_t freq_num) {
	chan->freq_num = freq_num;

	return 0;
}

static int adpcm_pcm_mix_driver_alloc_buffers(struct adpcm_pcm_mix_driver *driver, int buf_size) {
	if(driver->buf_size != buf_size) {
		driver->buf_size = buf_size;
		// TODO: error checking
		driver->decode_buf = realloc(driver->decode_buf, driver->buf_size * sizeof(*driver->decode_buf));
		driver->decode_resample_buf = realloc(driver->decode_resample_buf, driver->buf_size * sizeof(*driver->decode_resample_buf));
		driver->mix_buf_l = realloc(driver->mix_buf_l, driver->buf_size * sizeof(*driver->mix_buf_l));
		driver->mix_buf_r = realloc(driver->mix_buf_r, driver->buf_size * sizeof(*driver->mix_buf_r));
		if(!driver->decode_buf || !driver->decode_resample_buf || !driver->mix_buf_l || !driver->mix_buf_r)
			return 1;
	}

	return 0;
}

static int adpcm_pcm_mix_driver_play(struct adpcm_driver *driver, uint8_t channel, uint8_t *data, int data_len, uint8_t freq_num, uint8_t volume) {
	struct adpcm_pcm_mix_driver *pdrv = (struct adpcm_pcm_mix_driver *)driver;
	return adpcm_mix_driver_channel_play(&pdrv->channels[channel], data, data_len, freq_num, volume);
}

static int adpcm_pcm_mix_driver_stop(struct adpcm_driver *driver, uint8_t channel) {
	struct adpcm_pcm_mix_driver *pdrv = (struct adpcm_pcm_mix_driver *)driver;
	return adpcm_mix_driver_channel_stop(&pdrv->channels[channel]);
}

static int adpcm_pcm_mix_driver_set_freq(struct adpcm_driver *driver, uint8_t channel, uint8_t freq) {
	struct adpcm_pcm_mix_driver *pdrv = (struct adpcm_pcm_mix_driver *)driver;
	return adpcm_mix_driver_channel_set_freq(&pdrv->channels[channel], freq);
}

static int adpcm_pcm_mix_driver_set_volume(struct adpcm_driver *driver, uint8_t channel, uint8_t vol) {
	struct adpcm_pcm_mix_driver *pdrv = (struct adpcm_pcm_mix_driver *)driver;
	return adpcm_mix_driver_channel_set_volume(&pdrv->channels[channel], vol);
}

static int adpcm_pcm_mix_driver_set_pan(struct adpcm_driver *driver, uint8_t pan) {
	driver->pan = pan & 0x03;
	return 0;
}

int adpcm_pcm_mix_driver_init(struct adpcm_pcm_mix_driver *driver, int sample_rate, int buf_size) {
	adpcm_driver_init(&driver->adpcm_driver);
	driver->adpcm_driver.play = adpcm_pcm_mix_driver_play;
	driver->adpcm_driver.stop = adpcm_pcm_mix_driver_stop;
	driver->adpcm_driver.set_freq = adpcm_pcm_mix_driver_set_freq;
	driver->adpcm_driver.set_volume = adpcm_pcm_mix_driver_set_volume;
	driver->adpcm_driver.set_pan = adpcm_pcm_mix_driver_set_pan;

	for(int i = 0; i < 8; i++) {
		adpcm_mix_driver_channel_init(&driver->channels[i]);
	}

	fixed_resampler_init(&driver->resamplers[0], sinctbl4, 1, 26, 1, 4); //  3906.25
	fixed_resampler_init(&driver->resamplers[1], sinctbl3, 1, 26, 1, 3); //  5208.33
	fixed_resampler_init(&driver->resamplers[2], sinctbl4, 2, 26, 1, 2); //  7812.50
	fixed_resampler_init(&driver->resamplers[3], sinctbl3, 1, 26, 2, 3); // 10416.67

	adpcm_init(&driver->encoder_status);

	driver->buf_size = 0;
	driver->decode_buf = driver->decode_resample_buf = driver->mix_buf_l = driver->mix_buf_r = 0;
	adpcm_pcm_mix_driver_alloc_buffers(driver, buf_size);

	int err = RESAMPLER_ERR_SUCCESS;
	driver->output_resampler = speex_resampler_init(2, 15625, sample_rate, SPEEX_RESAMPLER_QUALITY_DEFAULT, &err);
	if(err != RESAMPLER_ERR_SUCCESS || !driver->output_resampler) {
		return -1;
	}

	return 0;
}

void adpcm_pcm_mix_driver_deinit(struct adpcm_pcm_mix_driver *driver) {
	speex_resampler_destroy(driver->output_resampler);
	driver->output_resampler = NULL;

	// no ADPCM status deinit

	for(int i = 0; i < 4; i++) {
		fixed_resampler_deinit(&driver->resamplers[i]);
	}

	for(int i = 0; i < 8; i++) {
		adpcm_mix_driver_channel_deinit(&driver->channels[i]);
	}
}

int adpcm_pcm_mix_driver_estimate(struct adpcm_pcm_mix_driver *driver, int buf_size) {
	spx_uint32_t in_len = 1, out_len = buf_size;
	speex_resampler_estimate(driver->output_resampler, 0, &in_len, &out_len);
	return out_len;
}

int adpcm_pcm_mix_driver_run(struct adpcm_pcm_mix_driver *driver, stream_sample_t *buf_l, stream_sample_t *buf_r, int buf_size) {
	int r = adpcm_pcm_mix_driver_alloc_buffers(driver, buf_size);
	if(r != 0)
		return r;

	speex_resampler_set_input_stride(driver->output_resampler, 1);
	speex_resampler_set_output_stride(driver->output_resampler, 1);
	spx_uint32_t in_len = buf_size, out_len = buf_size;
	speex_resampler_estimate(driver->output_resampler, 0, &in_len, &out_len);

	memset(driver->mix_buf_l, 0, in_len * sizeof(*driver->mix_buf_l));
	memset(driver->mix_buf_r, 0, in_len * sizeof(*driver->mix_buf_r));

	stream_sample_t samp;

	for(int i = 0; i < 5; i++) {
		memset(driver->decode_buf, 0, in_len * sizeof(*driver->decode_buf));
		if(i < 4) {
			int estimated_in_len = fixed_resampler_estimate(&driver->resamplers[i], in_len);
			int fixed_in_len = estimated_in_len;
			int fixed_out_len = in_len;
			for(int j = 0; j < 8; j++) {
				struct adpcm_mix_driver_channel *chan = &driver->channels[j];
				if(chan->freq_num != i)
					continue;

				for(int k = 0; k < estimated_in_len; k++) {
					samp = adpcm_mix_driver_channel_get_sample(chan);
					driver->decode_buf[k] += samp;
				}
			}

			fixed_resampler_resample(&driver->resamplers[i], driver->decode_buf, &fixed_in_len, driver->decode_resample_buf, &fixed_out_len);

			for(int k = 0; k < in_len; k++) {
				driver->mix_buf_l[k] += driver->adpcm_driver.pan & 0x01 ? driver->decode_resample_buf[k] : 0;
				driver->mix_buf_r[k] += driver->adpcm_driver.pan & 0x02 ? driver->decode_resample_buf[k] : 0;
			}
		} else {
			for(int j = 0; j < 8; j++) {
				struct adpcm_mix_driver_channel *chan = &driver->channels[j];
				if(chan->freq_num != i)
					continue;

				for(int k = 0; k < in_len; k++) {
					samp = adpcm_mix_driver_channel_get_sample(chan);
					driver->mix_buf_l[k] += driver->adpcm_driver.pan & 0x01 ? samp : 0;
					driver->mix_buf_r[k] += driver->adpcm_driver.pan & 0x02 ? samp : 0;
				}
			}
		}
	}

	spx_uint32_t in_len_l = buf_size;
	spx_uint32_t out_len_l = buf_size;
	speex_resampler_process_int(driver->output_resampler, 0, driver->mix_buf_l, &in_len_l, buf_l, &out_len_l);

	spx_uint32_t in_len_r = buf_size;
	spx_uint32_t out_len_r = buf_size;
	speex_resampler_process_int(driver->output_resampler, 1, driver->mix_buf_r, &in_len_r, buf_r, &out_len_r);

	return 0;
}

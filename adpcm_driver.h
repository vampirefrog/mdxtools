#ifndef ADPCM_DRIVER_H_
#define ADPCM_DRIVER_H_

/* ADPCM mixer and driver */

#include <stdint.h>
#include "adpcm.h"
#include "timer.h"
#include "mamedef.h"
#include "speex_resampler.h"
#include "fixed_resampler.h"

// base ADPCM class
struct adpcm_driver {
	uint8_t pan;

	int (*play)(struct adpcm_driver *d, uint8_t channel, uint8_t *data, int len, uint8_t freq, uint8_t vol);
	int (*stop)(struct adpcm_driver *d, uint8_t channel);
	int (*set_volume)(struct adpcm_driver *d, uint8_t channel, uint8_t vol);
	int (*set_freq)(struct adpcm_driver *d, uint8_t channel, uint8_t freq);
	int (*set_pan)(struct adpcm_driver *d, uint8_t pan);
};

int adpcm_driver_play(struct adpcm_driver *d, uint8_t channel, uint8_t *data, int len, uint8_t freq, uint8_t vol);
int adpcm_driver_stop(struct adpcm_driver *d, uint8_t channel);
int adpcm_driver_set_freq(struct adpcm_driver *d, uint8_t channel, uint8_t freq);
int adpcm_driver_set_volume(struct adpcm_driver *d, uint8_t channel, uint8_t vol);
int adpcm_driver_set_pan(struct adpcm_driver *d, uint8_t pan);


struct adpcm_mix_driver_channel {
	// 0 = 3.9kHz
	// 1 = 5.2kHz
	// 2 = 7.8kHz
	// 3 = 10.4kHz
	// 4 = 15.6kHz
	int freq_num;
	int volume;

	struct adpcm_status decoder_status;
	uint8_t *data;
	int data_len;
	int data_pos;
	int nybble;
};

struct adpcm_pcm_mix_driver {
	struct adpcm_driver adpcm_driver; // parent

	struct adpcm_mix_driver_channel channels[8];
	struct fixed_resampler resamplers[4];
	struct adpcm_status encoder_status;

	stream_sample_t *decode_buf, *decode_resample_buf, *mix_buf_l, *mix_buf_r;
	int buf_size;

	SpeexResamplerState *output_resampler;
};
int adpcm_pcm_mix_driver_init(struct adpcm_pcm_mix_driver *driver, int sample_rate, int buf_size);
void adpcm_pcm_mix_driver_deinit(struct adpcm_pcm_mix_driver *driver);
int adpcm_pcm_mix_driver_estimate(struct adpcm_pcm_mix_driver *driver, int buf_size);
int adpcm_pcm_mix_driver_run(struct adpcm_pcm_mix_driver *driver, stream_sample_t *buf_l, stream_sample_t *buf_r, int buf_size);

#endif /* ADPCM_DRIVER_H_ */

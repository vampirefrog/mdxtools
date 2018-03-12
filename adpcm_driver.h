#ifndef ADPCM_DRIVER_H_
#define ADPCM_DRIVER_H_

/* ADPCM mixer and driver */

#include <stdint.h>
#include "adpcm.h"
#include "timer.h"
#include "mamedef.h"
#include "speex_resampler.h"

struct adpcm_mixer_channel {
//	uint8_t *data;
	int16_t *data;
	int num_samples, cur_sample;
	int volume, freq, pan;
	struct adpcm_status decoder_status;
	SpeexResamplerState *resampler_state;
	int resample_remainder; // used for calculating number of samples to decode
};

struct adpcm_mixer {
	struct adpcm_mixer_channel channels[8];
	int target_rate;
	int16_t *decode_buffer, *resample_buffer;
	int decode_buffer_len, resample_buffer_len;
};

void adpcm_mixer_init(
	struct adpcm_mixer *mixer, int target_rate,
	int16_t *decode_buffer, int decode_buffer_len,
	int16_t *resample_buf, int resample_buf_len
);
void adpcm_mixer_mix(struct adpcm_mixer *mixer, stream_sample_t *out, int num_samples);
int  adpcm_mixer_play(struct adpcm_mixer *d, int channel, int16_t *data, int len, int freq, int vol, int pan);
int  adpcm_mixer_stop(struct adpcm_mixer *d, int channel);
int  adpcm_mixer_set_volume(struct adpcm_mixer *d, int channel, uint8_t vol);

struct adpcm_driver {
	struct adpcm_mixer mixer;
	int sample_rate;
	int pan;
	void *data_ptr;
	void (*process_mix)(struct adpcm_driver *driver, stream_sample_t *mixL, stream_sample_t *mixR, int num_samples, void *data_ptr);
};

void adpcm_driver_init(struct adpcm_driver *d, int sample_rate);
void adpcm_driver_end(struct adpcm_driver *d);
void adpcm_driver_run(struct adpcm_driver *d, stream_sample_t *outL, stream_sample_t *outR, int num_samples);

struct adpcm_oki_driver {
	struct adpcm_driver parent;
	struct adpcm_status encoder_status;
	int zeroing_counter; // output 24 0x80 bytes to zero the signal, after last non-zero sample has played
	SpeexResamplerState *resampler_state; // upsample 15625 to sound card rate
	struct timer timer;
	int samples_left;

	void (*oki_write)(struct adpcm_oki_driver *d, uint8_t adr, uint8_t data, void *data_ptr);
};
void adpcm_oki_driver_init(
	struct adpcm_oki_driver *driver,
	struct adpcm_mixer *mixer,
	void (*oki_write)(struct adpcm_oki_driver *, uint8_t, uint8_t, void *)
);

struct adpcm_pcm_driver {
	struct adpcm_driver parent;
};
void adpcm_pcm_driver_init(
	struct adpcm_pcm_driver *driver,
	int sample_rate, int16_t *decode_buf, int decode_buf_len,
	int16_t *resample_buf, int resample_buf_len
);

#endif /* ADPCM_DRIVER_H_ */

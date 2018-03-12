#ifndef WAV_H_
#define WAV_H_

#include <stdio.h>

struct wav {
	FILE *f;
	int sample_rate;
	int num_channels;
	int bits_per_sample, bytes_per_sample;

	int total_samples;
};

int wav_open(struct wav *w, const char *filename, int sample_rate, int num_channels, int bits);
int wav_write_stereo_sample(struct wav *w, int l, int r);
int wav_write_mono_sample(struct wav *w, int s);
int wav_close(struct wav *w);

#endif /* WAV_H_ */

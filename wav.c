#include "wav.h"

#include <stdint.h>

static inline size_t fwriteu16(FILE  *f, uint16_t i) {
	return fwrite(&i, 1, 2, f);
}

static inline size_t fwriteu32(FILE *f, uint32_t i) {
	return fwrite(&i, 1, 4, f);
}

int wav_open(struct wav *w, const char *filename, int sample_rate, int num_channels, int bits) {
	w->num_channels = num_channels;
	w->bits_per_sample = bits;
	w->bytes_per_sample = (bits + 7) / 8;
	w->sample_rate = sample_rate;
	w->total_samples = 0;

	w->f = fopen(filename, "wb");
	if(!w->f) return -1;

	fwrite("RIFF", 1, 4, w->f);
	fwriteu32(w->f, 4 + 24 + 8 + 0);
	fwrite("WAVE", 1, 4, w->f);
	fwrite("fmt ", 1, 4, w->f);
	fwriteu32(w->f, 16);
	fwriteu16(w->f, 1); // PCM format
	fwriteu16(w->f, num_channels);
	fwriteu32(w->f, sample_rate);
	fwriteu32(w->f, sample_rate * num_channels * w->bytes_per_sample);
	fwriteu16(w->f, num_channels * w->bytes_per_sample);
	fwriteu16(w->f, bits);
	fwrite("data", 1, 4, w->f);
	fwriteu32(w->f, 0);

	return 0;
}

int wav_write_stereo_sample(struct wav *w, int l, int r) {
	fwrite(&l, 1, w->bytes_per_sample, w->f);
	fwrite(&r, 1, w->bytes_per_sample, w->f);
	w->total_samples++;
	return w->total_samples;
}

int wav_write_mono_sample(struct wav *w, int s) {
	fwrite(&s, 1, w->bytes_per_sample, w->f);
	w->total_samples++;
	return w->total_samples;
}

int wav_close(struct wav *w) {
	fseek(w->f, 4, SEEK_SET);
	fwriteu32(w->f, 4 + 24 + 8 + w->total_samples * w->bytes_per_sample * w->num_channels);
	fseek(w->f, 40, SEEK_SET);
	fwriteu32(w->f, w->total_samples * w->bytes_per_sample * w->num_channels);
	fclose(w->f);
	return 0;
}

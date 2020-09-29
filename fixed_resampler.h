#ifndef FIXED_RESAMPLER_H_
#define FIXED_RESAMPLER_H_

#include <stdint.h>
#include <stdlib.h>

#include "mamedef.h"

struct fixed_resampler {
	stream_sample_t *table;
	int table_step;
	int numerator, denominator;

	int num_zero_crossings;

	int counter;
	stream_sample_t *history;
};

int fixed_resampler_init(struct fixed_resampler *resampler, stream_sample_t *table, int table_step, int num_zero_crossings, int numerator, int denominator);
int fixed_resampler_deinit(struct fixed_resampler *resampler);
int fixed_resampler_estimate(struct fixed_resampler *resampler, int output_samples);
int fixed_resampler_resample(struct fixed_resampler *resampler, stream_sample_t *in, int *in_len, stream_sample_t *out, int *out_len);

#endif /* FIXED_RESAMPLER_H_ */

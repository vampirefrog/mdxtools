#include "fixed_resampler.h"

#include <string.h>

int fixed_resampler_init(struct fixed_resampler *resampler, stream_sample_t *table, int table_step, int num_zero_crossings, int numerator, int denominator) {
	resampler->sample_read_cb_data_ptr = 0;
	resampler->sample_read_cb = 0;

	resampler->table = table;
	resampler->table_step = table_step;
	resampler->num_zero_crossings = num_zero_crossings;
	resampler->numerator = numerator;
	resampler->denominator = denominator;
	resampler->counter = numerator; // force reading a sample at the start

	size_t history_byte_size = sizeof(*resampler->history) * resampler->num_zero_crossings * 2;
	resampler->history = malloc(history_byte_size);
	if(!resampler->history)
		return -1;
	memset(resampler->history, 0, history_byte_size);

	return 0;
}

int fixed_resampler_deinit(struct fixed_resampler *resampler) {
	resampler->sample_read_cb_data_ptr = 0;
	resampler->sample_read_cb = 0;

	resampler->table = 0;
	resampler->table_step = 0;
	resampler->num_zero_crossings = 0;
	resampler->numerator = 0;
	resampler->denominator = 0;
	resampler->counter = 0;

	if(resampler->history)
		free(resampler->history);

	return 0;
}

int fixed_resampler_estimate(struct fixed_resampler *resampler, int output_samples) {
	int counter = resampler->counter;
	int numerator = resampler->numerator;
	int denominator = resampler->denominator;

	int ret = 0;

	while(output_samples--) {
		counter += numerator;
		while(counter >= denominator) {
			counter -= denominator;
			ret++;
		}
	}

	return ret;
}

int fixed_resampler_resample(struct fixed_resampler *resampler, stream_sample_t *in, int *in_len, stream_sample_t *out, int *out_len) {
	stream_sample_t *filter = resampler->table;

	int ilen = *in_len, olen = *out_len;

	while(olen) {
		stream_sample_t x = 0;

		if(resampler->counter == 0) {
			x = resampler->history[resampler->num_zero_crossings - 1];
			// printf("reading from history %d\n", x);
		} else {
			stream_sample_t *h1 = resampler->history;
			stream_sample_t *h2 = &resampler->history[resampler->num_zero_crossings];
			stream_sample_t *f1 = &filter[((resampler->num_zero_crossings - 1) * resampler->denominator + resampler->counter) * resampler->table_step];
			stream_sample_t *f2 = &filter[(resampler->denominator - resampler->counter) * resampler->table_step];
			for(int i = 0; i < resampler->num_zero_crossings; i++) {
				stream_sample_t x1;
				x1 = *h1 * *f1;
				x += x1 / 32767;
				x1 = *h2 * *f2;
				x += x1 / 32767;
				h1++;
				h2++;
				f1 -= resampler->denominator * resampler->table_step;
				f2 += resampler->denominator * resampler->table_step;
			}
			// printf("computed x=%d\n", x);
		}

		// printf("counter=%d x=%d num_zero_crossings=%d ilen=%d olen=%d\n", resampler->counter, x, resampler->num_zero_crossings, ilen, olen);

		resampler->counter += resampler->numerator;
		while(resampler->counter >= resampler->denominator) {
			stream_sample_t is;
			if(ilen > 0) {
				is = *in;
				in++;
			} else {
				goto done;
			}
			ilen--;

			resampler->counter -= resampler->denominator;
			// printf("moving history is=%d\n", is);
			memmove(resampler->history, &resampler->history[1], sizeof(*resampler->history) * (resampler->num_zero_crossings * 2 - 1));
			resampler->history[resampler->num_zero_crossings * 2 - 1] = is;
		}

		if(x > 32767) x = 32767;
		if(x < -32767) x = -32767;

		*out++ = x;
		olen--;
	}

done:

	*in_len -= ilen;
	*out_len -= olen;

	return 0;
}

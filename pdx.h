#ifndef PDX_H_
#define PDX_H_

#include <stdint.h>

#define PDX_NUM_SAMPLES 96

struct pdx_sample {
	uint8_t *data;
	int len;
	int16_t *decoded_data;
	int num_samples; // refers to https://en.wikipedia.org/wiki/Sample_(signal)
};

struct pdx_file {
	struct pdx_sample samples[PDX_NUM_SAMPLES];
	int num_samples; // refers to https://en.wikipedia.org/wiki/Sampling_(music)
};

int pdx_file_load(struct pdx_file *pdx, uint8_t *data, int data_len);

#endif /* PDX_H_ */

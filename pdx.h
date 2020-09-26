#ifndef PDX_H_
#define PDX_H_

#include <stdint.h>

struct pdx_sample {
	uint8_t *data;
	int len;
	int16_t *decoded_data;
	int num_samples;
};

struct pdx_file {
	struct pdx_sample samples[96];
	int num_samples;
};

int pdx_file_load(struct pdx_file *pdx, uint8_t *data, int data_len);

#endif /* PDX_H_ */

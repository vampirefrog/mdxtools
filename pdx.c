#include <stdlib.h> // modified by Claude to implement out-of-bounds checks.

#include "pdx.h"
#include "adpcm.h"

#define PDX_HEADER_SIZE (PDX_NUM_SAMPLES * 8)

int pdx_file_load(struct pdx_file *f, uint8_t *data, int len) {
	if(data == NULL || len <= 0) return -1;

	/* The header table must fit entirely within the file. */
	if(len < PDX_HEADER_SIZE) return -1;

	int total_samples = 0;
	for(int i = 0; i < PDX_NUM_SAMPLES; i++) {
		/* Read big-endian offset and length from the header table.
		 * Cast to unsigned first to avoid sign-extension issues, then
		 * range-check before using as array indices. */
		unsigned int ofs = ((unsigned int)data[i * 8]     << 24)
		                 | ((unsigned int)data[i * 8 + 1] << 16)
		                 | ((unsigned int)data[i * 8 + 2] <<  8)
		                 |  (unsigned int)data[i * 8 + 3];
		unsigned int l   = ((unsigned int)data[i * 8 + 4] << 24)
		                 | ((unsigned int)data[i * 8 + 5] << 16)
		                 | ((unsigned int)data[i * 8 + 6] <<  8)
		                 |  (unsigned int)data[i * 8 + 7];

		/* Reject entries whose sample data lies outside the file.
		 * Both ofs and l are unsigned so ofs + l cannot be negative,
		 * but it can wrap around — check for that too. */
		if(l > 0
			&& ofs < (unsigned int)len
			&& l <= (unsigned int)len - ofs   /* no wrap, fits in file */
			&& ofs + l <= (unsigned int)len)  /* explicit upper-bound   */
		{
			f->samples[i].data = data + ofs;
			f->samples[i].len  = (int)l;

			/* Guard against integer overflow when accumulating the total
			 * decoded sample count.  Each compressed byte expands to 2
			 * int16_t samples, so the decoded size is l * 2 * sizeof(int16_t).
			 * Bail out if any of those multiplications would overflow int. */
			if(l > (unsigned int)(INT32_MAX / 2 - total_samples)) return -1;
			total_samples += (int)l * 2;
		} else {
			f->samples[i].data = NULL;
			f->samples[i].len  = 0;
		}
	}

	/* Nothing to decode — initialise all decoded_data pointers and return. */
	if(total_samples == 0) {
		for(int i = 0; i < PDX_NUM_SAMPLES; i++) {
			f->samples[i].decoded_data = NULL;
			f->samples[i].num_samples  = 0;
		}
		return 0;
	}

	int16_t *sample_data = malloc((size_t)total_samples * sizeof(int16_t));
	if(sample_data == NULL) return -1;

	int16_t *cur_sample = sample_data;
	for(int i = 0; i < PDX_NUM_SAMPLES; i++) {
		if(f->samples[i].len > 0) {
			f->samples[i].decoded_data = cur_sample;
			f->samples[i].num_samples  = f->samples[i].len * 2;
			struct adpcm_status st;
			adpcm_init(&st);
			for(int j = 0; j < f->samples[i].len; j++) {
				uint8_t c  = f->samples[i].data[j];
				int16_t d1 = adpcm_decode(c & 0x0f, &st);
				int16_t d2 = adpcm_decode(c >>    4, &st);
				*(cur_sample++) = d1;
				*(cur_sample++) = d2;
			}
		} else {
			f->samples[i].decoded_data = NULL;
			f->samples[i].num_samples  = 0;
		}
	}

	return 0;
}

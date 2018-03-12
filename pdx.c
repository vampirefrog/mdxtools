#include "pdx.h"
#include "adpcm.h"

// TODO: check out of bounds conditions
int pdx_load(struct pdx *f, uint8_t *data, int len) {
	if(data == 0 || len == 0) return -1;

	int total_samples = 0;
	for(int i = 0; i < 96; i++) {
		int ofs = (data[i * 8] << 24) | (data[i * 8 + 1] << 16) | (data[i * 8 + 2] << 8) | data[i * 8 + 3];
		int l = (data[i * 8 + 4] << 24) | (data[i * 8 + 5] << 16) | (data[i * 8 + 6] << 8) | data[i * 8 + 7];
		if(ofs < len && ofs + l <= len) {
			f->samples[i].data = data + ofs;
			if(ofs + l > len)
				l = len - ofs;
			f->samples[i].len = l;
			total_samples += l * 2;
		} else {
			f->samples[i].data = 0;
			f->samples[i].len = 0;
		}
	}
	int16_t *sample_data = malloc(total_samples * 2);
	int16_t *cur_sample = sample_data;
	for(int i = 0; i < 96; i++) {
		if(f->samples[i].len > 0) {
			f->samples[i].decoded_data = cur_sample;
			f->samples[i].num_samples = f->samples[i].len * 2;
			struct adpcm_status st;
			adpcm_init(&st);
			for(int j = 0; j < f->samples[i].len; j++) {
				uint8_t c = f->samples[i].data[j];
				int16_t d1 = adpcm_decode(c & 0x0f, &st);
				int16_t d2 = adpcm_decode(c >> 4, &st);
				*(cur_sample++) = d1;
				*(cur_sample++) = d2;
			}
		}
	}

	return 0;
}

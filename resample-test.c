#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "fixed_resampler.h"

stream_sample_t sinctbl4[] = {
#include "sinctbl4.h"
};

stream_sample_t sinctbl3[] = {
#include "sinctbl3.h"
};

int opt_clock = 4000000;
int opt_divider = 1024;
int opt_zero_crossings = 26;

struct resample_config {
	int clock, div;
	stream_sample_t *tbl;
	int tblstep;
	int approx, target;
	int numerator, denominator;
};

struct resample_config configs[] = {
	{ .clock = 8000000, .div =  512, .tbl = sinctbl4, .tblstep = 4, .approx = 15625, .target = 15625, .numerator = 1, .denominator = 1 },
	{ .clock = 8000000, .div =  768, .tbl = sinctbl3, .tblstep = 1, .approx = 10417, .target = 15625, .numerator = 2, .denominator = 3 },
	{ .clock = 4000000, .div =  512, .tbl = sinctbl4, .tblstep = 2, .approx =  7812, .target = 15625, .numerator = 1, .denominator = 2 },
	{ .clock = 4000000, .div =  768, .tbl = sinctbl3, .tblstep = 1, .approx =  5208, .target = 15625, .numerator = 1, .denominator = 3 },
	{ .clock = 4000000, .div = 1024, .tbl = sinctbl4, .tblstep = 1, .approx =  3906, .target = 15625, .numerator = 1, .denominator = 4 },
};

stream_sample_t sample_read_cb(struct fixed_resampler *resampler, void *data_ptr) {
	FILE *inf = (FILE *)data_ptr;

	int16_t sample;
	fread(&sample, sizeof(sample), 1, inf);

	return sample;
}

int main(int argc, char **argv) {
	FILE *inf = stdin;
	if(argc > 1) {
		inf = fopen(argv[1], "rb");
		if(!inf) {
			fprintf(stderr, "Could not open %s for reading: %s (%d)\n", argv[1], strerror(errno), errno);
			return 0;
		}
	}

	FILE *outf = stdout;
	if(argc > 2) {
		outf = fopen(argv[2], "wb");
		if(!outf) {
			fprintf(stderr, "Could not open %s for writing: %s (%d)\n", argv[2], strerror(errno), errno);
			return 0;
		}
	}

	if(argc > 3) {
		opt_clock = atoi(argv[3]);
	}

	if(argc > 4) {
		opt_divider = atoi(argv[4]);
	}

	if(argc > 5) {
		opt_zero_crossings = atoi(argv[5]);
	}

	struct resample_config *config;
	for(int i = 0; i < sizeof(configs) / sizeof(configs[0]); i++) {
		struct resample_config *c = &configs[i];
		if(c->clock == opt_clock && c->div == opt_divider) {
			config = c;
		}
	}

	if(!config) {
		fprintf(stderr, "Could not find config for clock=%d, divider=%d\n", opt_clock, opt_divider);
		return 1;
	}

	struct fixed_resampler resampler;
	fixed_resampler_init(&resampler, config->tbl, config->tblstep, opt_zero_crossings, config->numerator, config->denominator);

	stream_sample_t in_buf[1024], out_buf[1024];
	int16_t in_buf16[1024], out_buf16[1024];
	while(!feof(inf)) {
		int in_samples = fixed_resampler_estimate(&resampler, 1);
		// printf("in_samples=%d\n", in_samples);

		if(in_samples > 0) {
			fread(in_buf16, sizeof(*in_buf16), in_samples, inf);
			for(int i = 0; i < in_samples; i++)
				in_buf[i] = in_buf16[i];
		}

		int in_len = in_samples, out_len = 1;
		int result = fixed_resampler_resample(&resampler, in_buf, &in_len, out_buf, &out_len);
		// printf("%d %d %d %d  %d %d %d %d\n",
		// 	in_buf[0], in_buf[1], in_buf[2], in_buf[3],
		// 	out_buf[0], out_buf[1], out_buf[2], out_buf[3]);
		printf("result=%d in_len=%d out_len=%d\n", result, in_len, out_len);

		for(int i = 0; i < out_len; i++)
			out_buf16[i] = out_buf[i];
		size_t s = fwrite(out_buf16, sizeof(*out_buf16), out_len, outf);
		if(s < 1) {
			fprintf(stderr, "Error writing sample: %s (%d)", strerror(errno), errno);
			return 1;
		}
	}

	return 0;
}

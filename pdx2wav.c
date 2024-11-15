#include <stdio.h>
#include <sndfile.h>
#include "tools.h"
#include "pdx.h"
#include "adpcm.h"

int main(int argc, char **argv) {
	if(argc < 2) {
		fprintf(stderr, "Usage: %s <file.pdx> [prefix]\n", argv[0]);
		return 1;
	}

	size_t data_len;
	uint8_t *data = load_file(argv[1], &data_len);

	struct pdx_file pdx;
	pdx_file_load(&pdx, data, data_len);

	for(int i = 0; i < 96; i++) {
		if(pdx.samples[i].len == 0) continue;

		SF_INFO sfinfo = {
			.samplerate = 15625,
			.channels = 1,
			.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16,
		};
		char buf[1024];
		snprintf(buf, sizeof(buf), "%s.%03d.wav", argv[1], i);
		printf("Opening %s\n", buf);
		SNDFILE *file = sf_open(buf, SFM_WRITE, &sfinfo);
		if(!file) {
			printf("Failed to open file for writing\n");
			return 1;
		}

		struct adpcm_status st;
		adpcm_init(&st);
		printf("samples %d len=%d\n", i, pdx.samples[i].len);
		for(int j = 0; j < pdx.samples[i].len; j++) {
			int16_t frame[2];
			frame[0] = adpcm_decode(pdx.samples[i].data[j] & 0x0f, &st) << 3;
			frame[1] = adpcm_decode(pdx.samples[i].data[j] >> 4, &st) << 3;
			sf_write_short(file, frame, 1);
		}
		for(int j = 0; j < 24; j++) {
			int16_t frame[2];
			frame[0] = adpcm_decode(8, &st) << 3;
			frame[1] = adpcm_decode(0, &st) << 3;
			sf_write_short(file, frame, 1);
		}

		sf_close(file);
	}

	return 0;
}

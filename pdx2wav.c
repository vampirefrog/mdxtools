#include "tools.h"
#include "pdx.h"
#include "wav.h"
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
		char buf[1024];
		snprintf(buf, sizeof(buf), "%s.%03d.wav", argv[1], i);
		struct wav wav;
		wav_open(&wav, buf, 15625, 1, 16);
		struct adpcm_status st;
		adpcm_init(&st);
		printf("samples %d len=%d\n", i, pdx.samples[i].len);
		for(int j = 0; j < pdx.samples[i].len; j++) {
			wav_write_mono_sample(&wav, adpcm_decode(pdx.samples[i].data[j] & 0x0f, &st) << 3);
			wav_write_mono_sample(&wav, adpcm_decode(pdx.samples[i].data[j] >> 4, &st) << 3);
		}
		for(int j = 0; j < 24; j++) {
			wav_write_mono_sample(&wav, adpcm_decode(8, &st) << 3);
			wav_write_mono_sample(&wav, adpcm_decode(0, &st) << 3);
		}
		wav_close(&wav);
	}

	return 0;
}

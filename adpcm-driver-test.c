#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sndfile.h>

#include "adpcm_pcm_mix_driver.h"
#include "tools.h"

int main(int argc, char **argv) {
	if(argc < 2) {
		fprintf(stderr, "Not enough arguments\n");
		return 1;
	}

	int opt_sample_rate = 44100;

	struct adpcm_pcm_mix_driver driver;
	adpcm_pcm_mix_driver_init(&driver, opt_sample_rate, 0);

	uint8_t *sample;
	size_t sample_len;
	sample = load_file(argv[1], &sample_len);
	if(!sample) {
		fprintf(stderr, "Could not load %s\n", argv[1]);
		return 1;
	}

	adpcm_driver_play((struct adpcm_driver *)&driver, 2, sample, sample_len, 4, 255);
	adpcm_driver_play((struct adpcm_driver *)&driver, 4, sample, sample_len, 2, 255);
	adpcm_driver_play((struct adpcm_driver *)&driver, 3, sample, sample_len, 0, 255);

	SF_INFO sfinfo = {
		.samplerate = opt_sample_rate,
		.channels = 2,
		.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16,
	};
	char wavname[256];
	printf("Outputting to %s\n", wavname);
	replace_ext(wavname, sizeof(wavname), argv[1], "wav");
	SNDFILE *file = sf_open(wavname, SFM_WRITE, &sfinfo);
	if(!file) {
		printf("Failed to open file for writing\n");
		return 1;
	}

	int target_samples = 48000 * 6;
	stream_sample_t bufL[1024], bufR[1024];
	int16_t wavbuf[1024 * 2];
	int inc = 1;
	for(int i = 0; i <= target_samples; i += inc) {
		int estimated = adpcm_pcm_mix_driver_estimate(&driver, inc);
		adpcm_pcm_mix_driver_run(&driver, bufL, bufR, estimated);
		for(int n = 0; n < estimated; n++) {
			if(bufL[n] > 32767)
				bufL[n] = 32767;
			if(bufL[n] < -32767)
				bufL[n] = -32767;
			if(bufR[n] > 32767)
				bufR[n] = 32767;
			if(bufR[n] < -32767)
				bufR[n] = -32767;
			wavbuf[n * 2] = bufL[n];
			wavbuf[n * 2 + 1] = bufR[n];
		}
		sf_write_short(file, wavbuf, estimated);
	}

	sf_close(file);

	return 0;
}

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "adpcm_driver.h"
#include "tools.h"
#include "wav.h"

int main(int argc, char **argv) {
	if(argc < 2) {
		fprintf(stderr, "Not enough arguments\n");
		return 1;
	}

	struct adpcm_pcm_mix_driver driver;
	adpcm_pcm_mix_driver_init(&driver, 48000, 0);

	uint8_t *sample;
	unsigned int sample_len;
	sample = load_file(argv[1], &sample_len);
	if(!sample) {
		fprintf(stderr, "Could not load %s\n", argv[1]);
		return 1;
	}

	adpcm_driver_play((struct adpcm_driver *)&driver, 2, sample, sample_len, 4, 255);
	adpcm_driver_play((struct adpcm_driver *)&driver, 4, sample, sample_len, 2, 255);
	adpcm_driver_play((struct adpcm_driver *)&driver, 3, sample, sample_len, 0, 255);

	struct wav wav;
	char wavname[256];
	replace_ext(wavname, sizeof(wavname), argv[1], "wav");
	wav_open(&wav, wavname, 48000, 2, 16);

	int target_samples = 48000 * 6;
	stream_sample_t bufL[1024], bufR[1024];
	int inc = 1;
	for(int i = 0; i <= target_samples; i += inc) {
		int estimated = adpcm_pcm_mix_driver_estimate(&driver, inc);
		adpcm_pcm_mix_driver_run(&driver, bufL, bufR, inc);
		for(int n = 0; n < inc; n++) {
			if(bufL[n] > 32767)
				bufL[n] = 32767;
			if(bufL[n] < -32767)
				bufL[n] = -32767;
			if(bufR[n] > 32767)
				bufR[n] = 32767;
			if(bufR[n] < -32767)
				bufR[n] = -32767;
			wav_write_stereo_sample(&wav, bufL[n], bufR[n]);
		}
	}

	wav_close(&wav);

	return 0;
}

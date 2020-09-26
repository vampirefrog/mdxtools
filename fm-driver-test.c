#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "mdx.h"
#include "fm_driver.h"
#include "tools.h"
#include "wav.h"

int main(int argc, char **argv) {
	if(argc < 2) {
		fprintf(stderr, "Not enough arguments\n");
		return 1;
	}

	int opt_sample_rate = 44100;

	struct fm_opm_emu_driver fm_driver;
	fm_opm_emu_driver_init(&fm_driver, opt_sample_rate);

	struct mdx_file mdx_file;

	size_t mdx_file_size;
	uint8_t *mdx_file_data = load_file(argv[1], &mdx_file_size);
	mdx_file_load(&mdx_file, mdx_file_data, mdx_file_size);
	printf("Loaded MDX file \"%s\"\n", argv[1]);

	uint8_t *v = 0;
	for(int i = 0; i < 256; i++) {
		if(mdx_file.voices[i]) {
			v = mdx_file.voices[i];
			break;
		}
	}

	if(v) {
		printf(
			"TL %02x %02x %02x %02x\n",
			mdx_voice_osc_get_tl(v, 0),
			mdx_voice_osc_get_tl(v, 1),
			mdx_voice_osc_get_tl(v, 2),
			mdx_voice_osc_get_tl(v, 3)
		);
		fm_driver_load_voice((struct fm_driver *)&fm_driver, 2, v, 0, 3);
		fm_driver_set_pitch((struct fm_driver *)&fm_driver, 2, ((0x2c << 6) + 5) << 8);
		fm_driver_set_tl((struct fm_driver *)&fm_driver, 2, 0x15, v);
		fm_driver_note_on((struct fm_driver *)&fm_driver, 2, 0xf, v);
	}

	struct wav wav;
	char wavname[256];
	replace_ext(wavname, sizeof(wavname), argv[1], "wav");
	wav_open(&wav, wavname, 48000, 2, 16);

	int target_samples = 1024 * 20;
	stream_sample_t bufL[1024], bufR[1024];
	for(int i = 0; i <= target_samples; i += sizeof(bufL) / sizeof(bufL[0])) {
		fm_opm_emu_driver_run(&fm_driver, bufL, bufR, sizeof(bufL) / sizeof(bufL[0]));
		// printf("%d %d %d %d\n", bufL[0], bufL[1], bufL[2], bufL[3]);
		for(int n = 0; n < sizeof(bufL) / sizeof(bufL[0]); n++) {
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

	fm_opm_emu_driver_deinit(&fm_driver);

	return 0;
}
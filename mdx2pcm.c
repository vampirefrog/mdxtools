#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tools.h"
#include "timer.h"
#include "mdx.h"
#include "wav.h"
#include "sjis.h"
#include "cmdline.h"
#include "mdx_renderer.h"
#include "adpcm_driver.h"

#define BUFFER_SIZE 4096
#define SAMPLE_RATE 44100

int opt_channel_mask = 0xffff;
int opt_loops = 1;

int main(int argc, char **argv) {
	int optind = cmdline_parse_args(argc, argv, (struct cmdline_option[]){
		{
			'm', "mask",
			"Channel mask",
			"mask",
			TYPE_REQUIRED,
			TYPE_INT, &opt_channel_mask
		},
		{
			'l', "loops",
			"Loop <n> times.",
			"n",
			TYPE_REQUIRED,
			TYPE_INT, &opt_loops
		},
		CMDLINE_ARG_TERMINATOR
	}, 1, 0, "<file.mdx>");

	if(optind < 0) exit(-optind);

	size_t l;
	uint8_t *mdx_data = load_file(argv[1], &l);
	struct mdx_file f;
	mdx_file_load(&f, mdx_data, l);
	if(f.pdx_filename_len > 0) {
		char buf[256];
		find_pdx_file(argv[1], (char *)f.pdx_filename, buf, sizeof(buf));
		printf("loading PDX \"%s\"\n", buf);
		uint8_t *pdx_data = load_file(buf, &l);
		if(pdx_data)
			pdx_load(&f.pdx, pdx_data, l);
	} else printf("No PDX file\n");

	struct adpcm_pcm_driver d;
	int16_t decode_buf[BUFFER_SIZE], resample_buf[BUFFER_SIZE];
	adpcm_pcm_driver_init(&d, SAMPLE_RATE, decode_buf, BUFFER_SIZE, resample_buf, BUFFER_SIZE);

	SAMP bufL[BUFFER_SIZE], bufR[BUFFER_SIZE], chipBufL[BUFFER_SIZE], chipBufR[BUFFER_SIZE];
	struct mdx_renderer r;
	mdx_renderer_init(
		&r, &f, (struct adpcm_driver *)&d, SAMPLE_RATE,
		bufL, bufR, chipBufL, chipBufR, BUFFER_SIZE
	);
	r.player.driver.max_loops = opt_loops;
	r.player.driver.channel_mask = opt_channel_mask;

	struct wav w;
	wav_open(&w, "mdxout.wav", SAMPLE_RATE, 2, 16);
	int numbufs = argc >= 3 ? atoi(argv[2]) : 100;
	while(!r.player.driver.ended) {
		mdx_renderer_render(&r);
		for(int j = 0; j < BUFFER_SIZE; j++) {
			wav_write_stereo_sample(&w, bufL[j], bufR[j]);
		}
	}
	wav_close(&w);

	return 0;
}

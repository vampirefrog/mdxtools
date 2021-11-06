#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "wav.h"
#include "cmdline.h"
#include "tools.h"
#include "mdx.h"
#include "pdx.h"
#include "mdx_driver.h"
#include "pcm_timer_driver.h"
#include "adpcm_pcm_mix_driver.h"
#include "fm_opm_emu_driver.h"
#include "mamedef.h"

#define BUFFER_SIZE 2048

int opt_channel_mask = 0xffff;
int opt_loops = 1;
char *opt_output = 0;
int opt_sample_rate = 44100;
int opt_fadeout_rate = 4;

stream_sample_t bufL[BUFFER_SIZE], bufR[BUFFER_SIZE], chipBufL[BUFFER_SIZE], chipBufR[BUFFER_SIZE];
int16_t buf[BUFFER_SIZE * 2];

struct mdx_driver mdx_driver;

static void sigint_handler(int s) {
	signal(SIGINT, SIG_DFL);
	printf("Fading out...\n");
	fflush(stdout);
	mdx_driver_start_fadeout(&mdx_driver, opt_fadeout_rate);
}

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
			"Loop <n> times. Default 1.",
			"n",
			TYPE_REQUIRED,
			TYPE_INT, &opt_loops
		},
		{
			'f', "fadeout-rate",
			"Fade-out rate. Default 4.",
			"n",
			TYPE_REQUIRED,
			TYPE_INT, &opt_fadeout_rate
		},
		{
			'o', "output",
			"Output file, use `-' for stdout.",
			"n",
			TYPE_REQUIRED,
			TYPE_STRING, &opt_output
		},
		CMDLINE_ARG_TERMINATOR
	}, 1, -1, "<file.mdx>");

	if(optind < 0) exit(-optind);

	struct pcm_timer_driver timer_driver;
	struct adpcm_pcm_mix_driver adpcm_driver;
	struct fm_opm_emu_driver fm_driver;

	pcm_timer_driver_init(&timer_driver, opt_sample_rate);
	adpcm_pcm_mix_driver_init(&adpcm_driver, opt_sample_rate, 0);
	fm_opm_emu_driver_init(&fm_driver, opt_sample_rate);
	mdx_driver_init(
		&mdx_driver,
		(struct timer_driver *)&timer_driver,
		(struct fm_driver *)&fm_driver,
		(struct adpcm_driver *)&adpcm_driver
	);
	mdx_driver.max_loops = opt_loops;

	stream_sample_t bufL[BUFFER_SIZE], bufR[BUFFER_SIZE];
	stream_sample_t mixBufL[BUFFER_SIZE], mixBufR[BUFFER_SIZE];

	if(!opt_output || !opt_output[0]) {
		char midbuf[PATH_MAX];
		replace_ext(midbuf, sizeof(midbuf), argv[optind], "wav");
		opt_output = midbuf;
	}

	struct wav wav;
	printf("Opening %s\n", opt_output);
	if(wav_open(&wav, opt_output, opt_sample_rate, 2, 16)) {
		fprintf(stderr, "Could not open output file \"%s\": %s (%d)\n", opt_output, strerror(errno), errno);
	}

	signal(SIGINT, sigint_handler);

	for(int i = optind; i < argc; i++) {
		struct mdx_file mdx_file;
		struct pdx_file pdx_file;

		size_t mdx_file_size;
		uint8_t *mdx_file_data = load_file(argv[i], &mdx_file_size);
		if(!mdx_file_data)
			continue;
		mdx_file_load(&mdx_file, mdx_file_data, mdx_file_size);
		printf("Loaded MDX file \"%s\"\n", argv[i]);

		if(mdx_file.pdx_filename_len > 0) {
			char buf[256];
			find_pdx_file(argv[i], (char *)mdx_file.pdx_filename, buf, sizeof(buf));
			if(buf[0]) {
				printf("Loading PDX \"%s\" from \"%s\"\n", mdx_file.pdx_filename, buf);
				size_t pdx_data_size;
				uint8_t *pdx_file_data = load_file(buf, &pdx_data_size);
				if(pdx_file_data)
					pdx_file_load(&pdx_file, pdx_file_data, pdx_data_size);
			} else {
				printf("Could not find \"%s\"\n", mdx_file.pdx_filename);
			}
		} else printf("No PDX file\n");

		mdx_driver_load(&mdx_driver, &mdx_file, &pdx_file);

		while(!mdx_driver.ended) {
			fflush(stdout);
			stream_sample_t *mixBufLp = mixBufL, *mixBufRp = mixBufR;
			int samples_remaining = BUFFER_SIZE;

			memset(mixBufL, 0, sizeof(mixBufL));
			memset(mixBufR, 0, sizeof(mixBufR));

			while(samples_remaining > 0) {
				int timer_samples = pcm_timer_driver_estimate(&timer_driver, samples_remaining);
				int fm_samples = fm_opm_emu_driver_estimate(&fm_driver, samples_remaining);
				int adpcm_samples = adpcm_pcm_mix_driver_estimate(&adpcm_driver, samples_remaining);

				int samples = timer_samples;
				if(fm_samples < samples)
					samples = fm_samples;
				if(adpcm_samples < samples)
					samples = adpcm_samples;

				// printf("running adpcm\n");
				adpcm_pcm_mix_driver_run(&adpcm_driver, bufL, bufR, samples);
				for(int n = 0; n < samples; n++) {
					mixBufLp[n] += bufL[n];
					mixBufRp[n] += bufR[n];
				}
				// printf("running fm\n");
				fm_opm_emu_driver_run(&fm_driver, bufL, bufR, samples);
				for(int n = 0; n < samples; n++) {
					mixBufLp[n] += bufL[n];
					mixBufRp[n] += bufR[n];
				}
				// printf("running pcm_timer_driver\n");
				pcm_timer_driver_advance(&timer_driver, samples);

				samples_remaining -= samples;
				// printf("timer_samples=%d fm_samples=%d adpcm_samples=%d samples=%d samples_remaining=%d\n", timer_samples, fm_samples, adpcm_samples, samples, samples_remaining);
				mixBufLp += samples;
				mixBufRp += samples;
			}

			for(int n = 0; n < BUFFER_SIZE; n++) {
				wav_write_stereo_sample(&wav, mixBufL[n], mixBufR[n]);
			}
		}
	}

	wav_close(&wav);

	return 0;
}

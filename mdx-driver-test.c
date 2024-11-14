#include <stdio.h>
#include <string.h>
#include <audiofile.h>

#include "tools.h"
#include "mdx.h"
#include "pdx.h"
#include "mdx_driver.h"
#include "pcm_timer_driver.h"
#include "adpcm_pcm_mix_driver.h"
#include "fm_opm_emu_driver.h"

int main(int argc, char **argv) {
	struct mdx_driver mdx_driver;
	struct pcm_timer_driver timer_driver;
	struct adpcm_pcm_mix_driver adpcm_driver;
	struct fm_opm_emu_driver fm_driver;

	int opt_sample_rate = 44100;

	pcm_timer_driver_init(&timer_driver, opt_sample_rate);
	adpcm_pcm_mix_driver_init(&adpcm_driver, opt_sample_rate, 0);
	fm_opm_emu_driver_init(&fm_driver, 0, opt_sample_rate);
	mdx_driver_init(
		&mdx_driver,
		(struct timer_driver *)&timer_driver,
		(struct fm_driver *)&fm_driver,
		(struct adpcm_driver *)&adpcm_driver
	);

	struct mdx_file mdx_file;
	struct pdx_file pdx_file;

	size_t mdx_file_size;
	uint8_t *mdx_file_data = load_file(argv[1], &mdx_file_size);
	mdx_file_load(&mdx_file, mdx_file_data, mdx_file_size);
	printf("Loaded MDX file \"%s\"\n", argv[1]);

	if(mdx_file.pdx_filename_len > 0) {
		char buf[256];
		find_pdx_file(argv[1], (char *)mdx_file.pdx_filename, buf, sizeof(buf));
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

	// load files above
	mdx_driver_load(&mdx_driver, &mdx_file, &pdx_file);

	AFfilesetup setup = afNewFileSetup();
	afInitFileFormat(setup, AF_FILE_WAVE);
	afInitChannels(setup, AF_DEFAULT_TRACK, 2);
	afInitRate(setup, AF_DEFAULT_TRACK, 48000);
	afInitSampleFormat(setup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);
	char wavname[256];
	printf("Outputting to %s\n", wavname);
	replace_ext(wavname, sizeof(wavname), argv[1], "wav");
	AFfilehandle file = afOpenFile("output.wav", "w", setup);
	if(file == AF_NULL_FILEHANDLE) {
		fprintf(stderr, "Error opening output file.\n");
		return 1;
	}

#define BUFFER_SIZE 1024
	stream_sample_t bufL[BUFFER_SIZE], bufR[BUFFER_SIZE];
	int16_t mixBuf[BUFFER_SIZE * 2];
	int16_t *mixBufp = mixBuf;
	for(int i = 0; i < 1000; i++) {
		memset(mixBuf, 0, sizeof(mixBuf));
		mixBufp = mixBuf;
		int samples_remaining = BUFFER_SIZE;
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
				mixBufp[n * 2] += bufL[n];
				mixBufp[n * 2 + 1] += bufR[n];
			}
			// printf("running fm\n");
			fm_opm_emu_driver_run(&fm_driver, bufL, bufR, samples);
			for(int n = 0; n < samples; n++) {
				mixBufp[n * 2] += bufL[n];
				mixBufp[n * 2 + 1] += bufR[n];
			}
			// printf("running pcm_timer_driver\n");
			pcm_timer_driver_advance(&timer_driver, samples);

			samples_remaining -= samples;
			// printf("timer_samples=%d fm_samples=%d adpcm_samples=%d samples=%d samples_remaining=%d\n", timer_samples, fm_samples, adpcm_samples, samples, samples_remaining);
			mixBufp += samples * 2;
		}

		afWriteFrames(file, AF_DEFAULT_TRACK, mixBuf, BUFFER_SIZE);
	}

	afCloseFile(file);
	afFreeFileSetup(setup);

	return 0;
}

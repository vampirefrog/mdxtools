#include <stdio.h>
#include <signal.h>

#include <portaudio.h>

#include "tools.h"
#include "cmdline.h"
#include "mdx_renderer.h"
#include "adpcm_driver.h"

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 2048

int opt_channel_mask = 0xffff;
int opt_loops = 1;

SAMP bufL[BUFFER_SIZE], bufR[BUFFER_SIZE], chipBufL[BUFFER_SIZE], chipBufR[BUFFER_SIZE];
int16_t buf[BUFFER_SIZE * 2];
struct mdx_renderer r;

static void sigint_handler(int s){
	signal(SIGINT, SIG_DFL);
	printf("Fading out...\n");
	mdx_driver_start_fadeout(&r.player.driver, 4);
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
			"Loop <n> times.",
			"n",
			TYPE_REQUIRED,
			TYPE_INT, &opt_loops
		},
		CMDLINE_ARG_TERMINATOR
	}, 1, 0, "<file.mdx>");

	if(optind < 0) exit(-optind);

	size_t l;
	uint8_t *mdx_data = load_file(argv[optind], &l);
	if(!mdx_data) {
		return 1;
	}
	struct mdx_file f;
	int er = mdx_file_load(&f, mdx_data, l);
	if(er != MDX_SUCCESS) {
		printf("Error loading MDX file \"%s\": %s (%d)\n", argv[optind], mdx_error_name(er), er);
		return 2;
	}
	printf("Loaded \"%s\"\n", argv[optind]);
	if(f.pdx_filename_len > 0) {
		char buf[256];
		find_pdx_file(argv[optind], (char *)f.pdx_filename, buf, sizeof(buf));
		if(buf[0]) {
			printf("Loading PDX \"%s\" from \"%s\"\n", f.pdx_filename, buf);
			uint8_t *pdx_data = load_file(buf, &l);
			if(pdx_data)
				pdx_load(&f.pdx, pdx_data, l);
		} else {
			printf("Could not find \"%s\"\n", f.pdx_filename);
		}
	} else printf("No PDX file\n");

	struct adpcm_pcm_driver d;
	int16_t decode_buf[BUFFER_SIZE], resample_buf[BUFFER_SIZE];
	adpcm_pcm_driver_init(&d, SAMPLE_RATE, decode_buf, BUFFER_SIZE, resample_buf, BUFFER_SIZE);

	mdx_renderer_init(
		&r, &f, (struct adpcm_driver *)&d, SAMPLE_RATE,
		bufL, bufR, chipBufL, chipBufR, BUFFER_SIZE
	);
	r.player.driver.max_loops = opt_loops;
	r.player.driver.channel_mask = opt_channel_mask;

	signal(SIGINT, sigint_handler);

	PaStream *stream;
	PaError err;

	err = Pa_Initialize();
	if( err != paNoError ) goto error;

	err = Pa_OpenDefaultStream(
		&stream,
		0,              /* no input channels */
		2,              /* stereo output */
		paInt16,        /* 32 bit floating point output */
		SAMPLE_RATE,
		BUFFER_SIZE,    /* frames per buffer */
		NULL,
		NULL
	);
	if( err != paNoError ) goto error;

	err = Pa_StartStream( stream );
	if( err != paNoError ) goto error;

	while(!r.player.driver.ended) {
		mdx_renderer_render(&r);
		int16_t *out = buf;
		for(int i = 0; i < BUFFER_SIZE; i++) {
			*out++ = bufL[i];
			*out++ = bufR[i];
		}
		err = Pa_WriteStream( stream, buf, BUFFER_SIZE);
		if( err ) goto error;
	}

	err = Pa_StopStream( stream );
	if( err != paNoError ) goto error;
	err = Pa_CloseStream( stream );
	if( err != paNoError ) goto error;
	Pa_Terminate();

	return err;

error:
	Pa_Terminate();
	fprintf( stderr, "An error occured while using the portaudio stream\n" );
	fprintf( stderr, "Error number: %d\n", err );
	fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
	return err;
}

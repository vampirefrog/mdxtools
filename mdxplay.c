#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ao/ao.h>
#include <math.h>
#include <errno.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glob.h>
#include <signal.h>

#include "tools.h"
#include "cmdline.h"
#include "mdx_renderer.h"
#include "adpcm_driver.h"

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 2048

int opt_channel_mask = 0xffff;
int opt_loops = 1;

struct mdx_renderer r;

static void sigint_handler(int s){
	signal(SIGINT, SIG_DFL);
	printf("Fading out...\n");
	mdx_driver_start_fadeout(&r.player.driver, 4);
}

static void tick_cb(struct mdx_driver *d, void *data_ptr) {
	(void)data_ptr;

	char *cols_env= getenv("COLUMNS");
	int cols = cols_env ? atoi(cols_env) : 80;
	if(cols > 96) cols = 96;
	int octaves = cols / 12;
	static int notes[16] = {
		-2, -2, -2, -2,
		-2, -2, -2, -2,
		-2, -2, -2, -2,
		-2, -2, -2, -2,
	};


	printf("\033[0;0f");
	char *keyb = "█  █ █ █  █ ";
	char *keybs[12] = {
		"\033[34m█\033[47;30m  █ █ █  █ ",
		"█\033[44m \033[47;30m █ █ █  █ ",
		"█ \033[44m \033[47;30m█ █ █  █ ",
		"█  \033[34m█\033[47;30m █ █  █ ",
		"█  █\033[44m \033[47;30m█ █  █ ",
		"█  █ \033[34m█\033[47;30m █  █ ",
		"█  █ █\033[44m \033[47;30m█  █ ",
		"█  █ █ \033[34m█\033[47;30m  █ ",
		"█  █ █ █\033[44m \033[47;30m █ ",
		"█  █ █ █ \033[44m \033[47;30m█ ",
		"█  █ █ █  \033[34m█\033[47;30m ",
		"█  █ █ █  █\033[44m \033[47;30m",
	};
	printf("\033[16A");
	for(int y = 0; y < 16; y++) {
		struct mdx_driver_channel *chan = &d->channels[y];
		printf("\033[47;30m");
		if(chan->note != notes[y]) {
			notes[y] = d->channels[y].note;
			if(chan->note > -1) {
				int oct = notes[y] / 12;
				int note = notes[y] % 12;
				for(int i = 0; i < octaves; i++) {
					if(i == oct && chan->ticks_remaining > 0) {
						printf("%s", keybs[note]);
					} else printf("%s", keyb);
				}
				printf("\033[0m\033[K");
			}
		}
		printf("\n");
	}
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
	// if(er == MDX_ERR_LZX) {
	// 	uint8_t *lzx = mdx_data + 4 + f.data_start_ofs;
	// 	l -= f.data_start_ofs + 4;
	// 	lzx_decompress(lzx, l);
	// 	return 1;
	// }
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

	SAMP bufL[BUFFER_SIZE], bufR[BUFFER_SIZE], chipBufL[BUFFER_SIZE], chipBufR[BUFFER_SIZE];
	mdx_renderer_init(
		&r, &f, (struct adpcm_driver *)&d, SAMPLE_RATE,
		bufL, bufR, chipBufL, chipBufR, BUFFER_SIZE
	);
	r.player.driver.max_loops = opt_loops;
	r.player.driver.channel_mask = opt_channel_mask;
	//r.player.driver.tick_cb = tick_cb;

	ao_initialize();

	/* -- Setup for default driver -- */

	int default_driver = ao_default_driver_id();

	ao_sample_format format;
	memset(&format, 0, sizeof(format));
	format.bits = 16;
	format.channels = 2;
	format.rate = SAMPLE_RATE;
	format.byte_format = AO_FMT_LITTLE;

	/* -- Open driver -- */
	ao_device *device = ao_open_live(default_driver, &format, NULL /* no options */);
	if (device == NULL) {
		fprintf(stderr, "Error opening device.\n");
		return 1;
	}

	// signal(SIGINT, sigint_handler);

	int16_t aobuf[BUFFER_SIZE * 2];
	while(!r.player.driver.ended) {
		mdx_renderer_render(&r);
		for(int i = 0; i < BUFFER_SIZE; i++) {
			aobuf[i * 2] = bufL[i];
			aobuf[i * 2 + 1] = bufR[i];
		}
		ao_play(device, (char *)aobuf, BUFFER_SIZE * 2 * 2);
	}

	/* -- Close and shutdown -- */
	ao_close(device);

	ao_shutdown();

	return (0);
}

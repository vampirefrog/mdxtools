#include <stdio.h>
#include <string.h>
#ifdef __linux__
#include <linux/limits.h>
#endif
#ifdef __APPLE__
#include <sys/syslimits.h>
#endif
#include <errno.h>
#include "cmdline.h"
#include "mdx.h"
#include "tools.h"
#include "midi_timer_driver.h"
#include "adpcm_midi_driver.h"
#include "fm_midi_driver.h"
#include "mdx_driver.h"
#include "midilib/midi.h"
#include "midilib/midi_file.h"
#include "midilib/midi_track.h"
#include "midilib/stream.h"

char *opt_output = 0;
int opt_channel_mask = 0xffff;
int opt_loops = 1;
int opt_fadeout_rate = 4;

static int write_cb(void *buf, size_t len, void *data_ptr) {
	return fwrite(buf, 1, len, (FILE *)data_ptr);
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
			"Output to this file. `-' means stdout (default).",
			"file",
			TYPE_OPTIONAL,
			TYPE_STRING, &opt_output
		},
		CMDLINE_ARG_TERMINATOR
	}, 1, 1, "<file.mdx>");

	if(optind < 0) exit(-optind);

	for(int i = optind; i < argc; i++) {
		struct midi_file midi_file;
		midi_file_init(&midi_file, MIDI_FORMAT_MULTI_TRACKS, 16, 48);

		for(int t = 0; t < 16; t++) {
			struct midi_track *track = midi_file.tracks + t;
			char buf[256];
			snprintf(buf, sizeof(buf), "Channel %c (%s)", mdx_track_name(t), t < 8 ? "FM" : "ADPCM");
			midi_track_write_track_name(track, 0, buf, -1);
			midi_track_write_bank_select(track, 0, t, 0);
			midi_track_write_control_change(track, 0, t, 126, 127); // Monophonic mode
			if(t >= 8) { // OPM only
				midi_track_write_nrpn_msb(track, 0, t, 0, 112); // OPM Clock = 4MHz
			}
			midi_track_write_control_change(track, 0, t, 12, 0); // Amplitude LFO level
			midi_track_write_control_change(track, 0, t, 13, 0); // Pitch LFO level
			midi_track_write_rpn_msb(track, 0, t, 0, 32); // Pitch Bend Sensitivity
			midi_track_write_pitch_bend(track, 0, t, 0x2000);
		}

		struct midi_track *first_track = midi_file_prepend_empty_track(&midi_file);
		midi_track_write_time_signature(first_track, 0, 4, 2, 0, 0);
		char buf[PATH_MAX];
		snprintf(buf, sizeof(buf), "Converted from %s", argv[optind]);
		midi_track_write_text(first_track, 0, buf, -1);

		struct mdx_driver mdx_driver;
		struct midi_timer_driver timer_driver;
		struct adpcm_midi_driver adpcm_driver;
		struct fm_midi_driver fm_driver;

		midi_timer_driver_init(&timer_driver);
		adpcm_midi_driver_init(&adpcm_driver, &midi_file);
		fm_midi_driver_init(&fm_driver, &midi_file);
		mdx_driver_init(
			&mdx_driver,
			(struct timer_driver *)&timer_driver,
			(struct fm_driver *)&fm_driver,
			(struct adpcm_driver *)&adpcm_driver
		);
		mdx_driver.max_loops = opt_loops;

		struct mdx_file mdx_file;
		struct pdx_file pdx_file;

		size_t mdx_file_size;
		uint8_t *mdx_file_data = load_file(argv[i], &mdx_file_size);
		if(!mdx_file_data)
			continue;
		mdx_file_load(&mdx_file, mdx_file_data, mdx_file_size);
		printf("Loaded MDX file \"%s\"\n", argv[i]);

		mdx_driver_load(&mdx_driver, &mdx_file, &pdx_file);

		while(!mdx_driver.ended) {
			adpcm_midi_driver_tick(&adpcm_driver);
			fm_midi_driver_tick(&fm_driver);
			midi_timer_driver_tick(&timer_driver);
		}

		adpcm_midi_driver_end(&adpcm_driver);
		fm_midi_driver_end(&fm_driver);

		if(!opt_output || !opt_output[0]) {
			char midbuf[PATH_MAX];
			replace_ext(midbuf, sizeof(midbuf), argv[optind], "mid");
			opt_output = midbuf;
		}
		FILE *o = fopen(opt_output, "wb");
		if(!o) {
			fprintf(stderr, "Could not open %s: %s (%d)\n", opt_output, strerror(errno), errno);
			return -1;
		}
		midi_file_write(&midi_file, write_cb, o);
		fclose(o);
		midi_file_clear(&midi_file);
	}

	return 0;
}

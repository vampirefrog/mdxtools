#include <stdio.h>
#include <string.h>
#include "mdx.h"
#include "mdx_decompiler.h"
#include "tools.h"
#include "cmdline.h"

int opt_utf8 = 0;
char *opt_output = 0;
int opt_columns = 80;
int opt_adpcm_notes = 0;
int opt_fm_note_nums = 0;
int opt_ticks_only = 0;
FILE *of;

void line_cb(const char *ln) {
	if(ln) fprintf(of, "%s\n", ln);
}

int main(int argc, char **argv) {
	int optind = cmdline_parse_args(argc, argv, (struct cmdline_option[]){
		{
			'u', "utf8",
			"Convert strings to UTF-8",
			0,
			TYPE_SWITCH,
			TYPE_INT, &opt_utf8
		},
		{
			'c', "columns",
			"Wrap text at this many columns.",
			"cols",
			TYPE_OPTIONAL,
			TYPE_INT, &opt_columns
		},
		{
			'o', "output",
			"Output to this file. `-' means stdout (default).",
			"file",
			TYPE_OPTIONAL,
			TYPE_STRING, &opt_output
		},
		{
			'f', "fm-note-nums",
			"Output FM notes as numbers: n10,20",
			0,
			TYPE_SWITCH,
			TYPE_INT, &opt_fm_note_nums
		},
		{
			'a', "adpcm-notes",
			"Output ADPCM notes as normal notes: cdef",
			0,
			TYPE_SWITCH,
			TYPE_INT, &opt_adpcm_notes
		},
		{
			't', "ticks-only",
			"Only express note lengths in ticks.",
			0,
			TYPE_SWITCH,
			TYPE_INT, &opt_ticks_only
		},
		CMDLINE_ARG_TERMINATOR
	}, 1, 1, "<file.mdx>");

	if(optind < 0) exit(-optind);

	of = stdout;
	if(opt_output && strcmp(opt_output, "-")) of = fopen(opt_output, "w");

	for(int i = optind; i < argc; i++) {
		size_t l;
		uint8_t *mdx_data = load_file(argv[optind], &l);
		if(!mdx_data) {
			return 1;
		}
		struct mdx_file f;
		int er = mdx_file_load(&f, mdx_data, l);
		if(er != MDX_SUCCESS) {
			fprintf(stderr, "Error loading MDX file \"%s\": %s (%d)\n", argv[optind], mdx_error_name(er), er);
			return 2;
		}

		struct mdx_decompiler d;
		char buf[512];
		if(opt_columns > sizeof(buf) - 1)
			opt_columns = sizeof(buf) - 1;
		mdx_decompiler_init(&d, buf, opt_columns);
		d.adpcm_notes = opt_adpcm_notes;
		d.fm_note_nums = opt_fm_note_nums;
		d.ticks_only = opt_ticks_only;
		d.line = line_cb;
		mdx_decompiler_decompile(&d, &f);
	}

	if(opt_output) fclose(of);

	return 0;
}

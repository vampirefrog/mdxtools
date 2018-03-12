#pragma once

#include "mdx.h"

struct mdx_decompiler {
	// line buffer
	char *buf;
	int columns;
	int cur_col;

	// settings:
	int adpcm_notes; // Output ADPCM notes as cdef
	int fm_note_nums; // Output FM notes as n10,20
	int ticks_only; // output note lengths as ticks always

	// current channel state
	int rest_ticks;
	int next_key_off;
	int portamento;
	int octave;
	int loop_ofs;

	void (*line)(const char *ln);
};

void mdx_decompiler_init(struct mdx_decompiler *d, char *buf, int columns);
void mdx_decompiler_decompile(struct mdx_decompiler *d, struct mdx_file *f);

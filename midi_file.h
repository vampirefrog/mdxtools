#pragma once

#include "midi.h"
#include "midi_track.h"
#include "stream.h"

struct midi_file {
	int file_format;
	int num_tracks;
	int ticks_per_quarter_note;

	struct midi_track *tracks;
};

int midi_file_init(struct midi_file *f, int file_format, int num_tracks, int ticks_per_quarter_note);
int midi_file_clear(struct midi_file *f);
int midi_file_write(struct midi_file *f, struct stream *stream);
struct midi_track *midi_file_append_track(struct midi_file *f);
struct midi_track *midi_file_prepend_track(struct midi_file *f);

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "midi_file.h"

int midi_file_init(struct midi_file *f, int file_format, int num_tracks, int ticks_per_quarter_note) {
	f->file_format = file_format;
	f->ticks_per_quarter_note = ticks_per_quarter_note;
	f->num_tracks = num_tracks;

	if(num_tracks > 0) {
		f->tracks = malloc(sizeof(struct midi_track *) * num_tracks);
		if(!f->tracks) return MIDI_ERR_OUT_OF_MEMORY;
	}

	return MIDI_SUCCESS;
}

int midi_file_clear(struct midi_file *f) {
	if(f->tracks) free(f->tracks);
	if(errno) return errno;
	f->tracks = 0;
	f->num_tracks = 0;

	return 0;
}

int midi_file_write(struct midi_file *f, struct stream *stream) {
	stream_write(stream, "MThd", 4);
	stream_write_big_uint32(stream, 0x06);
	stream_write_big_uint16(stream, f->file_format);
	stream_write_big_uint16(stream, f->num_tracks);
	stream_write_big_uint16(stream, f->ticks_per_quarter_note);

	for(int i = 0; i < f->num_tracks; i++) {
		struct midi_track *t = &f->tracks[i];
		stream_write(stream, "MTrk", 4);
		stream_write_big_uint32(stream, t->buffer.data_len);
		stream_write_buffer(stream, &t->buffer);
	}

	return 0;
}

struct midi_track *midi_file_append_track(struct midi_file *f) {
	f->num_tracks++;
	f->tracks = realloc(f->tracks, f->num_tracks * sizeof(struct midi_track));
	if(!f->tracks)
		return 0;
	struct midi_track *track = &f->tracks[f->num_tracks - 1];
	midi_track_init(track);
	return track;
}

struct midi_track *midi_file_prepend_track(struct midi_file *f) {
	f->num_tracks++;
	f->tracks = realloc(f->tracks, f->num_tracks * sizeof(struct midi_track));
	if(!f->tracks)
		return 0;
	memmove(f->tracks + 1, f->tracks, (f->num_tracks - 1) * sizeof(struct midi_track));
	struct midi_track *track = &f->tracks[0];
	midi_track_init(track);
	return track;
}

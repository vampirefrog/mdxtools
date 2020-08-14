#include "midi_writer.h"

int midi_file_write(struct write_stream *stream, struct midi_track *tracks, int numTracks, int ticksPerQuarterNote = 960, int fileType = 1) {
	stream_write(stream, "MThd", 4);
	stream_write_big_uint32(stream, 0x06);
	stream_write_big_uint16(stream, fileType);
	stream_write_big_uint16(stream, numTracks);
	stream_write_big_uint16(stream, ticksPerQuarterNote);

	for(int i = 0; i < numTracks; i++) {
		struct midi_track *t = &tracks[i];
		stream_write(stream, "MTrk", 4);
		stream_write_big_uint32(stream, t.len);
		stream_write_buffer(stream, t->buffer);
	}
}

#pragma once

#include "midi.h"

int midi_file_write(struct write_stream *stream, struct midi_track *tracks, int numTracks, int ticksPerQuarterNote = 960, int fileType = 1);

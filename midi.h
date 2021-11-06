#pragma once

#include <stdint.h>

#define MIDI_SUCCESS 0
#define MIDI_ERR_NOT_A_MIDI_FILE 1
#define MIDI_ERR_INVALID_TRACK_HEADER 2
#define MIDI_ERR_OUT_OF_MEMORY 3

#define MIDI_FORMAT_SINGLE_TRACK 0
#define MIDI_FORMAT_MULTI_TRACKS 1
#define MIDI_FORMAT_SEQ_TRACKS 2

#define MIDI_DEFAULT_TICKS_PER_QUARTER_NOTE 960
#define MIDI_DEFAULT_TEMPO 120

const char *midi_file_format_name(uint16_t n);
const char *midi_note_name(uint8_t n, uint8_t *octave);
const char *midi_cc_name(uint8_t cc);
const char *midi_rpn_name(uint16_t rpn);

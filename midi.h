#pragma once

#include <stdint.h>
#include "stream.h"

#define MIDI_SUCCESS 0
#define MIDI_ERR_NOT_A_MIDI_FILE 1
#define MIDI_ERR_INVALID_TRACK_HEADER 2
#define MIDI_ERR_OUT_OF_MEMORY 3

#define MIDI_FORMAT_SINGLE_TRACK 0
#define MIDI_FORMAT_MULTI_TRACKS 1
#define MIDI_FORMAT_SEQ_TRACKS 2

#define MIDI_DEFAULT_TICKS_PER_QUARTER_NOTE 960
#define MIDI_DEFAULT_TEMPO 120

struct midi_track {
	struct buffer buffer;
	uint8_t last_cmd;
};

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

const char *midi_file_format_name(uint16_t n);
const char *midi_note_name(uint8_t n, uint8_t *octave);
const char *midi_cc_name(uint8_t cc);
const char *midi_rpn_name(uint16_t rpn);

void midi_track_init(struct midi_track *track);
void midi_track_write_delta_time(struct midi_track *track, uint32_t t);
void midi_track_write_cmd(track, struct midi_track *track, uint8_t cmd, bool noReuse = false);
void midi_track_write_note_on(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint8_t note, uint8_t velocity);
void midi_track_write_meta_event(struct midi_track *track, uint32_t delta_time, uint8_t ev, uint8_t len, uint8_t *buf);
void midi_track_write_meta_event(struct midi_track *track, uint32_t delta_time, uint8_t ev, uint8_t len, ...);
void midi_track_write_tempo(struct midi_track *track, uint32_t delta_time, uint32_t tempo);
void midi_track_write_time_signature(struct midi_track *track, uint32_t delta_time, uint8_t numerator, uint8_t denominator, uint8_t ticksPerClick = 0, uint8_t quarterNote32ndNotes = 0);
void midi_track_write_track_end(struct midi_track *track, uint32_t delta_time);
void midi_track_write_program_change(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint8_t program);
void midi_track_write_text_event(struct midi_track *track, uint8_t event, uint32_t delta_time, const char *text, int len = -1);
void midi_track_write_text(struct midi_track *track, uint32_t delta_time, const char *text, int len = -1);
void midi_track_write_track_name(struct midi_track *track, uint32_t delta_time, const char *text, int len = -1);
void midi_track_write_pitch_bend(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint16_t bend);
void midi_track_write_control_change(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint8_t controller, uint8_t value);
void midi_track_write_nrpn(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint16_t number, uint8_t value_msb);
void midi_track_write_nrpn(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint16_t number, uint8_t value_msb, uint8_t value_lsb);
void midi_track_write_rpn(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint16_t number, uint8_t value_msb);
void midi_track_write_rpn(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint16_t number, uint8_t value_msb, uint8_t value_lsb);
void midi_track_write_bank_select(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint16_t bank);


#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include "midi.h"

const char *midi_file_format_name(uint16_t n) {
	const char *format_names[] = {
		"single-track",
		"multiple tracks, synchronous",
		"multiple tracks, asynchronous"
	};
	if(n < 3) return format_names[n];
	return "unknown";
}

const char *midi_note_name(uint8_t n, uint8_t *octave) {
	const char *note_names[] = {
		"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
	};
	if(octave) *octave = n / 12 - 1;
	return note_names[n % 12];
}

const char *midi_cc_name(uint8_t cc) {
	const char *cc_names[] = {
		"Bank select",
		"Modulation",
		"Breath Controller",
		"Controller 3 (undefined)",
		"Foot Controller",
		"Portamento Time",
		"Data Entry MSB",
		"Channel Volume (formerly Main Volume)",
		"Balance",
		"Controller 9 (undefined)",
		"Pan",
		"Expression",
		"Effect Control 1",
		"Effect Control 2",
		"Controller 14 (undefined)",
		"Controller 15 (undefined)",
		"General Purpose 1",
		"General Purpose 2",
		"General Purpose 3",
		"General Purpose 4",
		"Controller 20 (undefined)",
		"Controller 21 (undefined)",
		"Controller 22 (undefined)",
		"Controller 23 (undefined)",
		"Controller 24 (undefined)",
		"Controller 25 (undefined)",
		"Controller 26 (undefined)",
		"Controller 27 (undefined)",
		"Controller 28 (undefined)",
		"Controller 29 (undefined)",
		"Controller 30 (undefined)",
		"Controller 31 (undefined)",
		"Bank select LSB",
		"Modulation LSB",
		"Breath Controller LSB",
		"Controller 35 (undefined)",
		"Foot Controller LSB",
		"Portamento Time LSB",
		"Data Entry LSB",
		"Channel Volume LSB (formerly Main Volume)",
		"Balance LSB",
		"Controller 41 (undefined)",
		"Pan LSB",
		"Expression LSB",
		"Effect Control 1 LSB",
		"Effect Control 2 LSB",
		"Controller 46 (undefined)",
		"Controller 47 (undefined)",
		"General Purpose 1 LSB",
		"General Purpose 2 LSB",
		"General Purpose 3 LSB",
		"General Purpose 4 LSB",
		"Controller 52 (undefined)",
		"Controller 53 (undefined)",
		"Controller 54 (undefined)",
		"Controller 55 (undefined)",
		"Controller 56 (undefined)",
		"Controller 57 (undefined)",
		"Controller 58 (undefined)",
		"Controller 59 (undefined)",
		"Controller 60 (undefined)",
		"Controller 61 (undefined)",
		"Controller 62 (undefined)",
		"Controller 63 (undefined)",
		"Damper Pedal (Sustain)",
		"Portamento On/Off",
		"Sostenuto On/Off",
		"Soft Pedal On/Off",
		"Legato Footswitch",
		"Hold 2",
		"Sound Controller 1 (Sound Variation)",
		"Sound Controller 2 (Resonance/Timbre)",
		"Sound Controller 3 (Release Time)",
		"Sound Controller 4 (Attack Time)",
		"Sound Controller 5 (Cut-off Frequency/Brightness)",
		"Sound Controller 6 (Decay Time)",
		"Sound Controller 7 (Vibrato Rate)",
		"Sound Controller 8 (Vibrato Depth)",
		"Sound Controller 9 (Vibrato Delay)",
		"Sound Controller 10 (undefined)",
		"General Purpose 5",
		"General Purpose 6",
		"General Purpose 7",
		"General Purpose 7",
		"Portamento Control",
		"Controller 85 (undefined)",
		"Controller 86 (undefined)",
		"Controller 87 (undefined)",
		"Controller 88 (undefined)",
		"Controller 89 (undefined)",
		"Controller 90 (undefined)",
		"Effects 1 Depth (Reverb)",
		"Effects 2 Depth (Tremolo)",
		"Effects 3 Depth (Chorus)",
		"Effects 4 Depth (Celeste/Detune)",
		"Effects 5 Depth (Phaser)",
		"Data Entry +1 (Increment)",
		"Data Entry -1(Decrement)",
		"NRPN LSB",
		"NRPN MSB",
		"RPN LSB",
		"RPN MSB",
		"Controller 102 (undefined)",
		"Controller 103 (undefined)",
		"Controller 104 (undefined)",
		"Controller 105 (undefined)",
		"Controller 106 (undefined)",
		"Controller 107 (undefined)",
		"Controller 108 (undefined)",
		"Controller 109 (undefined)",
		"Controller 110 (undefined)",
		"Controller 111 (undefined)",
		"Controller 112 (undefined)",
		"Controller 113 (undefined)",
		"Controller 114 (undefined)",
		"Controller 115 (undefined)",
		"Controller 116 (undefined)",
		"Controller 117 (undefined)",
		"Controller 118 (undefined)",
		"Controller 119 (undefined)",
		"All Sound Off",
		"Reset All Controllers",
		"Local Control On/Off",
		"All Notes Off",
		"Omni Mode Off",
		"Omni Mode On",
		"Poly Mode Off/Mono Mode On",
		"Poly Mode On/Mono Mode Off",
	};
	return cc_names[cc & 0x7f];
}

const char *midi_rpn_name(uint16_t rpn) {
	const char *names[] = {
		"Pitch Bend Sensitivity",
		"Fine Tuning",
		"Coarse Tuning",
		"Tuning Program Select",
		"Tuning Bank Select",
	};
	if(rpn < 5) return names[rpn];
	return "Unknown";
}

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

void midi_track_init(struct midi_track *track) {
	buffer_init(&track->buffer);
	track->last_cmd = 0;
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

void midi_track_write_delta_time(struct midi_track *track, uint32_t t) {
	if(t > 0x1fffff) buffer_write_uint8(&track->buffer, 0x80 | ((t >> 21) & 0x7f));
	if(t > 0x3fff) buffer_write_uint8(&track->buffer, 0x80 | ((t >> 14) & 0x7f));
	if(t > 0x7f) buffer_write_uint8(&track->buffer, 0x80 | ((t >> 7) & 0x7f));
	buffer_write_uint8(&track->buffer, t & 0x7f);
}

void midi_track_write_cmd(struct midi_track *track, uint8_t cmd, int noReuse) {
	if(track->last_cmd != cmd || noReuse) {
		track->last_cmd = cmd;
		buffer_write_uint8(&track->buffer, cmd);
	}
}

void midi_track_write_note_on(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint8_t note, uint8_t velocity) {
	midi_track_write_delta_time(track, delta_time);
	midi_track_write_cmd(track, 0x90 | (channel & 0x0f), 0);
	buffer_write_uint8(&track->buffer, note & 0x7f);
	buffer_write_uint8(&track->buffer, velocity & 0x7f);
}

void midi_track_write_meta_event_buf(struct midi_track *track, uint32_t delta_time, uint8_t ev, uint8_t len, uint8_t *buf) {
	midi_track_write_delta_time(track, delta_time);
	midi_track_write_cmd(track, 0xff, 1);
	buffer_write_uint8(&track->buffer, ev);
	buffer_write_uint8(&track->buffer, len);
	buffer_write(&track->buffer, buf, len);
}

void midi_track_write_meta_event(struct midi_track *track, uint32_t delta_time, uint8_t ev, uint8_t len, ...) {
	midi_track_write_delta_time(track, delta_time);
	midi_track_write_cmd(track, 0xff, 1);
	buffer_write_uint8(&track->buffer, ev);
	buffer_write_uint8(&track->buffer, len);
	if(len > 0) {
		va_list ap;
		va_start(ap, len);
		for(int i = 0; i < len; i++) buffer_write_uint8(&track->buffer, va_arg(ap, unsigned int));
		va_end(ap);
	}
}

void midi_track_write_tempo(struct midi_track *track, uint32_t delta_time, uint32_t tempo) {
	midi_track_write_meta_event(track, delta_time, 0x51, 3, (tempo >> 16) & 0xff, (tempo >> 8) & 0xff, (tempo >> 0) & 0xff);
}

void midi_track_write_time_signature(struct midi_track *track, uint32_t delta_time, uint8_t numerator, uint8_t denominator, uint8_t ticksPerClick, uint8_t quarterNote32ndNotes) {
	if(quarterNote32ndNotes > 0 || ticksPerClick > 0)
		midi_track_write_meta_event(track, delta_time, 0x58, 4, numerator, denominator, ticksPerClick, quarterNote32ndNotes);
	else
		midi_track_write_meta_event(track, delta_time, 0x58, 2, numerator, denominator);
}

void midi_track_write_track_end(struct midi_track *track, uint32_t delta_time) {
	midi_track_write_meta_event(track, delta_time, 0x2f, 0);
}

void midi_track_write_program_change(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint8_t program) {
	midi_track_write_delta_time(track, delta_time);
	uint8_t cmd = 0xc0 | (channel & 0x0f);
	midi_track_write_cmd(track, cmd, 0);
	buffer_write_uint8(&track->buffer, program & 0x7f);
}

void midi_track_write_text_event(struct midi_track *track, uint8_t event, uint32_t delta_time, const char *text, int len) {
	if(len < 0) len = strlen(text);
	if(len > 127) len = 127;
	midi_track_write_meta_event_buf(track, delta_time, event, len, (uint8_t *)text);
}

void midi_track_write_text(struct midi_track *track, uint32_t delta_time, const char *text, int len) {
	midi_track_write_text_event(track, 0x01, delta_time, text, len);
}

void midi_track_write_track_name(struct midi_track *track, uint32_t delta_time, const char *text, int len) {
	midi_track_write_text_event(track, 0x03, delta_time, text, len);
}

void midi_track_write_pitch_bend(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint16_t bend) {
	midi_track_write_delta_time(track, delta_time);
	midi_track_write_cmd(track, 0xe0 | (channel & 0x0f), 0);
	buffer_write_uint8(&track->buffer, bend & 0x7f);
	buffer_write_uint8(&track->buffer, (bend >> 7) & 0x7f);
}

void midi_track_write_control_change(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint8_t controller, uint8_t value) {
	midi_track_write_delta_time(track, delta_time);
	midi_track_write_cmd(track, 0xb0 | (channel & 0x0f), 1);
	buffer_write_uint8(&track->buffer, controller & 0x7f);
	buffer_write_uint8(&track->buffer, value & 0x7f);
}

void midi_track_write_nrpn_msb(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint16_t number, uint8_t value_msb) {
	midi_track_write_control_change(track, delta_time, channel, 99, (number >> 7) & 0x7f);
	midi_track_write_control_change(track, 0, channel, 98, number & 0x7f);
	midi_track_write_control_change(track, 0, channel, 6, value_msb & 0x7f);
}

void midi_track_write_nrpn_msb_lsb(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint16_t number, uint8_t value_msb, uint8_t value_lsb) {
	midi_track_write_control_change(track, delta_time, channel, 99, (number >> 7) & 0x7f);
	midi_track_write_control_change(track, 0, channel, 98, number & 0x7f);
	midi_track_write_control_change(track, 0, channel, 6, value_msb & 0x7f);
	midi_track_write_control_change(track, 0, channel, 38, value_lsb & 0x7f);
}

void midi_track_write_rpn_msb(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint16_t number, uint8_t value_msb) {
	midi_track_write_control_change(track, delta_time, channel, 101, (number >> 7) & 0x7f);
	midi_track_write_control_change(track, 0, channel, 100, number & 0x7f);
	midi_track_write_control_change(track, 0, channel, 6, value_msb & 0x7f);
}

void midi_track_write_rpn_msb_lsb(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint16_t number, uint8_t value_msb, uint8_t value_lsb) {
	midi_track_write_control_change(track, delta_time, channel, 101, (number >> 7) & 0x7f);
	midi_track_write_control_change(track, 0, channel, 100, number & 0x7f);
	midi_track_write_control_change(track, 0, channel, 6, value_msb & 0x7f);
	midi_track_write_control_change(track, 0, channel, 38, value_lsb & 0x7f);
}

void midi_track_write_bank_select(struct midi_track *track, uint32_t delta_time, uint8_t channel, uint16_t bank) {
	midi_track_write_control_change(track, delta_time, channel, 0, (bank >> 7) & 0x7f);
	midi_track_write_control_change(track, 0, channel, 32, bank & 0x7f);
}

void midi_track_write_track(struct midi_track *track, struct midi_track *from) {
	buffer_write_buffer(&track->buffer, &from->buffer);
}

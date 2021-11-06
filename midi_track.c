#include <stdarg.h>
#include <string.h>
#include "midi_track.h"

void midi_track_init(struct midi_track *track) {
	buffer_init(&track->buffer);
	track->last_cmd = 0;
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

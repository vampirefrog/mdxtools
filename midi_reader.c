#include <stdlib.h>

#include "midi_reader.h"

struct midi_reader *midi_reader_new();

void midi_reader_init(struct midi_reader *r) {
	r->rpn_msb = 127;
	r->rpn_lsb = 127;
	r->nrpn = 0;
	r->dump_bytes = 0;

	r->handle_header = 0;
	r->handle_byte = 0;
	r->handle_track = 0;
	r->handle_unknown_command = 0;
	r->handle_note_on = 0;
	r->handle_note_off = 0;
	r->handle_key_after_touch = 0;
	r->handle_control_change = 0;
	r->handle_program_change = 0;
	r->handle_channel_after_touch = 0;
	r->handle_pitch_wheel_change = 0;
	r->handle_meta_event = 0;
	r->handle_meta_sequence_number = 0;
	r->handle_text_event = 0;
	r->handle_copyright_info_event = 0;
	r->handle_track_name_event = 0;
	r->handle_instrument_name_event = 0;
	r->handle_lyric_event = 0;
	r->handle_marker_text_event = 0;
	r->handle_program_name_event = 0;
	r->handle_device_name_event = 0;
	r->handle_tempo = 0;
	r->handle_time_signature = 0;
	r->handle_track_end = 0;
	r->handle_rpn = 0;
	r->handle_rpn_lsb = 0;
	r->handle_nrpn = 0;
	r->handle_nrpn_lsb = 0;
}

int midi_reader_load(struct midi_reader *r, struct stream *s) {
	r->stream = s;

	midi_reader_read_header(r);
	stream_seek((struct stream *)r->stream, r->header_len + 8, SEEK_SET);
	for(int i = 0; i < r->num_tracks; i++) {
		int ret = midi_reader_read_track(r, i);
		if(ret != MIDI_SUCCESS) {
			return ret;
		}
	}

	return MIDI_SUCCESS;
}

int midi_reader_read_header(struct midi_reader *r) {
	if(!stream_read_compare((struct stream *)r->stream, "MThd", 0)) {
		return MIDI_ERR_NOT_A_MIDI_FILE;
	}
	r->header_len = stream_read_big_uint32((struct stream *)r->stream);
	r->file_format = stream_read_big_uint16((struct stream *)r->stream);
	r->num_tracks = stream_read_big_uint16((struct stream *)r->stream);
	r->ticks_per_quarter_note = stream_read_big_uint16((struct stream *)r->stream);
	if(r->handle_header)
		r->handle_header(r);

	return MIDI_SUCCESS;
}

int midi_reader_read_track(struct midi_reader *r, int i) {
	if(!stream_read_compare((struct stream *)r->stream, "MTrk", 0)) {
		return MIDI_ERR_INVALID_TRACK_HEADER;
	}

	int track_length = stream_read_big_uint32((struct stream *)r->stream);
	if(r->handle_track)
		r->handle_track(r, i, track_length);

	r->state = None;
	r->duration = r->channel = 0;
	for(int j = track_length; j > 0 && !stream_eof((struct stream *)r->stream); j--) {
		uint8_t b = stream_read_uint8((struct stream *)r->stream);
		if(r->handle_byte)
			r->handle_byte(r, b);
		midi_reader_eat(r, b);
	}

	return MIDI_SUCCESS;
}

void midi_reader_eat(struct midi_reader *r, uint8_t b) {
	switch(r->state) {
		case None:
			r->duration = 0;
			// fall through to the next case
		case Duration:
			r->duration <<= 7;
			r->duration |= (b & 0x7f);
			if(b & 0x80) {
				r->state = Duration;
			} else {
				r->state = AfterDuration;
			}
			break;
		case AfterDuration:
			if(b & 0x80) {
				r->command = b;
				r->channel = b & 0x0f;
			} else {
				switch(r->command) {
					case 0xff:
						r->xx = b;
						r->state = MetaEventL;
						break;
					case 0xf8:
						// Timing clock
						break;
					case 0xfa:
						break;
					case 0xfb:
						break;
					case 0xfc:
						break;
					default:
						switch(r->command & 0xf0) {
							case 0x80:
								r->nn = b;
								r->state = NoteOffV;
								break;
							case 0x90:
								r->nn = b;
								r->state = NoteOnV;
								break;
							case 0xa0:
								r->nn = b;
								r->state = KeyAfterTouchV;
								break;
							case 0xb0:
								r->nn = b;
								r->state = ControlChangeV;
								break;
							case 0xc0:
								if(r->handle_program_change)
									r->handle_program_change(r, r->channel, r->duration, b);
								r->state = None;
								break;
							case 0xd0:
								if(r->handle_channel_after_touch)
									r->handle_channel_after_touch(r, r->channel, r->duration, b);
								r->state = None;
								break;
							case 0xe0:
								r->nn = b;
								r->state = PitchWheelChangeT;
								break;
							default:
								if(r->handle_unknown_command)
									r->handle_unknown_command(r, b);
								r->state = None;
								break;
						}
						break;
				}
			}
			break;
		case MetaEventL:
			r->meta_len = b;
			r->meta_count = 0;
			if(r->meta_len == 0) {
				if(r->handle_meta_event)
					r->handle_meta_event(r, r->duration, r->xx, 0, 0);
				switch(r->xx) {
					case 0x2f:
						if(r->handle_track_end)
							r->handle_track_end(r, r->duration);
						break;
				}
				r->state = None;
			} else {
				r->meta_buf = malloc(r->meta_len + 1); // add +1 for trailing zero
				r->state = MetaEventD;
			}
			break;
		case MetaEventD:
			r->meta_buf[r->meta_count++] = b;
			if(r->meta_count == r->meta_len) {
				r->state = None;
				r->meta_buf[r->meta_len] = 0;
				if(r->handle_meta_event)
					r->handle_meta_event(r, r->duration, r->xx, r->meta_len, r->meta_buf);
				switch(r->xx) {
					case 0x00: // Track sequence number
						if(r->meta_len >= 2) {
							if(r->handle_meta_sequence_number)
								r->handle_meta_sequence_number(r, r->duration, (r->meta_buf[0] << 8) | r->meta_buf[1]);
						}
						break;
					case 0x01: // Text event
						if(r->handle_text_event)
							r->handle_text_event(r, r->duration, (const char *)r->meta_buf);
						break;
					case 0x02: // Copyright info text
						if(r->handle_copyright_info_event)
							r->handle_copyright_info_event(r, r->duration, (const char *)r->meta_buf);
						break;
					case 0x03: // Track name text
						if(r->handle_track_name_event)
							r->handle_track_name_event(r, r->duration, (const char *)r->meta_buf);
						break;
					case 0x04: // Instrument name text
						if(r->handle_instrument_name_event)
							r->handle_instrument_name_event(r, r->duration, (const char *)r->meta_buf);
						break;
					case 0x05: // Lyric text
						if(r->handle_lyric_event)
							r->handle_lyric_event(r, r->duration, (const char *)r->meta_buf);
						break;
					case 0x06: // Marker text
						if(r->handle_marker_text_event)
							r->handle_marker_text_event(r, r->duration, (const char *)r->meta_buf);
						break;
					case 0x08:
						if(r->handle_program_name_event)
							r->handle_program_name_event(r, r->duration, (const char *)r->meta_buf);
						break;
					case 0x09:
						if(r->handle_device_name_event)
							r->handle_device_name_event(r, r->duration, (const char *)r->meta_buf);
						break;
					case 0x51: // Set tempo
						if(r->meta_len >= 3)
							if(r->handle_tempo)
								r->handle_tempo(r, r->duration, (r->meta_buf[0] << 16) | (r->meta_buf[1] << 8) | r->meta_buf[2]);
						break;
					case 0x58: {
							uint8_t numerator = 0;
							uint8_t denominator = 0;
							uint8_t ticksPerClick = 0;
							uint8_t quarterNote32ndNotes = 0;
							if(r->meta_len >= 1) numerator = r->meta_buf[0];
							if(r->meta_len >= 2) denominator = r->meta_buf[1];
							if(r->meta_len >= 3) ticksPerClick = r->meta_buf[2];
							if(r->meta_len >= 4) quarterNote32ndNotes = r->meta_buf[3];
							if(r->handle_time_signature)
								r->handle_time_signature(r, r->duration, numerator, denominator, ticksPerClick, quarterNote32ndNotes);
						}
						break;
				}
				free(r->meta_buf);
				r->meta_buf = 0;
			}
			break;
		case NoteOffV:
			if(r->handle_note_off)
				r->handle_note_off(r, r->channel, r->duration, r->nn, b);
			r->state = None;
			break;
		case NoteOnV:
			if(r->handle_note_on)
				r->handle_note_on(r, r->channel, r->duration, r->nn, b);
			r->state = None;
			break;
		case KeyAfterTouchV:
			if(r->handle_key_after_touch)
				r->handle_key_after_touch(r, r->channel, r->duration, r->nn, b);
			r->state = None;
			break;
		case ControlChangeV:
			if(r->handle_control_change)
				r->handle_control_change(r, r->channel, r->duration, r->nn, b);
			switch(r->nn) {
				case 98:
					r->rpn_lsb = b;
					r->nrpn = 1;
					break;
				case 99:
					r->rpn_msb = b;
					r->nrpn = 1;
					break;
				case 100:
					r->rpn_lsb = b;
					r->nrpn = 0;
					break;
				case 101:
					r->rpn_msb = b;
					r->nrpn = 0;
					break;
				case 6:
					if(r->rpn_lsb < 127 && r->rpn_msb < 127) {
						if(r->nrpn) {
							if(r->handle_nrpn)
								r->handle_nrpn(r, r->duration, r->channel, ((r->rpn_msb & 0x7f) << 7) | (r->rpn_lsb & 0x7f), b);
						} else {
							if(r->handle_rpn)
								r->handle_rpn(r, r->duration, r->channel, ((r->rpn_msb & 0x7f) << 7) | (r->rpn_lsb & 0x7f), b);
						}
					}
					break;
				case 38:
					if(r->rpn_lsb < 127 && r->rpn_msb < 127) {
						if(r->nrpn) {
							if(r->handle_nrpn_lsb)
								r->handle_nrpn_lsb(r, r->duration, r->channel, ((r->rpn_msb & 0x7f) << 7) | (r->rpn_lsb & 0x7f), b);
						} else {
							if(r->handle_rpn_lsb)
								r->handle_rpn_lsb(r, r->duration, r->channel, ((r->rpn_msb & 0x7f) << 7) | (r->rpn_lsb & 0x7f), b);
						}
					}
					break;
			}
			r->state = None;
			break;
		case PitchWheelChangeT:
			if(r->handle_pitch_wheel_change)
				r->handle_pitch_wheel_change(r, r->channel, r->duration, r->nn | (b << 7));
			r->state = None;
			break;
		default:
			r->state = None;
	}
}

#include <stdlib.h>
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

int midi_reader_load(struct midi_reader *r, struct read_stream *s) {
	r->read_stream = s;

	midi_reader_read_header(r);
	stream_seek((struct stream *)r->read_stream, r->header_len + 8, SEEK_SET);
	for(int i = 0; i < r->num_tracks; i++) {
		midi_reader_read_track(r, i);
	}

	return MIDI_SUCCESS;
}

int midi_reader_read_header(struct midi_reader *r) {
	if(!stream_read_compare((struct stream *)r->read_stream, "MThd", 0)) {
		return MIDI_ERR_NOT_A_MIDI_FILE;
	}
	r->header_len = stream_read_big_uint32((struct stream *)r->read_stream);
	r->file_format = stream_read_big_uint16((struct stream *)r->read_stream);
	r->num_tracks = stream_read_big_uint16((struct stream *)r->read_stream);
	r->ticks_per_quarter_note = stream_read_big_uint16((struct stream *)r->read_stream);
	if(r->handle_header)
		r->handle_header(r);

	return MIDI_SUCCESS;
}

int midi_reader_read_track(struct midi_reader *r, int i) {
	if(!stream_read_compare((struct stream *)r->read_stream, "MTrk", 0)) {
		return MIDI_ERR_INVALID_TRACK_HEADER;
	}

	int track_length = stream_read_big_uint32((struct stream *)r->read_stream);
	if(r->handle_track)
		r->handle_track(r, i, track_length);

	r->state = None;
	r->duration = r->channel = 0;
	for(int j = track_length; j > 0 && !stream_eof((struct stream *)r->read_stream); j--) {
		uint8_t b = stream_read_uint8((struct stream *)r->read_stream);
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

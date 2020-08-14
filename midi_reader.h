#pragma once

#include "midi.h"

struct midi_reader {
	struct stream *read_stream;

	uint8_t rpn_msb, rpn_lsb;
	unsigned nrpn:1;
	unsigned dump_bytes:1;
	int header_len;
	int file_format;
	int num_tracks;
	int ticks_per_quarter_note;

	enum {
		None = 0,
		Duration,
		AfterDuration, // either a command or running mode
		AfterCommand,
		NoteOffN,
		NoteOffV,
		NoteOnN,
		NoteOnV,
		KeyAfterTouchN,
		KeyAfterTouchV,
		ControlChangeC,
		ControlChangeV,
		ProgramChangeP,
		ChannelAfterTouchC,
		PitchWheelChangeB,
		PitchWheelChangeT,
		MetaEventC,
		MetaEventL,
		MetaEventD
	} state;
	uint8_t nn, xx, meta_len, meta_count, *meta_buf, command;
	uint32_t duration;
	uint8_t channel;

	void (*handle_header)(struct midi_reader *h);
	void (*handle_byte)(struct midi_reader *h, uint8_t b);
	void (*handle_track)(struct midi_reader *h, int number, int length);
	void (*handle_unknown_command)(struct midi_reader *h, uint8_t b);
	void (*handle_note_on)(struct midi_reader *h, uint8_t channel, int duration, int note, int vel);
	void (*handle_note_off)(struct midi_reader *h, uint8_t channel, int duration, int note, int vel);
	void (*handle_key_after_touch)(struct midi_reader *h, uint8_t channel, int duration, int note, int vel);
	void (*handle_control_change)(struct midi_reader *h, uint8_t channel, int duration, int controller, int value);
	void (*handle_program_change)(struct midi_reader *h, uint8_t channel, int duration, int program);
	void (*handle_channel_after_touch)(struct midi_reader *h, uint8_t channel, int duration, int value);
	void (*handle_pitch_wheel_change)(struct midi_reader *h, uint8_t channel, uint32_t duration, uint16_t value);
	void (*handle_meta_event)(struct midi_reader *h, int duration, int cmd, int len, uint8_t *data);
	void (*handle_meta_sequence_number)(struct midi_reader *h, int duration, uint16_t seq);
	void (*handle_text_event)(struct midi_reader *h, int duration, const char *txt);
	void (*handle_copyright_info_event)(struct midi_reader *h, int duration, const char *txt);
	void (*handle_track_name_event)(struct midi_reader *h, int duration, const char *txt);
	void (*handle_instrument_name_event)(struct midi_reader *h, int duration, const char *txt);
	void (*handle_lyric_event)(struct midi_reader *h, int duration, const char *txt);
	void (*handle_marker_text_event)(struct midi_reader *h, int duration, const char *txt);
	void (*handle_program_name_event)(struct midi_reader *h, int duration, const char  *txt);
	void (*handle_device_name_event)(struct midi_reader *h, int duration, const char  *txt);
	void (*handle_tempo)(struct midi_reader *h, int duration, uint32_t tempo);
	void (*handle_time_signature)(struct midi_reader *h, int duration, uint8_t numerator, uint8_t denominator, uint8_t ticks_per_click, uint8_t quarter_note32nd_notes);
	void (*handle_track_end)(struct midi_reader *h, int duration);
	void (*handle_rpn)(struct midi_reader *h, uint8_t channel, int duration, uint16_t number, uint8_t msb);
	void (*handle_rpn_lsb)(struct midi_reader *h, uint8_t channel, int duration, uint16_t number, uint8_t lsb);
	void (*handle_nrpn)(struct midi_reader *h, uint8_t channel, int duration, uint16_t number, uint8_t msb);
	void (*handle_nrpn_lsb)(struct midi_reader *h, uint8_t channel, int duration, uint16_t number, uint8_t lsb);
};

void midi_reader_init(struct midi_reader *r);
int midi_reader_load(struct midi_reader *r, struct read_stream *s);
int midi_reader_read_header(struct midi_reader *r);
int midi_reader_read_track(struct midi_reader *r, int i);
void midi_reader_eat(struct midi_reader *r, uint8_t b);

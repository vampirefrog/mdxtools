#pragma once

#include <stdint.h>
#include "mdx.h"

struct mdx_interpreter_track {
	uint8_t *data;
	int len;
	int pos;

	int used;

	// state
	int ended, waiting;
	int ticks_remaining;
	int volume, opm_volume; // cache computed OPM volume

	uint8_t key_on_delay, key_on_delay_counter; // k
	int staccato, staccato_counter; // q
	int voice_num; // @

	int note, pan, pitch;
	int adpcm_freq_num;
	int16_t detune;
	int16_t portamento;
	int sample_pos;

	int skipNoteOff, skipNoteOn;

	// MP pitch LFO
	int lfo_delay, lfo_delay_counter;
	struct mdx_lfo pitch_lfo, amplitude_lfo;

	// MH OPM LFO
	uint8_t pms_ams, keysync, mhon;
};

void mdx_interpreter_track_init(struct mdx_interpreter_track *track, struct mdx_channel *mchan) {
	memset(track, 0, sizeof(*track));
	chan->data = mchan->data;
	chan->len = mchan->data_len;
	chan->pos = 0;

	chan->used = 0;
	chan->ended = 0;

	chan->key_on_delay = 0;
	chan->key_on_delay_counter = 0;
	chan->staccato = 8;
	chan->staccato_counter = 0;

	chan->ticks_remaining = 0;
	chan->volume = 24;
	chan->voice_num = -1;
	chan->skipNoteOff = 0;
	chan->skipNoteOn = 0;
	chan->pan = 3;
	chan->detune = 0;
	chan->note = -1;
	chan->repeat_stack_depth = 0;
	chan->portamento = 0;
	chan->adpcm_freq_num = 4;
}

struct mdx_interpreter {
	struct mdx_file *mdx_file;
	struct mdx_interpreter_track tracks[16];

	void (*note_on)(struct mdx_interpreter *, int);
	void (*note_off)(struct mdx_interpreter *, int);
	void (*write_opm)(struct mdx_interpreter *, uint8_t, uint8_t);
	void (*set_voice)(struct mdx_interpreter *, uint8_t track, uint8_t voice);
	void (*set_pan)(struct mdx_interpereter *, uint8_t track, uint8_t pan);
	
};

void mdx_interpreter_init(struct mdx_interpreter *interpreter, struct mdx_file *file);
void mdx_interpreter_tick(struct mdx_interpreter *interpreter);

#endif /* MDX_DRIVER_H_ */

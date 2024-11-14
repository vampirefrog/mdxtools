#pragma once

#include <stdint.h>

#include "midilib/buffer.h"

typedef void* yyscan_t;

#define MML_FLAG_AT (1 << 16)
#define MML_NOTE_FLAT (1 << 16)
#define MML_NOTE_SHARP (1 << 17)

struct mdx_compiler_channel {
	struct buffer buf;
	int total_ticks;
	int default_note_length;
	int octave;

	// repeat tracking
	struct {
		int start_location;
		int escape_location;
	} repeat_stack[32];
	int repeat_stack_pos;
	int loop_pos;
};

struct mdx_compiler_opm_voice_op {
	uint8_t dt1_mul, tl, ks_ar, ame_d1r, dt2_d2r, d1l_rr;
};

struct mdx_compiler_opm_voice {
	uint8_t used;
	struct mdx_compiler_opm_voice_op op[4];
	uint8_t fl_con, slot_mask;
};

struct mdx_compiler {
	struct mdx_compiler_channel channels[16];
	struct mdx_compiler_opm_voice voices[256];
	int ex_pcm;
	char *title, *pcmfile;

	yyscan_t scanner;
	int chan_mask;
};

struct mml_notelength {
	enum {
		NoteLenTicks,
		NoteLenDot,
		NoteLenInt,
		NoteLenDefault,
		NoteLenJoin
	} type;
	int val;
	struct mml_notelength *n1, *n2;
};

void mdx_compiler_init(struct mdx_compiler *);
void mdx_compiler_destroy(struct mdx_compiler *);
int  mdx_compiler_parse(struct mdx_compiler *, FILE *f);
void mdx_compiler_dump(struct mdx_compiler *);
void mdx_compiler_save(struct mdx_compiler *, const char *filename);
void mdx_compiler_directive(struct mdx_compiler *compiler, char *directive, char *value);
void mdx_compiler_opmdef(struct mdx_compiler *compiler, int voice_num, uint8_t *data);
void mdx_compiler_write(struct mdx_compiler *compiler, int chan_mask, ...);
void mdx_compiler_note(struct mdx_compiler *compiler, int chan_mask, int note, struct mml_notelength *l);
void mdx_compiler_note_num(struct mdx_compiler *compiler, int chan_mask, int note, struct mml_notelength *l);
void mdx_compiler_set_default_note_length(struct mdx_compiler *compiler, int chan_mask, struct mml_notelength *l);
void mdx_compiler_octave(struct mdx_compiler *compiler, int chan_mask, int octave);
void mdx_compiler_octave_down(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_octave_up(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_tempo(struct mdx_compiler *compiler, int chan_mask, int tempo, int at);
void mdx_compiler_rest(struct mdx_compiler *compiler, int chan_mask, struct mml_notelength *l);
void mdx_compiler_staccato(struct mdx_compiler *compiler, int chan_mask, int q, int at);
void mdx_compiler_portamento(struct mdx_compiler *compiler, int chan_mask, int note, struct mml_notelength *l, int note2);
void mdx_compiler_legato(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_set_voice(struct mdx_compiler *compiler, int chan_mask, int voice);
void mdx_compiler_volume(struct mdx_compiler *compiler, int chan_mask, int v, int at);
void mdx_compiler_volume_down(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_volume_up(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_pan(struct mdx_compiler *compiler, int chan_mask, int p);
void mdx_compiler_key_on_delay(struct mdx_compiler *compiler, int chan_mask, int k);
void mdx_compiler_loop_start(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_detune(struct mdx_compiler *compiler, int chan_mask, int16_t d);
void mdx_compiler_opm_write(struct mdx_compiler *compiler, int chan_mask, int r, int d);
void mdx_compiler_opm_noise_freq(struct mdx_compiler *compiler, int chan_mask, int w);
void mdx_compiler_adpcm_freq(struct mdx_compiler *compiler, int chan_mask, int f);
void mdx_compiler_sync_send(struct mdx_compiler *compiler, int chan_mask, int c);
void mdx_compiler_sync_wait(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_mh(struct mdx_compiler *compiler, int chan_mask, int lfo, int lfrq, int pmd, int amd, int pms, int ams, int sync);
void mdx_compiler_mhon(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_mhof(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_ma(struct mdx_compiler *compiler, int chan_mask, int waveform, int freq, int amplitude);
void mdx_compiler_maon(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_maof(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_mp(struct mdx_compiler *compiler, int chan_mask, int waveform, int freq, int amplitude);
void mdx_compiler_mpon(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_mpof(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_md(struct mdx_compiler *compiler, int chan_mask, int ticks);
void mdx_compiler_repeat_start(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_repeat_end(struct mdx_compiler *compiler, int chan_mask, int num_loops);
void mdx_compiler_repeat_escape(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_end(struct mdx_compiler *compiler, int chan_mask);
void mdx_compiler_add_opm_voice(struct mdx_compiler *, int num, uint8_t data[47]);

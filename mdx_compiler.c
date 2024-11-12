#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "mdx_compiler.h"
#include "mmlc.yy.h"
#include "midilib/buffer.h"

void mdx_compiler_init(struct mdx_compiler *compiler) {
	for(int i = 0; i < 16; i++) {
		struct mdx_compiler_channel *chan = &compiler->channels[i];
		buffer_init(&chan->buf);
		chan->total_ticks = 0;
		chan->default_note_length = 48;
		chan->octave = 4;
		memset(chan->repeat_stack, 0xff, sizeof(chan->repeat_stack)); // -1
		chan->repeat_stack_pos = 0;
		chan->loop_pos = -1;
	}
	memset(compiler->voices, 0, sizeof(compiler->voices));
	compiler->title = compiler->pcmfile = 0;
	compiler->ex_pcm = 0;
}

void mdx_compiler_destroy(struct mdx_compiler *compiler) {
	for(int i = 0; i < 16; i++) {
		struct mdx_compiler_channel *chan = &compiler->channels[i];
		buffer_destroy(&chan->buf);
	}
}

// https://stackoverflow.com/questions/48850242/thread-safe-reentrant-bison-flex
int mdx_compiler_parse(struct mdx_compiler *compiler, FILE *f) {
	yylex_init_extra(compiler, &compiler->scanner);
	yyset_in(f, compiler->scanner);
	yyparse(compiler->scanner, compiler);
	yylex_destroy(compiler->scanner);

	return 0;
}

void mdx_compiler_dump(struct mdx_compiler *compiler) {
	for(int i = 0; i < 16; i++) {
		printf("%2d: %5d: ", i, compiler->channels[i].total_ticks);
		buffer_dump(&compiler->channels[i].buf);
	}
}

void mdx_compiler_save(struct mdx_compiler *compiler, const char *filename) {
	FILE *f = fopen(filename, "wb");
	char buf[4] = { 0x0d, 0x0a, 0x1a, 0x00 };
	if(compiler->title)
		fwrite(compiler->title, 1, strlen(compiler->title), f);
	fwrite(buf, 1, 3, f);
	if(compiler->pcmfile)
		fwrite(compiler->pcmfile, 1, strlen(compiler->pcmfile), f);
	buf[0] = 0;
	fwrite(buf, 1, 1, f);
	int total_voices = 0;
	for(int i = 0; i < 256; i++) {
		if(compiler->voices[i].used) {
			total_voices++;
		}
	}
	int total_data_len = 0;
	int total_channels = compiler->ex_pcm ? 16 : 9;
	for(int i = 0; i < total_channels; i++) {
		buffer_write_uint8(&compiler->channels[i].buf, 0xf1);
		if(compiler->channels[i].loop_pos >= 0) {
			int16_t ofs = compiler->channels[i].loop_pos - compiler->channels[i].buf.data_len - 2;
			buffer_write_uint8(&compiler->channels[i].buf, ofs >> 8);
			buffer_write_uint8(&compiler->channels[i].buf, ofs & 0xff);
		} else {
			buffer_write_uint8(&compiler->channels[i].buf, 0x00);
		}
		total_data_len += compiler->channels[i].buf.data_len;
	}
	total_data_len += total_channels * 2 + 2;
	buf[0] = total_data_len >> 8;
	buf[1] = total_data_len;
	fwrite(buf, 1, 2, f);
	int o = 2 + total_channels * 2 ;
	for(int i = 0; i < total_channels; i++) {
		buf[0] = o >> 8;
		buf[1] = o;
		fwrite(buf, 1, 2, f);
		o += compiler->channels[i].buf.data_len;
	}
	for(int i = 0; i < total_channels; i++) {
		fwrite(compiler->channels[i].buf.data, 1, compiler->channels[i].buf.data_len, f);
	}
	for(int i = 0; i < 256; i++) {
		struct mdx_compiler_opm_voice *voice = &compiler->voices[i];
		if(!voice->used) continue;
		uint8_t buf[3 + 4 * 6];
		buf[0] = i;
		buf[1] = voice->fl_con;
		buf[2] = voice->slot_mask;
		uint8_t *b = buf + 3;
		int ops[4] = { 0, 2, 1, 3 };
		for(int j = 0; j < 4; j++) *b++ = voice->op[ops[j]].dt1_mul;
		for(int j = 0; j < 4; j++) *b++ = voice->op[ops[j]].tl;
		for(int j = 0; j < 4; j++) *b++ = voice->op[ops[j]].ks_ar;
		for(int j = 0; j < 4; j++) *b++ = voice->op[ops[j]].ame_d1r;
		for(int j = 0; j < 4; j++) *b++ = voice->op[ops[j]].dt2_d2r;
		for(int j = 0; j < 4; j++) *b++ = voice->op[ops[j]].d1l_rr;
		fwrite(buf, 1, 3 + 4 * 6, f);
	}
	fclose(f);
}

void mdx_compiler_directive(struct mdx_compiler *compiler, char *directive_str, char *value) {
	if(!strcasecmp(directive_str, "title")) {
		compiler->title = value;
	} else if(!strcasecmp(directive_str, "pcmfile")) {
		compiler->pcmfile = value;
	} else if(!strcasecmp(directive_str, "ex-pcm")) {
		compiler->ex_pcm = 1;
	} else fprintf(stderr, "Unknown directive: %s\n", directive_str);
}

void mdx_compiler_opmdef(struct mdx_compiler *compiler, int voice_num, uint8_t *data) {
	if(voice_num < 0) {
		fprintf(stderr, "Voice definition number %d cannot be less than 0\n", voice_num);
		return;
	} else if(voice_num > 255) {
		fprintf(stderr, "Voice definition number %d cannot be more than 255\n", voice_num);
		return;
	}

	struct mdx_compiler_opm_voice *voice = &compiler->voices[voice_num];
	voice->used = 1;
	voice->fl_con = ((data[45] & 0x07) << 3) | (data[44] & 0x07);
	voice->slot_mask = data[46] & 0x0f;
	for(int i = 0; i < 4; i++) {
		struct mdx_compiler_opm_voice_op *op = &voice->op[i];
		op->dt1_mul = (data[i * 11 + 7] & 0x0f) | ((data[i * 11 + 8] & 0x07) << 4);
		op->tl = data[i * 11 + 5] & 0x7f;
		op->ks_ar = (data[i * 11] & 0x1f) | ((data[i * 11 + 6] & 0x03) << 6);
		op->ame_d1r = ((data[i * 11 + 10] & 0x01) << 7) | (data[i * 11 + 1] & 0x1f);
		op->dt2_d2r = ((data[i * 11 + 9] & 0x03) << 6) | (data[i * 11 + 2] & 0x1f);
		op->d1l_rr = ((data[i * 11 + 4] & 0x0f) << 4) | (data[i * 11 + 3] & 0x0f);
	}
}

void mdx_compiler_write(struct mdx_compiler *compiler, int chan_mask, ...) {
	va_list ap;
	va_start(ap, chan_mask);
	while(1) {
		int c = va_arg(ap, int);
		if(c < 0) break;
		for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
			if(chan_mask & m) {
				struct mdx_compiler_channel *chan = &compiler->channels[i];
				buffer_write_uint8(&chan->buf, c);
			}
		}
	}
	va_end(ap);
}

void mdx_compiler_write16(struct mdx_compiler *compiler, int chan_mask, ...) {
	va_list ap;
	va_start(ap, chan_mask);
	while(1) {
		int c = va_arg(ap, int);
		if(c < 0) break;
		for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
			if(chan_mask & m) {
				struct mdx_compiler_channel *chan = &compiler->channels[i];
				buffer_write_big_uint16(&chan->buf, c);
			}
		}
	}
	va_end(ap);
}

static int mdx_compiler_calc_notelength(struct mml_notelength *l, int def) {
	if(!l) return def;

	switch(l->type) {
		case NoteLenTicks:
			return l->val;
		case NoteLenDot:
			return mdx_compiler_calc_notelength(l->n1, def) * 3 / 2;
		case NoteLenInt:
			return 192 / l->val;
		case NoteLenDefault:
			return def;
		case NoteLenJoin:
			return mdx_compiler_calc_notelength(l->n1, def) + mdx_compiler_calc_notelength(l->n2, def);
	}
	free(l);
	return 48;
}

static uint8_t mdx_compiler_note_value(int octave, int note) {
	// d+ is 0           a  b   c   d  e  f  g
	int note_table[] = { 6, 8, -3, -1, 1, 2, 4 };
	int n = note & 0xff;
	if(n < 0) n = 0;
	if(n > 6) n = 6;
	int val = octave * 12 + note_table[n] + ((note & MML_NOTE_SHARP) ? 1 : ((note & MML_NOTE_FLAT) ? -1 : 0));
	if(val < 0) return 0xff;
	if(val > 95) return 0xff;
	return val;
}

static int mdx_compiler_tempo_to_opm(int tempo) {
	return 256 - (78125 / (16 * tempo));
}

void mdx_compiler_tempo(struct mdx_compiler *compiler, int chan_mask, int tempo, int at) {
	if(!at && tempo < 19) {
		fprintf(stderr, "Tempo %d cannot be less than 19\n", tempo);
		return;
	} else if(!at && tempo > 4882) {
		fprintf(stderr, "Tempo %d cannot be more than 4882\n", tempo);
		return;
	}

	mdx_compiler_write(compiler, chan_mask, 0xff, at ? tempo : mdx_compiler_tempo_to_opm(tempo), -1);
}

void mdx_compiler_note(struct mdx_compiler *compiler, int chan_mask, int note, struct mml_notelength *l) {
	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if(chan_mask & m) {
			struct mdx_compiler_channel *chan = &compiler->channels[i];
			int note_value = mdx_compiler_note_value(chan->octave, note);
			if(note_value == 0xff) {
				fprintf(stderr, "Invalid note value o%d%c%c\n", chan->octave, 'a' + (note & 0xff), ((note & MML_NOTE_SHARP) ? '+' : (note & MML_NOTE_FLAT) ? '-' : ' '));
				continue;
			}
			buffer_write_uint8(&chan->buf, 0x80 + note_value);
			int ticks = mdx_compiler_calc_notelength(l, chan->default_note_length);
			buffer_write_uint8(&chan->buf, ticks - 1);
			chan->total_ticks += ticks;
		}
	}
}

void mdx_compiler_rest(struct mdx_compiler *compiler, int chan_mask, struct mml_notelength *l) {
	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if(chan_mask & m) {
			struct mdx_compiler_channel *chan = &compiler->channels[i];
			int ticks = mdx_compiler_calc_notelength(l, chan->default_note_length);
			buffer_write_uint8(&chan->buf, ticks-1);
			chan->total_ticks += ticks;
		}
	}
}

void mdx_compiler_set_default_note_length(struct mdx_compiler *compiler, int chan_mask, struct mml_notelength *l) {
	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if(chan_mask & m) {
			struct mdx_compiler_channel *chan = &compiler->channels[i];
			int ticks = mdx_compiler_calc_notelength(l, chan->default_note_length);
			chan->default_note_length = ticks;
		}
	}
}

void mdx_compiler_octave(struct mdx_compiler *compiler, int chan_mask, int octave) {
	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if(chan_mask & m) {
			struct mdx_compiler_channel *chan = &compiler->channels[i];
			chan->octave = octave;
		}
	}
}

void mdx_compiler_octave_down(struct mdx_compiler *compiler, int chan_mask) {
	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if(chan_mask & m) {
			struct mdx_compiler_channel *chan = &compiler->channels[i];
			chan->octave--;
			if(chan->octave < 0) chan->octave = 0;
		}
	}
}

void mdx_compiler_octave_up(struct mdx_compiler *compiler, int chan_mask) {
	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if(chan_mask & m) {
			struct mdx_compiler_channel *chan = &compiler->channels[i];
			chan->octave++;
			if(chan->octave > 8) chan->octave = 8;
		}
	}
}

void mdx_compiler_note_num(struct mdx_compiler *compiler, int chan_mask, int note, struct mml_notelength *l) {
	if(note < 0) {
		fprintf(stderr, "Invalid negative note %d\n", note);
		return;
	} else if(note > 95) {
		fprintf(stderr, "Invalid note %d > 95\n", note);
		return;
	}

	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if(chan_mask & m) {
			struct mdx_compiler_channel *chan = &compiler->channels[i];
			buffer_write_uint8(&chan->buf, 0x80 + note);
			int ticks = mdx_compiler_calc_notelength(l, chan->default_note_length);
			buffer_write_uint8(&chan->buf, ticks-1);
			chan->total_ticks += ticks;
		}
	}
}

void mdx_compiler_staccato(struct mdx_compiler *compiler, int chan_mask, int q, int at) {
	mdx_compiler_write(compiler, chan_mask, 0xf8, at ? 256 - q : q, -1);
}

void mdx_compiler_portamento(struct mdx_compiler *compiler, int chan_mask, int note, struct mml_notelength *l, int note2) {
	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if(chan_mask & m) {
			struct mdx_compiler_channel *chan = &compiler->channels[i];
			int ticks = mdx_compiler_calc_notelength(l, chan->default_note_length);
			int note_value_1 = mdx_compiler_note_value(chan->octave, note);
			if(note_value_1 == 0xff) {
				fprintf(stderr, "Invalid note value o%d%c%c\n", chan->octave, 'a' + (note & 0xff), ((note & MML_NOTE_SHARP) ? '+' : (note & MML_NOTE_FLAT) ? '-' : ' '));
				continue;
			}
			int note_value_2 = mdx_compiler_note_value(chan->octave, note2);
			if(note_value_2 == 0xff) {
				fprintf(stderr, "Invalid note value o%d%c%c\n", chan->octave, 'a' + (note2 & 0xff), ((note2 & MML_NOTE_SHARP) ? '+' : (note2 & MML_NOTE_FLAT) ? '-' : ' '));
				continue;
			}
			int portamento = 16384 * (note_value_2 - note_value_1) / (ticks);
			buffer_write_uint8(&chan->buf, 0xf2);
			buffer_write_big_int16(&chan->buf, portamento);
			buffer_write_uint8(&chan->buf, 0x80 + note_value_1);
			buffer_write_uint8(&chan->buf, ticks-1);
		}
	}
}

void mdx_compiler_legato(struct mdx_compiler *compiler, int chan_mask) {
	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if(chan_mask & m) {
			struct mdx_compiler_channel *chan = &compiler->channels[i];
			if(chan->buf.data_len > 2) {
				buffer_write_uint8(&chan->buf, chan->buf.data[chan->buf.data_len - 1]);
				chan->buf.data[chan->buf.data_len - 2] = chan->buf.data[chan->buf.data_len - 3];
				chan->buf.data[chan->buf.data_len - 3] = 0xf7;
			}
		}
	}
}

void mdx_compiler_volume(struct mdx_compiler *compiler, int chan_mask, int v, int at) {
	mdx_compiler_write(compiler, chan_mask, 0xfb, at ? 128 + v : v, -1);
}

void mdx_compiler_set_voice(struct mdx_compiler *compiler, int chan_mask, int voice) {
	if(voice > 255)
		fprintf(stderr, "Voice %d cannot be greater than 255\n", voice);
	mdx_compiler_write(compiler, chan_mask, 0xfd, voice, -1);
}

void mdx_compiler_end(struct mdx_compiler *compiler, int chan_mask) {
	mdx_compiler_write(compiler, chan_mask, 0xf1, 0x00, -1);
}

void mdx_compiler_volume_down(struct mdx_compiler *compiler, int chan_mask) {
	mdx_compiler_write(compiler, chan_mask, 0xfa, -1);
}

void mdx_compiler_volume_up(struct mdx_compiler *compiler, int chan_mask) {
	mdx_compiler_write(compiler, chan_mask, 0xf9, -1);
}

void mdx_compiler_pan(struct mdx_compiler *compiler, int chan_mask, int p) {
	mdx_compiler_write(compiler, chan_mask, 0xfc, p & 3, -1);
}

void mdx_compiler_key_on_delay(struct mdx_compiler *compiler, int chan_mask, int k) {
	mdx_compiler_write(compiler, chan_mask, 0xf0, k & 0xff, -1);
}

void mdx_compiler_detune(struct mdx_compiler *compiler, int chan_mask, int d) {
	mdx_compiler_write(compiler, chan_mask, 0xf3, d, -1);
}

void mdx_compiler_opm_noise_freq(struct mdx_compiler *compiler, int chan_mask, int w) {
	mdx_compiler_write(compiler, chan_mask, 0xed, 0x80 + w, -1);
}

void mdx_compiler_opm_write(struct mdx_compiler *compiler, int chan_mask, int r, int d) {
	mdx_compiler_write(compiler, chan_mask, 0xfe, r & 0xff, d & 0xff, -1);
}

void mdx_compiler_sync_send(struct mdx_compiler *compiler, int chan_mask, int c) {
	mdx_compiler_write(compiler, chan_mask, 0xef, c, -1);
}

void mdx_compiler_sync_wait(struct mdx_compiler *compiler, int chan_mask) {
	mdx_compiler_write(compiler, chan_mask, 0xee, -1);
}

void mdx_compiler_adpcm_freq(struct mdx_compiler *compiler, int chan_mask, int f) {
	mdx_compiler_write(compiler, chan_mask, 0xed, f & 0x07, -1);
}

void mdx_compiler_mh(struct mdx_compiler *compiler, int chan_mask, int lfo, int lfrq, int pmd, int amd, int pms, int ams, int sync) {
	mdx_compiler_write(
		compiler,
		chan_mask,
		0xea,
		(lfo & 0x03) | ((sync & 0x01) << 6),
		lfrq,
		(pmd & 0x7f) | 0x80,
		amd,
		((pms & 0x0f) << 4) | (ams & 0x0f),
		-1
	);
}

void mdx_compiler_mhon(struct mdx_compiler *compiler, int chan_mask) {
	mdx_compiler_write(compiler, chan_mask, 0xea, 0x81, -1);
}

void mdx_compiler_mhof(struct mdx_compiler *compiler, int chan_mask) {
	mdx_compiler_write(compiler, chan_mask, 0xea, 0x80, -1);
}

void mdx_compiler_ma(struct mdx_compiler *compiler, int chan_mask, int waveform, int freq, int amplitude) {
	mdx_compiler_write(compiler, chan_mask, 0xeb, waveform & 0x03, -1);
	mdx_compiler_write16(compiler, chan_mask, freq * 2, amplitude * 32, -1);
}

void mdx_compiler_maon(struct mdx_compiler *compiler, int chan_mask) {
	mdx_compiler_write(compiler, chan_mask, 0xeb, 0x81, -1);
}

void mdx_compiler_maof(struct mdx_compiler *compiler, int chan_mask) {
	mdx_compiler_write(compiler, chan_mask, 0xeb, 0x80, -1);
}

void mdx_compiler_mp(struct mdx_compiler *compiler, int chan_mask, int waveform, int freq, int amplitude) {
	mdx_compiler_write(compiler, chan_mask, 0xec, waveform & 0x03, -1);
	mdx_compiler_write16(compiler, chan_mask, freq * 2, amplitude * 128, -1);
}

void mdx_compiler_mpon(struct mdx_compiler *compiler, int chan_mask) {
	mdx_compiler_write(compiler, chan_mask, 0xec, 0x81, -1);
}

void mdx_compiler_mpof(struct mdx_compiler *compiler, int chan_mask) {
	mdx_compiler_write(compiler, chan_mask, 0xec, 0x80, -1);
}

void mdx_compiler_md(struct mdx_compiler *compiler, int chan_mask, int ticks) {
	mdx_compiler_write(compiler, chan_mask, 0xe9, ticks, -1);
}

void mdx_compiler_repeat_start(struct mdx_compiler *compiler, int chan_mask) {
	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if((chan_mask & m) == 0) continue;
		struct mdx_compiler_channel *channel = compiler->channels + i;
		channel->repeat_stack[channel->repeat_stack_pos].start_location = channel->buf.data_len;
		channel->repeat_stack_pos++;
		buffer_write_uint8(&channel->buf, 0xf6);
		buffer_write_uint8(&channel->buf, 0x00);
		buffer_write_uint8(&channel->buf, 0x00);
	}
}

void mdx_compiler_repeat_end(struct mdx_compiler *compiler, int chan_mask, int num_loops) {
	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if((chan_mask & m) == 0) continue;
		struct mdx_compiler_channel *channel = compiler->channels + i;
		channel->repeat_stack_pos--;
		if(channel->repeat_stack[channel->repeat_stack_pos].start_location >= 0) {
			channel->buf.data[channel->repeat_stack[channel->repeat_stack_pos].start_location + 1] = num_loops;
		}
		if(channel->repeat_stack[channel->repeat_stack_pos].escape_location >= 0) {
			int16_t escape_ofs = channel->buf.data_len - channel->repeat_stack[channel->repeat_stack_pos].escape_location + 1;
			channel->buf.data[channel->repeat_stack[channel->repeat_stack_pos].escape_location + 1] = escape_ofs >> 8;
			channel->buf.data[channel->repeat_stack[channel->repeat_stack_pos].escape_location + 2] = escape_ofs & 0xff;
		}
		buffer_write_uint8(&channel->buf, 0xf5);
		int16_t ofs = channel->repeat_stack[channel->repeat_stack_pos].start_location - channel->buf.data_len + 1;
		buffer_write_uint8(&channel->buf, ofs >> 8);
		buffer_write_uint8(&channel->buf, ofs & 0xff);
	}
}

void mdx_compiler_repeat_escape(struct mdx_compiler *compiler, int chan_mask) {
	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if((chan_mask & m) == 0) continue;
		struct mdx_compiler_channel *channel = compiler->channels + i;
		channel->repeat_stack[channel->repeat_stack_pos].escape_location = channel->buf.data_len;
		buffer_write_uint8(&channel->buf, 0xf4);
		buffer_write_uint8(&channel->buf, 0x00);
		buffer_write_uint8(&channel->buf, 0x00);
	}
}

void mdx_compiler_loop_start(struct mdx_compiler *compiler, int chan_mask) {
	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if((chan_mask & m) == 0) continue;
		struct mdx_compiler_channel *channel = compiler->channels + i;
		channel->loop_pos = channel->buf.data_len;
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "mml2mdx.h"
#include "buffer.h"

void mdx_compiler_init(struct mdx_compiler *compiler) {
	for(int i = 0; i < 16; i++) {
		struct mdx_compiler_channel *chan = &compiler->channels[i];
		buffer_init(&chan->buf);
		chan->total_ticks = 0;
		chan->default_note_length = 4;
		chan->octave = 4;
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
		buffer_write_uint8(&compiler->channels[i].buf, 0x00);
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
		// printf("dt1_mul=%02x tl=%02x ks_ar=%02x ame_d1r=%02x dt2_d2r=%02x, d1l_rr=%02x\n", voice->op[j].dt1_mul, voice->op[j].tl, voice->op[j].ks_ar, voice->op[j].ame_d1r, voice->op[j].dt2_d2r, voice->op[j].d1l_rr);
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

void mdx_compiler_directive(struct mdx_compiler *compiler, char *directive, char *value) {
	if(!strcasecmp(directive, "title")) {
		compiler->title = value;
	} else if(!strcasecmp(directive, "pcmfile")) {
		compiler->pcmfile = value;
	} else if(!strcasecmp(directive, "ex-pcm")) {
		compiler->ex_pcm = 1;
	} else fprintf(stderr, "Unknown directive: %s\n", directive);
}

void mdx_compiler_opmdef(struct mdx_compiler *compiler, int voice_num, uint8_t *data) {
	if(voice_num < 0 || voice_num > 255) return;

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

static int mdx_compiler_calc_notelength(struct mml_notelength *l, int def) {
	if(!l) return 192 / def;

	switch(l->type) {
		case NoteLenTicks:
			return l->val;
		case NoteLenDot:
			return mdx_compiler_calc_notelength(l->n1, def) * 3 / 2;
		case NoteLenInt:
			return 192 / l->val;
		case NoteLenDefault:
			return 192 / def;
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
	if(val < 0) val = 0;
	if(val > 95) val = 95;
	return val;
}

static int mdx_compiler_tempo_to_opm(int tempo) {
	return 256 - (78125 / (16 * tempo));
}

void mdx_compiler_tempo(struct mdx_compiler *compiler, int chan_mask, int tempo, int at) {
	mdx_compiler_write(compiler, chan_mask, 0xff, at ? tempo : mdx_compiler_tempo_to_opm(tempo), -1);
}

void mdx_compiler_note(struct mdx_compiler *compiler, int chan_mask, int note, struct mml_notelength *l) {
	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if(chan_mask & m) {
			struct mdx_compiler_channel *chan = &compiler->channels[i];
			int note_value = mdx_compiler_note_value(chan->octave, note);
			buffer_write_uint8(&chan->buf, 0x80 + note_value);
			int ticks = mdx_compiler_calc_notelength(l, chan->default_note_length);
			buffer_write_uint8(&chan->buf, ticks-1);
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

void mdx_compiler_set_default_note_length(struct mdx_compiler *compiler, int chan_mask, int l) {
	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if(chan_mask & m) {
			struct mdx_compiler_channel *chan = &compiler->channels[i];
			chan->default_note_length = l;
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
	mdx_compiler_write(compiler, chan_mask, 0xf8, at ? 128 + q : q, -1);
}

void mdx_compiler_portamento(struct mdx_compiler *compiler, int chan_mask, int note, struct mml_notelength *l, int note2) {
	for(int i = 0, m = 1; i < 16; i++, m <<= 1) {
		if(chan_mask & m) {
			struct mdx_compiler_channel *chan = &compiler->channels[i];
			int ticks = mdx_compiler_calc_notelength(l, chan->default_note_length);
			int note_value_1 = mdx_compiler_note_value(chan->octave, note);
			int note_value_2 = mdx_compiler_note_value(chan->octave, note2);
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

void mdx_compiler_loop_start(struct mdx_compiler *compiler, int chan_mask) {

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

void mdx_compiler_opm_lfo(struct mdx_compiler *compiler, int chan_mask, int lfo, int lfrq, int pmd, int amd, int pms, int ams, int sync) {

}

void mdx_compiler_mhon(struct mdx_compiler *compiler, int chan_mask) {

}

void mdx_compiler_mhof(struct mdx_compiler *compiler, int chan_mask) {

}

void mdx_compiler_ma(struct mdx_compiler *compiler, int chan_mask, int waveform, int freq, int amplitude) {

}

void mdx_compiler_maon(struct mdx_compiler *compiler, int chan_mask) {

}

void mdx_compiler_maof(struct mdx_compiler *compiler, int chan_mask) {

}

void mdx_compiler_mp(struct mdx_compiler *compiler, int chan_mask, int waveform, int freq, int amplitude) {

}

void mdx_compiler_mpon(struct mdx_compiler *compiler, int chan_mask) {

}

void mdx_compiler_mpof(struct mdx_compiler *compiler, int chan_mask) {

}

void mdx_compiler_md(struct mdx_compiler *compiler, int chan_mask) {

}

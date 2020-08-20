#include <string.h>
#include <stdio.h>
#include "mml.h"
#include "sjis.h"

static char channel_name(int c) {
	return c < 8 ? 'A' + c : (c < 16 ? 'P' + c - 8 : '?');
}

static int ticks_to_division(int t) {
	switch(t) {
		case 192: return 1;
		case  96: return 2;
		case  64: return 3;
		case  48: return 4;
		case  32: return 6;
		case  24: return 8;
		case  16: return 12;
		case  12: return 16;
		case   6: return 32;
		case   4: return 48;
		case   3: return 64;
		case   2: return 96;
		case   1: return 192;
	}
	return 0;
}

static int note_octave(int n) {
	return (n + 3) / 12;
}

#define PRINTLINE(s) d->line(s); s[0] = 0; d->cur_col = 0;
#define PRINTMMLLINE(s) memmove(s + 2, s, d->columns - 2); s[0] = channel_name(i); s[1] = ' ';  PRINTLINE(s);
#define LINEF(fmt...) { snprintf(d->buf, d->columns, fmt); PRINTLINE(d->buf) }
#define MMLF(fmt...) { \
	char buf[40]; \
	snprintf(buf, sizeof(buf), fmt); \
	int l = strlen(buf); \
	if(l + d->cur_col >= d->columns - 2) { \
		PRINTMMLLINE(d->buf) \
	} \
	strncat(d->buf, buf, d->columns - d->cur_col); \
	d->cur_col += l; \
}

void mdx_decompiler_init(struct mdx_decompiler *d, char *buf, int columns) {
	memset(d, 0, sizeof(*d));
	d->buf = buf;
	d->columns = columns;
	d->cur_col = 0;

	d->rest_ticks = 0;
	d->next_key_off = 0;
	d->octave = 4;
}

void mdx_decompiler_decompile(struct mdx_decompiler *d, struct mdx_file *f) {
	uint8_t utfbuf[256];
	char titlebuf[280];
	int l;
	if(f->title_len > 0) {
		memset(utfbuf, 0, sizeof(utfbuf));
		l = sjis_to_utf8(f->title, f->title_len, utfbuf, sizeof(utfbuf));
		if(l > 0) {
			snprintf(titlebuf, sizeof(titlebuf), "#title \"%s\"", utfbuf);
			d->line(titlebuf);
		}
	}

	if(f->pdx_filename_len > 0) {
		memset(utfbuf, 0, sizeof(utfbuf));
		l = sjis_to_utf8(f->pdx_filename, f->pdx_filename_len, utfbuf, sizeof(utfbuf));
		if(l > 0) {
			snprintf(titlebuf, sizeof(titlebuf), "#pcmfile \"%s\"", utfbuf);
			d->line(titlebuf);
		}
	}

	int ops[4] = { 0, 2, 1, 3 };
	for(int i = 0; i < 256; i++) {
		if(f->voices[i]) {
			uint8_t *v = f->voices[i];
			uint8_t *op = v + 3;

			LINEF("@%d = {", i);
			LINEF("/* AR D1R D2R  RR D1L   TL KS MUL DT1 DT2 AME */");
			for(int j = 0; j < 4; j++) {
				int o = ops[j];
				LINEF(
					"   %2d, %2d, %2d, %2d, %2d, %3d, %d, %2d, %2d, %2d,  %d,",
					op[0x08 + o] & 0x1f,      // AR  0-31
					op[0x0c + o] & 0x1f,      // D1R 0-31
					op[0x10 + o] & 0x1f,      // D2R 0-31
					op[0x14 + o] & 0x0f,      // RR  0-15
					op[0x14 + o] >> 4,        // D1L 0-15
					op[0x04 + o] & 0x7f,      // TL  0-127
					op[0x08 + o] >> 6,        // KS  0-3
					op[0x00 + o] & 0x0f,      // MUL 0-15
					op[0x00 + o] >> 4 & 0x07, // DT1 0-7
					op[0x10 + o] >> 6,        // DT2 0-3
					op[0x0c + o] >> 7         // AME 0-1
				);
			}
			LINEF("/* CON FL MASK */");
			LINEF("    %d,  %d,  %2d", v[1] & 0x07, v[1] >> 3 & 0x07, v[2] & 0x0f);
			LINEF("}");
		}
	}

	const char *note_names[12] = { "d+", "e", "f", "f+", "g", "g+", "a", "a+", "b", "c", "c+", "d" };
	for(int i = 0; i < f->num_channels; i++) {
		d->octave = 4;
		d->rest_ticks = 0;
		d->next_key_off = 0;
		LINEF("/* Channel %c */", channel_name(i));
		struct mdx_channel *chan = &f->channels[i];
		int pos = 0;

		// run through the entire channel once to find the loop point
		int loop_point = -1;
		while(1) {
			int r = mdx_cmd_len(chan->data, pos, chan->data_len);
			if(r <= 0) {
				// invalid command
				break;
			}
			// at this point we don't need to verify command length anymore
			uint8_t *b = chan->data + pos;
			if(b[0] == 0xf1) {
				if(b[1] != 0x00) {
					loop_point = b[1] << 8 | b[2];
					if(loop_point > 32768) loop_point -= 65533;
					loop_point += pos;
					break;
				}
				break;
			}

			pos += r;
		}

		// run through again and output MML
		pos = 0;
		while(1) {
			int r = mdx_cmd_len(chan->data, pos, chan->data_len);
			if(r <= 0) {
				// invalid command
				break;
			}
			// at this point we don't need to verify command length anymore
			uint8_t *b = chan->data + pos;
			if(b[0] == 0xf1) {
				// performance end
				break;
			}

			if(pos == loop_point) {
				MMLF(" L ")
				MMLF("o%d", d->octave)
			}

			if(*b < 0x80) {
				d->rest_ticks += *b + 1;
			} else if(*b < 0xe0) {
				if(d->rest_ticks > 0) {
					int t = ticks_to_division(d->rest_ticks);
					if(t)
						MMLF("r%d", t)
					else
						MMLF("r%%%d", d->rest_ticks)
					d->rest_ticks = 0;
				}
				int ticks = b[1] + 1;
				int t = d->ticks_only ? 0 : ticks_to_division(ticks);
				int n = b[0] - 0x80;
				int o = note_octave(n);
				if((i >= 8 && !d->adpcm_notes) || (i < 8 && d->fm_note_nums)) {
					if(t)
						MMLF("n%d,%d", n, t)
					else
						MMLF("n%d,%%%d", n, b[1] + 1)
				} else {
					if(o != d->octave) {
						if(o - d->octave == 1)
							MMLF(">")
						else if(d->octave - o == 1)
							MMLF("<")
						else
							MMLF("o%d", o);
						d->octave = o;
					}
					if(t)
						MMLF("%s%d", note_names[n % 12], t)
					else
						MMLF("%s%%%d", note_names[n % 12], b[1] + 1)
				}
				if(d->portamento && i < 8) {
					MMLF("_");
					int next_note = n + d->portamento * ticks / 16384;
					o = note_octave(next_note);
					if(o != d->octave) {
						if(o - d->octave == 1)
							MMLF(">")
						else if(d->octave - o == 1)
							MMLF("<")
						else
							MMLF("o%d", o);
						d->octave = o;
					}
					if(d->fm_note_nums)
						MMLF("n%d", next_note)
					else
						MMLF("%s", note_names[n%12]);
					d->portamento = 0;
				}
				if(d->next_key_off) {
					MMLF("&")
					d->next_key_off = 0;
				}
			} else switch(*b) {
				case 0xff:
					MMLF("@t%d", b[1])
					break;
				case 0xfe:
					MMLF("y%d,%d", b[1], b[2])
					break;
				case 0xfd:
					MMLF("@%d", b[1])
					break;
				case 0xfc:
					MMLF("p%d", b[1])
					break;
				case 0xfb:
					if(b[1] < 16)
						MMLF("v%d", b[1])
					else
						MMLF("@v%d", 255 - b[1])
					break;
				case 0xfa:
					MMLF("(")
					break;
				case 0xf9:
					MMLF(")")
					break;
				case 0xf8:
					MMLF("q%d", b[1])
					break;
				case 0xf7:
					d->next_key_off = 1;
					break;
				case 0xf6:
					MMLF("[")
					MMLF("o%d", d->octave)
					break;
				case 0xf5:
					{
						int ofs = ((b[1] << 8) | b[2]) - 65535;
						if(ofs < 0 && pos + ofs >= 0) {
							MMLF("]%d", chan->data[pos + ofs])
							MMLF("o%d", d->octave)
						}
					}
					break;
				case 0xf4:
					MMLF("/")
					break;
				case 0xf3:
					{
						int detune = b[1] << 8 | b[2];
						if(detune >= 32768) detune -= 65536;
						MMLF("D%d", detune)
					}
					break;
				case 0xf2:
					d->portamento = b[1] << 8 | b[2];
					if(d->portamento >= 32768) d->portamento = d->portamento - 65536;
					break;
				case 0xf0:
					MMLF("k%d", b[1])
					break;
				case 0xef:
					MMLF("S%d", b[1])
					break;
				case 0xee:
					MMLF("W")
					break;
				case 0xed:
					if(i < 8)
						MMLF("w%d", b[1] & 0x1f)
					else
						MMLF("F%d", b[1])
					break;
				case 0xec:
					if(b[1] == 0x80)
						MMLF("MPOF")
					else if(b[1] == 0x81)
						MMLF("MPON")
					else
						MMLF("MP%d,%d,%d", b[1], b[2] << 8 | b[3], b[4] << 8 | b[5])
					break;
				case 0xeb:
					if(b[1] == 0x80)
						MMLF("MAOF")
					else if(b[1] == 0x81)
						MMLF("MAON")
					else
						MMLF("MA%d,%d,%d", b[1], b[2] << 8 | b[3], b[4] << 8 | b[5])
					break;
				case 0xea:
					if(b[1] == 0x80)
						MMLF("MHOF")
					else if(b[1] == 0x81)
						MMLF("MHON")
					else
						MMLF("MH%d,%d,%d,%d,%d,%d,%d", b[1] & 0x03, b[2], b[3] & 0x7f, b[4], b[5] >> 4, b[5] & 0x0f, b[1] >> 6)
					break;
				case 0xe9:
					MMLF("MD%d", b[1])
					break;
				case 0xe8:
					break;
				case 0xe7:
					break;
			}
			pos += r;
		}

		// print out any remaining stuff
		if(d->cur_col > 0) {
			PRINTMMLLINE(d->buf)
		}
	}
}

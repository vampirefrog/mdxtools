#include <stdio.h>
#include <string.h>

#include "mdx.h"
#include "tools.h"

int mdx_cmd_len(uint8_t *data, int pos, int len) {
	if(len <= 0) return -1;

	uint8_t c = data[pos];
	if(c <= 0x7f) return 1;
	if(c <= 0xdf) return 2;

	if(c == 0xea || c == 0xeb || c == 0xec) {
		if(len < 2) return -1;
//		printf("hello %02x %02x len=%d\n", c, len, data[pos+1]);
		if(data[pos+1] == 0x80 || data[pos+1] == 0x81) return 2;
		if(len < 6) return -1;
		return 6;
	}

	switch(c) {
		case 0xfe:
		case 0xf6:
		case 0xf5:
		case 0xf4:
		case 0xf3:
		case 0xf2:
		case 0xf1:
		case 0xe7:
			return 3;
		case 0xff:
		case 0xfd:
		case 0xfc:
		case 0xfb:
		case 0xf8:
		case 0xf0:
		case 0xef:
		case 0xed:
		case 0xe9:
			return 2;
	}
	return 1;
}

int mdx_file_load(struct mdx_file *f, uint8_t *data, int len) {
	int i;

	f->data = data;
	f->data_len = len;
	f->title_len = 0;
	f->title = 0;
	f->pdx_filename_len = 0;
	f->pdx_filename = 0;
	// load title
	for(i = 2; i < len; i++) {
		if(f->data[i] == 0x1a && f->data[i-1] == 0x0a && f->data[i-2] == 0x0d) {
			f->title = f->data;
			f->title_len = i-2;
			i++;
			break;
		}
	}

	if(f->title == 0) {
		return MDX_ERR_BAD_TITLE;
	}

	// load PDX filename
	int pdxstart = i;
	for(; i < len; i++) {
		if(f->data[i] == 0) {
			f->pdx_filename = f->data + pdxstart;
			f->pdx_filename_len = i - pdxstart;
			i++;
			break;
		}
	}

	if(f->pdx_filename == 0) {
		return MDX_ERR_BAD_PCM_FILENAME;
	}

	int offsetstart = i;
	f->data_start_ofs = offsetstart;
	if(data[offsetstart + 4] == 'L' &&
		data[offsetstart + 5] == 'Z' &&
		data[offsetstart + 6] == 'X') {
		return MDX_ERR_LZX;
	}

	// 1 chunk for OPM voices and 16 chunks for channel data
	struct {
		int offset, len;
	} chunks[17];
	// read them in and set length to the whole rest of the file (offset to EOF)
	int min_ofs = len - offsetstart;
	for(int i = 0; i < 17; i++) {
		chunks[i].offset = (data[offsetstart + i * 2] << 8) | data[offsetstart + i * 2 + 1];
		if(chunks[i].offset + offsetstart >= len) {
			// invalidate offsets outside the file
			chunks[i].len = 0;
		} else {
			chunks[i].len = len - offsetstart - chunks[i].offset;
			if(i < 10 && chunks[i].offset < min_ofs) min_ofs = chunks[i].offset;
		}
	}
	f->num_channels = (min_ofs - 2) / 2;
	if(f->num_channels > 16) f->num_channels = 16;
	// calculate lengths
	for(int i = 0; i < 17; i++) {
		if(!chunks[i].len) continue;
		if(i > f->num_channels + 1) {
			chunks[i].len = 0;
			continue;
		}
		for(int j = 0; j <= f->num_channels; j++) {
			if(!chunks[j].len) continue;
			if(chunks[i].offset < chunks[j].offset && chunks[i].len > chunks[j].offset - chunks[i].offset)
				chunks[i].len = chunks[j].offset - chunks[i].offset;
		}
	}

	// check for overlaps and invalidate
	for(int i = 0; i <= f->num_channels; i++) {
		if(!chunks[i].len) continue;
		for(int j = 0; j <= f->num_channels; j++) {
			if(!chunks[j].len) continue;
			if(chunks[i].offset > chunks[j].offset && chunks[i].offset < chunks[j].offset + chunks[j].len) {
				chunks[i].len = 0;
				continue;
			}
		}
	}

	uint16_t voice_data_offset = offsetstart + chunks[0].offset;
	uint16_t voice_data_len = chunks[0].len;

	memset(f->channels, 0, sizeof(f->channels));
	for(int i = 0; i < f->num_channels; i++) {
		f->channels[i].data = f->data + offsetstart + chunks[i+1].offset;
		f->channels[i].data_len = chunks[i+1].len;
	}

	// load @voices
	memset(f->voices, 0, sizeof(f->voices));
	f->num_voices = voice_data_len / 27;
	for(int i = 0; i < f->num_voices; i++) {
		uint8_t *vptr = f->data + voice_data_offset + i * 27;
		// printf("voice_data_offset=%d voice_data_len=%d voices[%d].offset = %p\n",
		// 	voice_data_offset, voice_data_len, i, vptr);
		f->voices[f->data[voice_data_offset + i * 27]] = vptr;
	}

	return 0;
}

const char *mdx_lfo_waveform_name(uint8_t waveform) {
	const char *waveform_names[] = {
		"sawtooth", "square", "triangle"
	};
	return waveform <= 2 ? waveform_names[waveform] : "invalid";
}

const char mdx_channel_name(uint8_t chan) {
	if(chan < 8) return 'A' + chan;
	if(chan < 16) return 'P' + chan - 8;
	return '!';
}

const char *mdx_error_name(int err) {
	const char *names[] = {
		"Success",
		"MDX Title does not end before EOF",
		"PDX ending zero not found before EOF",
		"File is LZX compressed"
	};
	if(err >= 0 && err < MDX_MAX_ERR) {
		return names[err];
	}
	return "Unknown";
}

const char *mdx_command_name(uint8_t c) {
	const char *cmdNames[] = {
		"Informal",           // 0xe6
		"Extended MML",       // 0xe7
		"PCM4/8 enable",      // 0xe8
		"LFO delay",          // 0xe9
		"OPM LFO",            // 0xea
		"Amplitude LFO",      // 0xeb
		"Pitch LFO",          // 0xec
		"ADPCM/noise freq",   // 0xed
		"Sync wait",          // 0xee
		"Sync send",          // 0xef
		"Key on delay",       // 0xf0
		"Data end",           // 0xf1
		"Portamento",         // 0xf2
		"Detune",             // 0xf3
		"Repeat escape",      // 0xf4
		"Repeat end",         // 0xf5
		"Repeat start",       // 0xf6
		"Disable key-off",    // 0xf7
		"Sound length",       // 0xf8
		"Volume dec",         // 0xf9
		"Volume inc",         // 0xfa
		"Set volume",         // 0xfb
		"Output phase",       // 0xfc
		"Set voice #",        // 0xfd
		"Set OPM reg",        // 0xfe
		"Set tempo",          // 0xff
	};
	if(c >= 0xe6 && c <= 0xff) return cmdNames[c - 0xe6];
	return "Unknown";
}

const char *mdx_note_name(int note) {
	const char *noteNames[] = { "c", "c+", "d", "d+" , "e", "f", "f+", "g", "g+", "a", "a+", "b",  };
	return noteNames[(note + 3) % 12];
}

int mdx_note_octave(int note) {
	return (note + 3) / 12;
}

uint8_t mdx_voice_get_id(uint8_t *v) {
	return v[0];
}

uint8_t mdx_voice_get_fl(uint8_t *v) {
	return v[1] >> 3 & 0x07;
}

uint8_t mdx_voice_get_con(uint8_t *v) {
	return v[1] & 0x07;
}

uint8_t mdx_voice_get_slot_mask(uint8_t *v) {
	return v[2] & 0x0f;
}

uint8_t mdx_voice_osc_get_dt1(uint8_t *v, int osc) {
	return v[3 + osc] >> 4 & 0x07;
}

uint8_t mdx_voice_osc_get_mul(uint8_t *v, int osc) {
	return v[3 + osc] & 0x0f;
}

uint8_t mdx_voice_osc_get_tl(uint8_t *v, int osc) {
	return v[7 + osc] & 0x7f;
}

uint8_t mdx_voice_osc_get_ks(uint8_t *v, int osc) {
	return v[11 + osc] >> 6;
}

uint8_t mdx_voice_osc_get_ar(uint8_t *v, int osc) {
	return v[11 + osc] & 0x1f;
}

uint8_t mdx_voice_osc_get_ame(uint8_t *v, int osc) {
	return v[15 + osc] >> 7;
}

uint8_t mdx_voice_osc_get_d1r(uint8_t *v, int osc) {
	return v[15 + osc] & 0x1f;
}

uint8_t mdx_voice_osc_get_dt2(uint8_t *v, int osc) {
	return v[19 + osc] >> 6;
}

uint8_t mdx_voice_osc_get_d2r(uint8_t *v, int osc) {
	return v[19 + osc] & 0x1f;
}

uint8_t mdx_voice_osc_get_d1l(uint8_t *v, int osc) {
	return v[23 + osc] >> 4;
}

uint8_t mdx_voice_osc_get_rr(uint8_t *v, int osc) {
	return v[23 + osc] & 0x0f;
}

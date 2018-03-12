#include <stdio.h>
#include <string.h>

#include "mdx_driver.h"
#include "tools.h"

static int mdx_note_to_opm(int note) {
	uint8_t tbl[] = {
		0x0, // D#
		0x1, // E
		0x2, // F
		0x4, // F#
		0x5, // G
		0x6, // G#
		0x8, // A
		0x9, // A#
		0xA, // B
		0xC, // C
		0xD, // C#
		0xE, // D
	};
	return (note / 12) * 16 + tbl[note % 12];
}

static uint8_t mdx_volume_to_opm(uint8_t vol) {
	const uint8_t voltbl[] = {
		0x2A, 0x28, 0x25, 0x22,
		0x20, 0x1D, 0x1A, 0x18,
		0x15, 0x12, 0x10, 0x0D,
		0x0A, 0x08, 0x05, 0x02,
	};

	if(vol <= 15)
		return voltbl[vol];
	else if(vol >= 128)
		return vol - 128;

	return 0;
}

static void mdx_lfo_init(struct mdx_lfo *lfo, uint8_t waveform, uint16_t period, int16_t amplitude) {
	// printf(
	// 	"LFO START %d (%s) %d %d (%d.%d)\n",
	// 	waveform, mdx_lfo_waveform_name(waveform),
	// 	period, amplitude, amplitude >> 8,
	// 	(amplitude & 0xff) * 100 / 256
	// );
	lfo->enable = 1;
	lfo->waveform = waveform;
	lfo->period = period;
	lfo->amplitude = amplitude;
}

static void mdx_lfo_start(struct mdx_lfo *lfo) {
	lfo->phase = 0;
	lfo->pitch = 0;
	if(lfo->waveform == 0 || lfo->waveform == 2) {
		lfo->phase = lfo->period / 2;
	}
	if(lfo->waveform == 2) {
		lfo->pitch = lfo->amplitude * 2;
	}
}

static int mdx_lfo_tick(struct mdx_lfo *lfo) {
	if(lfo->enable && lfo->period > 0) {
		lfo->phase++;
		int phase_reset = 0;
		if(lfo->phase >= lfo->period) {
			phase_reset = 1;
			lfo->phase = 0;
		}
		if(lfo->waveform == 0) { // sawtooth
			if(phase_reset)
				lfo->pitch = -lfo->amplitude * lfo->period / 2;
			else
				lfo->pitch += lfo->amplitude;
		} else if(lfo->waveform == 1) { // square
			lfo->pitch = lfo->amplitude;
			if(phase_reset)
				lfo->amplitude = -lfo->amplitude;
		} else if(lfo->waveform == 2) { // triangle
			if(phase_reset)
				lfo->amplitude = -lfo->amplitude;
			lfo->pitch += lfo->amplitude;
		}
	}
	return lfo->pitch;
}

static void mdx_driver_channel_init(struct mdx_driver_channel *chan, struct mdx_channel *mchan) {
	memset(chan, 0, sizeof(*chan));
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

	memset(chan->repeat_stack, 0, sizeof(chan->repeat_stack));
}

static void mdx_driver_write_opm(struct mdx_driver *r, uint8_t reg, uint8_t val) {
	if(reg > 0x20 && r->opm_cache[reg] == val)
		return;
	r->opm_cache[reg] = val;
	r->write_opm(r, reg, val, r->data_ptr);
}

static void mdx_driver_channel_inc_volume(struct mdx_driver_channel *chan) {
	if(chan->volume < 15) chan->volume++;
	else if(chan->volume > 128) chan->volume--;
}

static void mdx_driver_channel_dec_volume(struct mdx_driver_channel *chan) {
	if(chan->volume < 16 && chan->volume > 0) chan->volume--;
	else if(chan->volume >= 128 && chan->volume < 255) chan->volume++;
}

static void mdx_driver_set_pitch(struct mdx_driver *r, int chan_num, int pitch) {
	mdx_driver_write_opm(r, 0x30 + chan_num, (pitch >> 6) & 0xfc);
	mdx_driver_write_opm(r, 0x28 + chan_num, mdx_note_to_opm(pitch >> 14));
}

static void mdx_driver_set_tl(struct mdx_driver *r, int chan_num, uint8_t *v, int volume) {
	const uint8_t con_masks[8] = {
		0x08, 0x08, 0x08, 0x08, 0x0c, 0x0e, 0x0e, 0x0f,
	};
	int mask = 1;
	for(int i = 0; i < 4; i++, mask <<= 1) {
		if((con_masks[v[1] & 0x07] & mask) > 0) {
			int vol = volume + v[7 + i];
			if(vol > 0x7f) vol = 0x7f;
			mdx_driver_write_opm(r, 0x60 + i * 8 + chan_num, vol); // TL
		} else {
			mdx_driver_write_opm(r, 0x60 + i * 8 + chan_num, v[7+i]); // TL
		}
	}
}

static void mdx_driver_end_channel(struct mdx_driver *r, int chan_num) {
	struct mdx_driver_channel *chan = &r->channels[chan_num];
	chan->ended = 1;
	r->ended = 1;
	for(int i = 0; i < 16; i++) {
		if(r->channels[i].used && !r->channels[i].ended) {
			r->ended = 0;
			break;
		}
	}
}

static uint8_t mdx_adpcm_volume_from_opm(uint8_t volume) {
	const uint8_t pcm_vol_table[] = { // ripped out of MXDRV
		0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e, 0x0d, 0x0d,
		0x0d, 0x0c, 0x0c, 0x0b, 0x0b, 0x0b, 0x0a, 0x0a,
		0x0a, 0x09, 0x09, 0x08, 0x08, 0x08, 0x07, 0x07,
		0x07, 0x06, 0x06, 0x05, 0x05, 0x05, 0x04, 0x04,
		0x04, 0x03, 0x03, 0x02, 0x02, 0x02, 0x01, 0x01,
		0x01, 0x00, 0x00
	};

	if(volume < sizeof(pcm_vol_table))
		return pcm_vol_table[volume];

	return 0;
}

void mdx_driver_start_fadeout(struct mdx_driver *r, int rate) {
	if(r->fade_rate != 0) return; // already fading out

	r->fade_rate = rate / 2 + 1;
	r->fade_counter = r->fade_rate;
	r->fade_value = 0; // attenuation
}

static void mdx_driver_note_on(struct mdx_driver *r, int chan_num) {
	if((r->channel_mask & (1 << chan_num)) == 0) return;

	struct mdx_driver_channel *chan = &r->channels[chan_num];
	// printf("%d: ON note=%d, ticks=%d pitch=%d detune=%d skipNoteOn=%d skipNoteOff=%d\n", chan_num, chan->note, chan->ticks_remaining, chan->pitch, (chan->pitch >> 8) & 0x3f, chan->skipNoteOn, chan->skipNoteOff);
	if(chan_num < 8) {
		if(chan->voice_num >= 0) {
			uint8_t *v = r->f->voices[chan->voice_num];
			if(v) {
				mdx_driver_set_pitch(r, chan_num, chan->pitch);
				mdx_driver_set_tl(r, chan_num, v, chan->opm_volume + r->fade_value);
				if(chan->skipNoteOn) {
					chan->skipNoteOn = 0;
				} else {
					if(chan->mhon) {
						mdx_driver_write_opm(r, 0x38, chan->pms_ams);
						if(chan->keysync) {
							mdx_driver_write_opm(r, 0x01, 0x02);
							mdx_driver_write_opm(r, 0x01, 0x00);
						}
					}

					if(chan->pitch_lfo.enable) {
						if(chan->lfo_delay) {
							chan->lfo_delay_counter = chan->lfo_delay;
						} else {
							mdx_lfo_start(&chan->pitch_lfo);
						}
					}

					mdx_driver_write_opm(r, 0x08, ((v[2] & 0x0f) << 3) | (chan_num & 0x07)); // Key On
				}
			} else printf("Note on !v %d\n", chan_num);
		}
	} else {
		if(chan->note < 96) {
			struct pdx_sample *s = &r->f->pdx.samples[chan->note];
			if(s->data && s->len) {
				adpcm_mixer_play(r->adpcm_mixer, chan_num - 8, s->decoded_data, s->len, chan->adpcm_freq_num, mdx_adpcm_volume_from_opm(chan->opm_volume + r->fade_value), chan->pan);
			}
		}
	}
}

static void mdx_driver_note_off(struct mdx_driver *r, int chan_num) {
	if((r->channel_mask & (1 << chan_num)) == 0) return;

	struct mdx_driver_channel *chan = &r->channels[chan_num];
	if(chan->note >= 0) { // key off
		//printf("%d: OFF %d\n", chan_num, chan->note);
		if(chan_num < 8) {
			if(chan->skipNoteOff) {
				chan->skipNoteOn = 1;
				chan->skipNoteOff = 0;
			} else {
				mdx_driver_write_opm(r, 0x08, chan_num & 0x07);
			}
			//printf("Note OFF skip On=%d Off=%d\n", chan->skipNoteOn, chan->skipNoteOff);
		} else {
			adpcm_mixer_stop(r->adpcm_mixer, chan_num - 8);
		}
		chan->note = -1;
	}
}

static int mdx_driver_advance_channel(struct mdx_driver *r, int chan_num) {
	struct mdx_driver_channel *chan = &r->channels[chan_num];
	uint8_t c = chan->data[chan->pos];
	if(chan->pos >= chan->len) {
		mdx_driver_end_channel(r, chan_num);
		return 0;
	}

	mdx_driver_note_off(r, chan_num);

	if(c <= 0x7f) {
		//printf("rest %d\n", c + 1);
		chan->note = -1;
		chan->ticks_remaining = c + 1;
		chan->pos++;
		return chan->ticks_remaining;
	} else if(c <= 0xdf) {
		chan->ticks_remaining = chan->data[chan->pos + 1] + 1;
		chan->key_on_delay_counter = 0;
		chan->pos += 2;
		chan->note = c & 0x7f;
		chan->pitch = ((5 + (chan->note << 6) + chan->detune) << 8);

		if(chan->staccato <= 8) {
			chan->staccato_counter = chan->staccato * chan->ticks_remaining / 8;
		} else {
			chan->staccato_counter = chan->staccato;
		}

		if(chan->key_on_delay) {
			chan->key_on_delay_counter = chan->key_on_delay;
		} else {
			mdx_driver_note_on(r, chan_num);
		}
		return chan->ticks_remaining;
	} else switch(c) {
		case 0xff: // Tempo
			if(r->set_tempo) {
				r->set_tempo(r, chan->data[chan->pos+1], r->data_ptr);
			}
			chan->pos += 2;
			break;
		case 0xfe: // OPM Register Write
			mdx_driver_write_opm(r, chan->data[chan->pos + 1], chan->data[chan->pos + 2]);
			chan->pos += 3;
			break;
		case 0xfd: // Set voice
			if(chan_num < 8) {
				chan->voice_num = chan->data[chan->pos + 1];
				uint8_t *v = r->f->voices[chan->voice_num];
				if(v) {
					for(int i = 0; i < 4; i++)
						mdx_driver_write_opm(r, 0x40 + i * 8 + chan_num, v[ 3 + i]); // DT1, MUL
					mdx_driver_set_tl(r, chan_num, v, chan->opm_volume);
					for(int i = 0; i < 4; i++)
						mdx_driver_write_opm(r, 0x80 + i * 8 + chan_num, v[11 + i]); // KS, AR
					for(int i = 0; i < 4; i++)
						mdx_driver_write_opm(r, 0xa0 + i * 8 + chan_num, v[15 + i]); // AME, D1R
					for(int i = 0; i < 4; i++)
						mdx_driver_write_opm(r, 0xc0 + i * 8 + chan_num, v[19 + i]); // DT2, D2R
					for(int i = 0; i < 4; i++)
						mdx_driver_write_opm(r, 0xe0 + i * 8 + chan_num, v[23 + i]); // D1L, RR
					mdx_driver_write_opm(r, 0x20 + chan_num, (chan->pan << 6) | v[1]); // PAN, FL, CON
				}
			}
			chan->pos += 2;
			break;
		case 0xfc: // Set Pan
			if(chan_num < 8) {
				chan->pan = chan->data[chan->pos + 1];
				//printf("%d: pan %d voice_num=%d voice=%p\n", chan_num, chan->pan, chan->voice_num, r->f->voices[chan->voice_num]);
				if(chan->voice_num >= 0) {
					uint8_t *v = r->f->voices[chan->voice_num];
					if(v) {
						mdx_driver_write_opm(r, 0x20 + chan_num, (chan->pan << 6) | v[1]); // PAN, FL, CON
					}
				}
			} else {
			}
			chan->pos += 2;
			break;
		case 0xfb: // Set volume
			chan->volume = chan->data[chan->pos+1];
			// printf("%d: volume %d\n", chan_num, chan->volume);
			chan->opm_volume = mdx_volume_to_opm(chan->volume);
			chan->pos += 2;
			break;
		case 0xfa: // Decrease volume
			mdx_driver_channel_dec_volume(chan);
			chan->opm_volume = mdx_volume_to_opm(chan->volume);
			chan->pos++;
			break;
		case 0xf9: // Increase volume
			mdx_driver_channel_inc_volume(chan);
			chan->opm_volume = mdx_volume_to_opm(chan->volume);
			chan->pos++;
			break;
		case 0xf8: // Staccato (q)
			chan->staccato = chan->data[chan->pos + 1];
			chan->pos += 2;
			break;
		case 0xf7: // Legato (&)
			if(chan_num < 8) {
				chan->skipNoteOff = 1;
			}
			chan->pos++;
			break;
		case 0xf6: // Repeat start ([)
			//printf("%d [ %d pos=%d\n",
			//	chan_num, chan->data[chan->pos + 1], chan->pos);
			// This is how MXDRV does it as well. No repeat stack needed.
			chan->data[chan->pos + 2] = chan->data[chan->pos + 1];
			chan->pos += 3;
			break;
		case 0xf5: // Repeat end (])
			{
				int16_t ofs = (chan->data[chan->pos + 1] << 8) | chan->data[chan->pos + 2];
				// printf("%d: ] pos=%d ofs=%d 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
				// 	chan_num, chan->pos, ofs,
				// 	chan->data[chan->pos + ofs], chan->data[chan->pos + ofs + 1],
				// 	chan->data[chan->pos + ofs + 2], chan->data[chan->pos + ofs + 3],
				// 	chan->data[chan->pos + ofs + 4], chan->data[chan->pos + ofs + 5]);
				chan->pos += 3;
				if(chan->data[chan->pos + ofs - 1] > 1) {
					chan->data[chan->pos + ofs - 1]--;
					chan->pos += ofs;
				}
			}
			break;
		case 0xf4: // Repeat escape (skip part of last repeat)
			{
				int16_t ofs = (chan->data[chan->pos + 1] << 8) | chan->data[chan->pos + 2];
				chan->pos += 3;
				int16_t start_ofs = (chan->data[chan->pos + ofs] << 8) | chan->data[chan->pos + ofs + 1];
				// printf("%d: / pos=%d ofs=%d start_ofs=%d 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
				// 	chan_num, chan->pos, ofs, start_ofs,
				// 	chan->data[chan->pos + ofs], chan->data[chan->pos + ofs + 1],
				// 	chan->data[chan->pos + ofs + 2], chan->data[chan->pos + ofs + 3],
				// 	chan->data[chan->pos + ofs + 4], chan->data[chan->pos + ofs + 5]);
				// printf("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
				// 	chan->data[chan->pos + ofs + start_ofs],
				// 	chan->data[chan->pos + ofs + start_ofs + 1],
				// 	chan->data[chan->pos + ofs + start_ofs + 2],
				// 	chan->data[chan->pos + ofs + start_ofs + 3],
				// 	chan->data[chan->pos + ofs + start_ofs + 4],
				// 	chan->data[chan->pos + ofs + start_ofs + 5]);
				if(chan->data[chan->pos + ofs + start_ofs + 1] <= 1) {
					chan->pos += ofs + 2;
				}
			}
			break;
		case 0xf3: // Detune (MML D)
			if(chan_num < 8) {
				chan->detune = (chan->data[chan->pos + 1] << 8) | chan->data[chan->pos + 2];
			}
			chan->pos += 3;
			break;
		case 0xf2: // Portamento (MML underscore)
			if(chan_num < 8) {
				chan->portamento = (chan->data[chan->pos + 1] << 8) | chan->data[chan->pos + 2];
			}
			chan->pos += 3;
			break;
		case 0xf1: // Data end or loop (MML ! or L)
			if(chan->data[chan->pos+1] == 0) {
				mdx_driver_end_channel(r, chan_num);
				chan->pos += 2;
			} else {
				int16_t ofs = (chan->data[chan->pos + 1] << 8) | chan->data[chan->pos + 2];
				if(r->loop_chan < 0)
					r->loop_chan = chan_num;
				if(chan_num == r->loop_chan) {
					r->cur_loop++;
					if(r->cur_loop >= r->max_loops && r->fade_rate == 0)
						mdx_driver_start_fadeout(r, 26);
				}
				chan->pos += ofs + 3;
			}
			break;
		case 0xf0: // Key on delay (in ticks)
			chan->key_on_delay = chan->data[chan->pos + 1];
			chan->pos += 2;
			break;
		case 0xef: // Sync send
			{
				int c = chan->data[chan->pos + 1];
				if(c < r->f->num_channels) {
					r->channels[c].waiting = 0;
				}
				chan->pos += 2;
			}
			break;
		case 0xee: // Sync wait
			chan->waiting = 1;
			chan->pos++;
			break;
		case 0xed:
			if(chan_num < 8) {
				//printf("%d: w %d\n", chan_num, chan->data[chan->pos + 1] & 0x1f);
				mdx_driver_write_opm(r, 0x0F, chan->data[chan->pos + 1]);
			} else {
				//printf("%d: F %d\n", chan_num, chan->data[chan->pos + 1]);
				chan->adpcm_freq_num = chan->data[chan->pos + 1];
			}
			chan->pos += 2;
			break;
		case 0xec: // Pitch LFO or vibrato
			if(chan->data[chan->pos + 1] == 0x80) {
				if(chan_num < 8) {
					chan->pitch_lfo.enable = 0;
				}
				chan->pos += 2;
			} else if(chan->data[chan->pos + 1] == 0x81) {
				if(chan_num < 8) {
					chan->pitch_lfo.enable = 1;
				}
				chan->pos += 2;
			} else {
				if(chan_num < 8) {
					mdx_lfo_init(
						&chan->pitch_lfo,
						chan->data[chan->pos + 1],
						(chan->data[chan->pos + 2] << 8) | chan->data[chan->pos + 3],
						(chan->data[chan->pos + 4] << 8) | chan->data[chan->pos + 5]
					);
				}
				chan->pos += 6;
			}
			break;
		case 0xeb: // Amplitude LFO or tremolo
			if(chan->data[chan->pos + 1] == 0x80) {
				if(chan_num < 8) {
					chan->amplitude_lfo.enable = 0;
				}
				chan->pos += 2;
			} else if(chan->data[chan->pos + 1] == 0x81) {
				if(chan_num < 8) {
					chan->amplitude_lfo.enable = 1;
				}
				chan->pos += 2;
			} else {
				if(chan_num < 8) {
					mdx_lfo_init(
						&chan->amplitude_lfo,
						chan->data[chan->pos + 1],
						(chan->data[chan->pos + 2] << 8) | chan->data[chan->pos + 3],
						(chan->data[chan->pos + 4] << 8) | chan->data[chan->pos + 5]
					);
				}
				chan->pos += 6;
			}
			break;
		case 0xea:
			if(chan->data[chan->pos + 1] == 0x80) {
				//printf("%d: MHOF\n", chan_num);
				chan->mhon = 0;
				chan->pos += 2;
			} else if(chan->data[chan->pos + 1] == 0x81) {
				//printf("%d: MHON\n", chan_num);
				chan->mhon = 1;
				chan->pos += 2;
			} else {
				//printf("%d: MH %02x %02x %02x %02x %02x\n",
				//	chan_num, chan->data[chan->pos + 1],
				//	chan->data[chan->pos + 2], chan->data[chan->pos + 3],
				//	chan->data[chan->pos + 4], chan->data[chan->pos + 5]);
				chan->keysync = chan->data[chan->pos + 1] & 0x40;
				chan->pms_ams = chan->data[chan->pos + 5];
				chan->mhon = 1;
				mdx_driver_write_opm(r, 0x19, 0x00); // YM2151: LFO Amplitude Modul. Depth
				mdx_driver_write_opm(r, 0x1B, chan->data[chan->pos + 1] & 0x03); // YM2151: LFO Wave Select
				mdx_driver_write_opm(r, 0x18, chan->data[chan->pos + 2]); // YM2151: LFO Frequency
				if(chan->data[chan->pos + 3] & 0x7f) mdx_driver_write_opm(r, 0x19, chan->data[chan->pos + 3]); // YM2151: LFO Phase Modul. Depth
				if(chan->data[chan->pos + 4]) mdx_driver_write_opm(r, 0x19, chan->data[chan->pos + 4]); // YM2151: LFO Amplitude Modul. Depth
				chan->pos += 6;
			}
			break;
		case 0xe9:
			chan->lfo_delay = chan->data[chan->pos + 1];
			chan->pos += 2;
			break;
		default:
			printf("%d: 0x%02x\n", chan_num, chan->data[chan->pos]);
			int l = mdx_cmd_len(chan->data, chan->pos, r->f->data_len - chan->pos);
			if(l < 0) mdx_driver_end_channel(r, chan_num);
			else chan->pos += l;
			break;
	}

	return 0;
}

void mdx_driver_tick(struct mdx_driver *r) {
	if(r->fade_rate > 0) {
		r->fade_counter--;
		if(r->fade_counter == 0) {
			r->fade_counter = r->fade_rate;
			r->fade_value++;
			if(r->fade_value > 72) {
				r->ended = 1;
				return;
			}
		}
	}

	for(int i = 0; i < r->f->num_channels; i++) {
		struct mdx_driver_channel *chan = &r->channels[i];
		if(!chan->used) continue;

		if(chan->ended || chan->waiting) continue;

		if(chan->key_on_delay_counter > 0) {
			chan->key_on_delay_counter--;
			if(chan->key_on_delay_counter == 0 && chan->staccato_counter == 0)
				mdx_driver_note_on(r, i);
		}

		if(chan->staccato_counter > 0) {
			chan->staccato_counter--;
			if(chan->staccato_counter == 0 && chan->key_on_delay_counter == 0)
				mdx_driver_note_off(r, i);
		}

		chan->ticks_remaining--;
		if(chan->ticks_remaining == 0) {
			chan->portamento = 0;
		}

		if(i < 8) {
			if(chan->portamento) {
				// printf("portamento %d + %d = %d\n", chan->pitch, chan->portamento, chan->pitch + chan->portamento);
				chan->pitch += chan->portamento;
			}

			int pitch = chan->pitch;
			if(chan->lfo_delay_counter) {
				chan->lfo_delay_counter--;
				if(chan->lfo_delay_counter == 0) {
					if(chan->pitch_lfo.enable)
						mdx_lfo_start(&chan->pitch_lfo);
					if(chan->amplitude_lfo.enable)
						mdx_lfo_start(&chan->amplitude_lfo);
				}
			}

			if(chan->pitch_lfo.enable && chan->lfo_delay_counter == 0) {
				pitch += chan->pitch_lfo.pitch;
				// printf("pitch=%d (%d) LFO pitch %d\n", chan->pitch, (chan->pitch >> 8) & 0x3f, chan->pitch_lfo.pitch);
				mdx_lfo_tick(&chan->pitch_lfo);
			}

			mdx_driver_set_pitch(r, i, pitch);

			int opmvol = chan->opm_volume + r->fade_value;
			if(chan->amplitude_lfo.enable && chan->lfo_delay_counter == 0) {
				mdx_lfo_tick(&chan->amplitude_lfo);
				opmvol += chan->amplitude_lfo.pitch;
			}

			if(chan->voice_num >= 0) {
				uint8_t *v = r->f->voices[chan->voice_num];
				if(v) {
					mdx_driver_set_tl(r, i, v, opmvol);
				}
			}
		} else {
			if(r->fade_value)
				adpcm_mixer_set_volume(r->adpcm_mixer, i-8, mdx_adpcm_volume_from_opm(chan->opm_volume + r->fade_value));
		}

		while(chan->ticks_remaining <= 0 && !chan->ended) {
			mdx_driver_advance_channel(r, i);
		}
	}

	if(r->tick_cb)
		r->tick_cb(r, r->data_ptr);
}

void mdx_driver_init(struct mdx_driver *r, struct mdx_file *f) {
	r->f = f;
	r->channel_mask = 0xffff;
	r->loop_chan = -1;
	r->ended = 0;
	r->max_loops = 2;
	r->fade_rate = 0;

	for(int i = 0; i < 16; i++) {
		mdx_driver_channel_init(&r->channels[i], &r->f->channels[i]);
		if(i < f->num_channels)
			r->channels[i].used = 1;
	}

	// reset registers
	for(int i = 0; i < 0x60; i++)
		mdx_driver_write_opm(r, i, 0x00);
	for(int i = 0x60; i < 0x80; i++)
		mdx_driver_write_opm(r, i, 0x7f);
	for(int i = 0x80; i < 0xe0; i++)
		mdx_driver_write_opm(r, i, 0x00);
	for(int i = 0xe0; i <= 0xff; i++)
		mdx_driver_write_opm(r, i, 0x0f);

	// Key off
	for(int i = 0; i < 8; i++)
		mdx_driver_write_opm(r, 0x08, i);
}

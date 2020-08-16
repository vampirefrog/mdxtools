#include <stdio.h>
#include <string.h>

#include "mdx_driver.h"
#include "tools.h"

void mdx_interpreter_track_init(struct mdx_interpreter_track *track, struct mdx_channel *mchan) {
	memset(track, 0, sizeof(*track));
	track->data = mchan->data;
	track->len = mchan->data_len;
	track->pos = 0;

	track->used = 0;
	track->ended = 0;

	track->key_on_delay = 0;
	track->key_on_delay_counter = 0;
	track->staccato = 8;
	track->staccato_counter = 0;

	track->ticks_remaining = 0;
	track->volume = 24;
	track->voice_num = -1;
	track->skipNoteOff = 0;
	track->skipNoteOn = 0;
	track->pan = 3;
	track->detune = 0;
	track->note = -1;
	track->repeat_stack_depth = 0;
	track->portamento = 0;
	track->adpcm_freq_num = 4;
}

static void mdx_interpreter_write_opm(struct mdx_interpreter *interp, uint8_t reg, uint8_t val) {
	if(reg > 0x20 && interp->opm_cache[reg] == val)
		return;
	interp->opm_cache[reg] = val;
	interp->write_opm(interp, reg, val, interp->data_ptr);
}

static void mdx_interpreter_track_inc_volume(struct mdx_interpreter_track *track) {
	if(track->volume < 15) track->volume++;
	else if(track->volume > 128) track->volume--;
}

static void mdx_interpreter_track_dec_volume(struct mdx_interpreter_track *track) {
	if(track->volume < 16 && track->volume > 0) track->volume--;
	else if(track->volume >= 128 && track->volume < 255) track->volume++;
}

static void mdx_interpreter_set_tl(struct mdx_interpreter *interp, int track_num, uint8_t *v, int volume) {
	const uint8_t con_masks[8] = {
		0x08, 0x08, 0x08, 0x08, 0x0c, 0x0e, 0x0e, 0x0f,
	};
	int mask = 1;
	for(int i = 0; i < 4; i++, mask <<= 1) {
		if((con_masks[v[1] & 0x07] & mask) > 0) {
			int vol = volume + v[7 + i];
			if(vol > 0x7f) vol = 0x7f;
			mdx_interpreter_write_opm(interp, 0x60 + i * 8 + track_num, vol); // TL
		} else {
			mdx_interpreter_write_opm(interp, 0x60 + i * 8 + track_num, v[7+i]); // TL
		}
	}
}

static void mdx_interpreter_end_channel(struct mdx_interpreter *interp, int track_num) {
	struct mdx_interpreter_track *track = &interp->channels[track_num];
	track->ended = 1;
	interp->ended = 1;
	for(int i = 0; i < 16; i++) {
		if(interp->channels[i].used && !interp->channels[i].ended) {
			interp->ended = 0;
			break;
		}
	}
}

static void mdx_interpreter_note_on(struct mdx_interpreter *interp, int track_num) {
	if((interp->channel_mask & (1 << track_num)) == 0) return;

	struct mdx_interpreter_track *track = &interp->channels[track_num];
	// printf("%d: ON note=%d, ticks=%d pitch=%d detune=%d skipNoteOn=%d skipNoteOff=%d\n", track_num, track->note, track->ticks_remaining, track->pitch, (track->pitch >> 8) & 0x3f, track->skipNoteOn, track->skipNoteOff);
	if(track_num < 8) {
		if(track->voice_num >= 0) {
			uint8_t *v = interp->f->voices[track->voice_num];
			if(v) {
				mdx_interpreter_set_pitch(interp, track_num, track->pitch);
				mdx_interpreter_set_tl(interp, track_num, v, track->opm_volume + interp->fade_value);
				if(track->skipNoteOn) {
					track->skipNoteOn = 0;
				} else {
					if(track->mhon) {
						mdx_interpreter_write_opm(interp, 0x38, track->pms_ams);
						if(track->keysync) {
							mdx_interpreter_write_opm(interp, 0x01, 0x02);
							mdx_interpreter_write_opm(interp, 0x01, 0x00);
						}
					}

					if(track->pitch_lfo.enable) {
						if(track->lfo_delay) {
							track->lfo_delay_counter = track->lfo_delay;
						} else {
							mdx_lfo_start(&track->pitch_lfo);
						}
					}

					mdx_interpreter_write_opm(interp, 0x08, ((v[2] & 0x0f) << 3) | (track_num & 0x07)); // Key On
				}
			} else printf("Note on !v %d\n", track_num);
		}
	} else {
		if(track->note < 96) {
			struct pdx_sample *s = &interp->f->pdx.samples[track->note];
			if(s->data && s->len) {
				adpcm_mixer_play(interp->adpcm_mixer, track_num - 8, s->decoded_data, s->len, track->adpcm_freq_num, mdx_adpcm_volume_from_opm(track->opm_volume + interp->fade_value), track->pan);
			}
		}
	}
}

static void mdx_interpreter_note_off(struct mdx_interpreter *interp, int track_num) {
	if((interp->channel_mask & (1 << track_num)) == 0) return;

	struct mdx_interpreter_track *track = &interp->channels[track_num];
	if(track->note >= 0) { // key off
		//printf("%d: OFF %d\n", track_num, track->note);
		if(track_num < 8) {
			if(track->skipNoteOff) {
				track->skipNoteOn = 1;
				track->skipNoteOff = 0;
			} else {
				mdx_interpreter_write_opm(interp, 0x08, track_num & 0x07);
			}
			//printf("Note OFF skip On=%d Off=%d\n", track->skipNoteOn, track->skipNoteOff);
		} else {
			adpcm_mixer_stop(interp->adpcm_mixer, track_num - 8);
		}
		track->note = -1;
	}
}

static int mdx_interpreter_advance_track(struct mdx_interpreter *interp, int track_num) {
	struct mdx_interpreter_track *track = &interp->channels[track_num];
	uint8_t c = track->data[track->pos];
	if(track->pos >= track->len) {
		mdx_interpreter_end_channel(interp, track_num);
		return 0;
	}

	mdx_interpreter_note_off(interp, track_num);

	if(c <= 0x7f) {
		//printf("rest %d\n", c + 1);
		track->note = -1;
		track->ticks_remaining = c + 1;
		track->pos++;
		return track->ticks_remaining;
	} else if(c <= 0xdf) {
		track->ticks_remaining = track->data[track->pos + 1] + 1;
		track->key_on_delay_counter = 0;
		track->pos += 2;
		track->note = c & 0x7f;
		track->pitch = ((5 + (track->note << 6) + track->detune) << 8);

		if(track->staccato <= 8) {
			track->staccato_counter = track->staccato * track->ticks_remaining / 8;
		} else {
			track->staccato_counter = track->staccato;
		}

		if(track->key_on_delay) {
			track->key_on_delay_counter = track->key_on_delay;
		} else {
			mdx_interpreter_note_on(interp, track_num);
		}
		return track->ticks_remaining;
	} else switch(c) {
		case 0xff: // Tempo
			if(interp->set_tempo) {
				interp->set_tempo(interp, track->data[track->pos+1], interp->data_ptr);
			}
			track->pos += 2;
			break;
		case 0xfe: // OPM Register Write
			mdx_interpreter_write_opm(interp, track->data[track->pos + 1], track->data[track->pos + 2]);
			track->pos += 3;
			break;
		case 0xfd: // Set voice
			if(interp->set_voice)
				interp->set_voice(interp, track_num, track->data[track->pos + 1]);
			track->pos += 2;
			break;
		case 0xfc: // Set Pan
			if(interp->set_pan)
				interp->set_pan(interp, track_num, track->data[track->pos + 1]);
			track->pos += 2;
			break;
		case 0xfb: // Set volume
			track->volume = track->data[track->pos+1];
			if(interp->set_volume)
				interp->set_volume(interp, track_num, track->volume);
			track->pos += 2;
			break;
		case 0xfa: // Decrease volume
			mdx_interpreter_channel_dec_volume(track);
			if(interp->set_volume)
				interp->set_volume(interp, track_num, track->volume);
			track->pos++;
			break;
		case 0xf9: // Increase volume
			mdx_interpreter_channel_inc_volume(track);
			if(interp->set_volume)
				interp->set_volume(interp, track_num, track->volume);
			track->pos++;
			break;
		case 0xf8: // Staccato (q)
			track->staccato = track->data[track->pos + 1];
			track->pos += 2;
			break;
		case 0xf7: // Legato (&)
			if(track_num < 8) {
				track->skipNoteOff = 1;
			}
			track->pos++;
			break;
		case 0xf6: // Repeat start ([)
			track->data[track->pos + 2] = track->data[track->pos + 1];
			track->pos += 3;
			break;
		case 0xf5: // Repeat end (])
			{
				int16_t ofs = (track->data[track->pos + 1] << 8) | track->data[track->pos + 2];
				track->pos += 3;
				if(track->data[track->pos + ofs - 1] > 1) {
					track->data[track->pos + ofs - 1]--;
					track->pos += ofs;
				}
			}
			break;
		case 0xf4: // Repeat escape (skip part of last repeat)
			{
				int16_t ofs = (track->data[track->pos + 1] << 8) | track->data[track->pos + 2];
				track->pos += 3;
				int16_t start_ofs = (track->data[track->pos + ofs] << 8) | track->data[track->pos + ofs + 1];
				if(track->data[track->pos + ofs + start_ofs + 1] <= 1) {
					track->pos += ofs + 2;
				}
			}
			break;
		case 0xf3: // Detune (MML D)
			if(track_num < 8) {
				track->detune = (track->data[track->pos + 1] << 8) | track->data[track->pos + 2];
			}
			track->pos += 3;
			break;
		case 0xf2: // Portamento (MML underscore)
			if(track_num < 8) {
				track->portamento = (track->data[track->pos + 1] << 8) | track->data[track->pos + 2];
			}
			track->pos += 3;
			break;
		case 0xf1: // Data end or loop (MML ! or L)
			if(track->data[track->pos+1] == 0) {
				mdx_interpreter_end_channel(interp, track_num);
				track->pos += 2;
			} else {
				int16_t ofs = (track->data[track->pos + 1] << 8) | track->data[track->pos + 2];
				if(interp->loop_chan < 0)
					interp->loop_chan = track_num;
				if(track_num == interp->loop_chan) {
					interp->cur_loop++;
					if(interp->cur_loop >= interp->max_loops && interp->fade_rate == 0)
						mdx_interpreter_start_fadeout(interp, 26);
				}
				track->pos += ofs + 3;
			}
			break;
		case 0xf0: // Key on delay (in ticks)
			track->key_on_delay = track->data[track->pos + 1];
			track->pos += 2;
			break;
		case 0xef: // Sync send
			{
				int c = track->data[track->pos + 1];
				if(c < interp->f->num_tracks) {
					interp->channels[c].waiting = 0;
				}
				track->pos += 2;
			}
			break;
		case 0xee: // Sync wait
			track->waiting = 1;
			track->pos++;
			break;
		case 0xed: // ADPCM sampling frequency / Noise frequency
			if(track_num < 8) {
				//printf("%d: w %d\n", track_num, track->data[track->pos + 1] & 0x1f);
				mdx_interpreter_write_opm(interp, 0x0F, track->data[track->pos + 1]);
			} else {
				//printf("%d: F %d\n", track_num, track->data[track->pos + 1]);
				track->adpcm_freq_num = track->data[track->pos + 1];
			}
			track->pos += 2;
			break;
		case 0xec: // Pitch LFO or vibrato
			if(track->data[track->pos + 1] == 0x80) {
				if(track_num < 8) {
					track->pitch_lfo.enable = 0;
				}
				track->pos += 2;
			} else if(track->data[track->pos + 1] == 0x81) {
				if(track_num < 8) {
					track->pitch_lfo.enable = 1;
				}
				track->pos += 2;
			} else {
				if(track_num < 8) {
					mdx_lfo_init(
						&track->pitch_lfo,
						track->data[track->pos + 1],
						(track->data[track->pos + 2] << 8) | track->data[track->pos + 3],
						(track->data[track->pos + 4] << 8) | track->data[track->pos + 5]
					);
				}
				track->pos += 6;
			}
			break;
		case 0xeb: // Amplitude LFO or tremolo
			if(track->data[track->pos + 1] == 0x80) {
				if(track_num < 8) {
					track->amplitude_lfo.enable = 0;
				}
				track->pos += 2;
			} else if(track->data[track->pos + 1] == 0x81) {
				if(track_num < 8) {
					track->amplitude_lfo.enable = 1;
				}
				track->pos += 2;
			} else {
				if(track_num < 8) {
					mdx_lfo_init(
						&track->amplitude_lfo,
						track->data[track->pos + 1],
						(track->data[track->pos + 2] << 8) | track->data[track->pos + 3],
						(track->data[track->pos + 4] << 8) | track->data[track->pos + 5]
					);
				}
				track->pos += 6;
			}
			break;
		case 0xea: // MHON/MHOF/MH
			if(track->data[track->pos + 1] == 0x80) {
				//printf("%d: MHOF\n", track_num);
				track->mhon = 0;
				track->pos += 2;
			} else if(track->data[track->pos + 1] == 0x81) {
				//printf("%d: MHON\n", track_num);
				track->mhon = 1;
				track->pos += 2;
			} else {
				//printf("%d: MH %02x %02x %02x %02x %02x\n",
				//	track_num, track->data[track->pos + 1],
				//	track->data[track->pos + 2], track->data[track->pos + 3],
				//	track->data[track->pos + 4], track->data[track->pos + 5]);
				track->keysync = track->data[track->pos + 1] & 0x40;
				track->pms_ams = track->data[track->pos + 5];
				track->mhon = 1;
				mdx_interpreter_write_opm(interp, 0x19, 0x00); // YM2151: LFO Amplitude Modul. Depth
				mdx_interpreter_write_opm(interp, 0x1B, track->data[track->pos + 1] & 0x03); // YM2151: LFO Wave Select
				mdx_interpreter_write_opm(interp, 0x18, track->data[track->pos + 2]); // YM2151: LFO Frequency
				if(track->data[track->pos + 3] & 0x7f) mdx_interpreter_write_opm(interp, 0x19, track->data[track->pos + 3]); // YM2151: LFO Phase Modul. Depth
				if(track->data[track->pos + 4]) mdx_interpreter_write_opm(interp, 0x19, track->data[track->pos + 4]); // YM2151: LFO Amplitude Modul. Depth
				track->pos += 6;
			}
			break;
		case 0xe9: // LFO Delay
			track->lfo_delay = track->data[track->pos + 1];
			track->pos += 2;
			break;
		default:
			printf("%d: 0x%02x\n", track_num, track->data[track->pos]);
			int l = mdx_cmd_len(track->data, track->pos, interp->f->data_len - track->pos);
			if(l < 0) mdx_interpreter_end_channel(interp, track_num);
			else track->pos += l;
			break;
	}

	return 0;
}

void mdx_interpreter_tick(struct mdx_interpreter *interp) {
	for(int i = 0; i < interp->f->num_tracks; i++) {
		struct mdx_interpreter_track *track = &interp->channels[i];
		if(!track->used) continue;

		if(track->ended || track->waiting) continue;

		if(track->key_on_delay_counter > 0) {
			track->key_on_delay_counter--;
			if(track->key_on_delay_counter == 0 && track->staccato_counter == 0)
				mdx_interpreter_note_on(interp, i);
		}

		if(track->staccato_counter > 0) {
			track->staccato_counter--;
			if(track->staccato_counter == 0 && track->key_on_delay_counter == 0)
				mdx_interpreter_note_off(interp, i);
		}

		track->ticks_remaining--;
		if(track->ticks_remaining == 0) {
			track->portamento = 0;
		}

		if(i < 8) {
			if(track->portamento) {
				// printf("portamento %d + %d = %d\n", track->pitch, track->portamento, track->pitch + track->portamento);
				track->pitch += track->portamento;
			}

			int pitch = track->pitch;
			if(track->lfo_delay_counter) {
				track->lfo_delay_counter--;
				if(track->lfo_delay_counter == 0) {
					if(track->pitch_lfo.enable)
						mdx_lfo_start(&track->pitch_lfo);
					if(track->amplitude_lfo.enable)
						mdx_lfo_start(&track->amplitude_lfo);
				}
			}

			if(track->pitch_lfo.enable && track->lfo_delay_counter == 0) {
				pitch += track->pitch_lfo.pitch;
				// printf("pitch=%d (%d) LFO pitch %d\n", track->pitch, (track->pitch >> 8) & 0x3f, track->pitch_lfo.pitch);
				mdx_lfo_tick(&track->pitch_lfo);
			}

			mdx_interpreter_set_pitch(interp, i, pitch);

			int opmvol = track->opm_volume + interp->fade_value;
			if(track->amplitude_lfo.enable && track->lfo_delay_counter == 0) {
				mdx_lfo_tick(&track->amplitude_lfo);
				opmvol += track->amplitude_lfo.pitch;
			}

			if(track->voice_num >= 0) {
				uint8_t *v = interp->f->voices[track->voice_num];
				if(v) {
					mdx_interpreter_set_tl(interp, i, v, opmvol);
				}
			}
		} else {
			if(interp->fade_value)
				adpcm_mixer_set_volume(interp->adpcm_mixer, i-8, mdx_adpcm_volume_from_opm(track->opm_volume + interp->fade_value));
		}

		while(track->ticks_remaining <= 0 && !track->ended) {
			mdx_interpreter_advance_track(interp, i);
		}
	}

	if(interp->tick_cb)
		interp->tick_cb(interp, interp->data_ptr);
}

void mdx_interpreter_init(struct mdx_interpreter *interpreter, struct mdx_file *mdx_file) {
	interpreter->mdx_file = mdx_file;
	interpreter->ended = 0;

	for(int i = 0; i < 16; i++) {
		mdx_interpreter_track_init(&interpreter->tracks[i], &interpreter->mdx_file->channels[i]);
		if(i < f->num_tracks)
			interpreter->tracks[i].used = 1;
	}
}

#include <stdio.h>
#include <string.h>

#include "mdx_driver.h"
#include "tools.h"

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

static void mdx_driver_track_init(struct mdx_driver_track *track) {
	memset(track, 0, sizeof(*track));
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
	track->portamento = 0;
	track->adpcm_freq_num = 4;
}

static void mdx_driver_track_load(struct mdx_driver_track *track, struct mdx_track *mtrack) {
	track->data = mtrack->data;
	track->len = mtrack->data_len;
}

static void mdx_driver_track_inc_volume(struct mdx_driver_track *track) {
	if(track->volume < 15) track->volume++;
	else if(track->volume > 128) track->volume--;
}

static void mdx_driver_track_dec_volume(struct mdx_driver_track *track) {
	if(track->volume < 16 && track->volume > 0) track->volume--;
	else if(track->volume >= 128 && track->volume < 255) track->volume++;
}

static void mdx_driver_end_track(struct mdx_driver *r, int track_num) {
	struct mdx_driver_track *track = &r->tracks[track_num];
	track->ended = 1;
	r->ended = 1;
	for(int i = 0; i < 16; i++) {
		if(r->tracks[i].used && !r->tracks[i].ended) {
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

static void mdx_driver_note_on(struct mdx_driver *r, int track_num) {
	if((r->track_mask & (1 << track_num)) == 0) return;

	struct mdx_driver_track *track = &r->tracks[track_num];
	if(track_num < 8) {
		if(track->voice_num >= 0) {
			uint8_t *v = r->mdx_file->voices[track->voice_num];
			if(v) {
				fm_driver_set_pitch(r->fm_driver, track_num, track->pitch);
				fm_driver_set_tl(r->fm_driver, track_num, track->opm_volume + r->fade_value, v);
				if(track->skipNoteOn) {
					track->skipNoteOn = 0;
				} else {
					if(track->mhon) {
						fm_driver_set_pms_ams(r->fm_driver, track_num, track->pms_ams);
						if(track->keysync) {
							fm_driver_reset_key_sync(r->fm_driver, track_num);
						}
					}

					if(track->pitch_lfo.enable) {
						if(track->lfo_delay) {
							track->lfo_delay_counter = track->lfo_delay;
						} else {
							mdx_lfo_start(&track->pitch_lfo);
						}
					}

					fm_driver_note_on(r->fm_driver, track_num, v[2], v);
				}
			} else printf("Note on !v %d\n", track_num);
		}
	} else {
		if(track->note < 96 && r->pdx_file) {
			adpcm_driver_play(r->adpcm_driver, track_num - 8, r->pdx_file->samples[track->note].data, r->pdx_file->samples[track->note].len, track->adpcm_freq_num, mdx_adpcm_volume_from_opm(track->opm_volume + r->fade_value));
		}
	}
}

static void mdx_driver_note_off(struct mdx_driver *driver, int track_num) {
	if((driver->track_mask & (1 << track_num)) == 0) return;

	struct mdx_driver_track *track = &driver->tracks[track_num];
	if(track->note >= 0) { // key off
		if(track_num < 8) {
			if(track->skipNoteOff) {
				track->skipNoteOn = 1;
				track->skipNoteOff = 0;
			} else {
				fm_driver_note_off(driver->fm_driver, track_num);
			}
		} else {
			adpcm_driver_stop(driver->adpcm_driver, track_num - 8);
		}
		track->note = -1;
	}
}

static int mdx_driver_track_advance(struct mdx_driver *driver, int track_num) {
	struct mdx_driver_track *track = &driver->tracks[track_num];

	uint8_t c = track->data[track->pos];
	if(track->pos >= track->len) {
		mdx_driver_end_track(driver, track_num);
		return 0;
	}

	mdx_driver_note_off(driver, track_num);

	if(c <= 0x7f) {
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
			mdx_driver_note_on(driver, track_num);
		}
		return track->ticks_remaining;
	} else switch(c) {
		case 0xff: // Tempo
			if(driver->set_tempo)
				driver->set_tempo(driver, track->data[track->pos+1], driver->data_ptr);
			if(driver->timer_driver)
				timer_driver_set_opm_tempo(driver->timer_driver, track->data[track->pos+1]);
			track->pos += 2;
			break;
		case 0xfe: // OPM Register Write
			fm_driver_write_opm_reg(driver->fm_driver, track->data[track->pos + 1], track->data[track->pos + 2]);
			track->pos += 3;
			break;
		case 0xfd: // Set voice
			if(track_num < 8) {
				track->voice_num = track->data[track->pos + 1];
				fm_driver_load_voice(driver->fm_driver, track_num, driver->mdx_file->voices[track->voice_num], track->opm_volume, track->pan);
			}
			track->pos += 2;
			break;
		case 0xfc: // Set Pan
			if(track_num < 8) {
				if(track->voice_num >= 0 && driver->mdx_file->voices[track->voice_num]) {
					track->pan = track->data[track->pos + 1];
					fm_driver_set_pan(driver->fm_driver, track_num, track->pan, driver->mdx_file->voices[track->voice_num]);
				}
			} else {
			}
			track->pos += 2;
			break;
		case 0xfb: // Set volume
			track->volume = track->data[track->pos+1];
			track->opm_volume = mdx_volume_to_opm(track->volume);
			if(driver->mdx_file->voices[track->voice_num] && track->voice_num >= 0) {
				fm_driver_set_tl(driver->fm_driver, track_num, track->opm_volume, driver->mdx_file->voices[track->voice_num]);
			}
			track->pos += 2;
			break;
		case 0xfa: // Decrease volume
			mdx_driver_track_dec_volume(track);
			track->opm_volume = mdx_volume_to_opm(track->volume);
			fm_driver_set_tl(driver->fm_driver, track_num, track->opm_volume, driver->mdx_file->voices[track->voice_num]);
			track->pos++;
			break;
		case 0xf9: // Increase volume
			mdx_driver_track_inc_volume(track);
			track->opm_volume = mdx_volume_to_opm(track->volume);
			fm_driver_set_tl(driver->fm_driver, track_num, track->opm_volume, driver->mdx_file->voices[track->voice_num]);
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
				mdx_driver_end_track(driver, track_num);
				track->pos += 2;
			} else {
				int16_t ofs = (track->data[track->pos + 1] << 8) | track->data[track->pos + 2];
				if(driver->loop_track < 0)
					driver->loop_track = track_num;
				if(track_num == driver->loop_track) {
					driver->cur_loop++;
					if(driver->cur_loop >= driver->max_loops && driver->fade_rate == 0)
						mdx_driver_start_fadeout(driver, 26);
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
				if(c < driver->mdx_file->num_tracks) {
					driver->tracks[c].waiting = 0;
				}
				track->pos += 2;
			}
			break;
		case 0xee: // Sync wait
			track->waiting = 1;
			track->pos++;
			break;
		case 0xed: // noise frequency / ADPCM freq num
			if(track_num < 8) {
				fm_driver_set_noise_freq(driver->fm_driver, track_num, track->data[track->pos + 1]);
			} else {
				adpcm_driver_set_freq(driver->adpcm_driver, track_num, track->data[track->pos + 1]);
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
				track->mhon = 0;
				track->pos += 2;
			} else if(track->data[track->pos + 1] == 0x81) {
				track->mhon = 1;
				track->pos += 2;
			} else {
				track->keysync = track->data[track->pos + 1] & 0x40;
				track->pms_ams = track->data[track->pos + 5];
				track->mhon = 1;
				fm_driver_load_lfo(driver->fm_driver, track_num, track->data[track->pos + 1] & 0x03, track->data[track->pos + 2], track->data[track->pos + 3], track->data[track->pos + 4]);
				track->pos += 6;
			}
			break;
		case 0xe9: // LFO Delay
			track->lfo_delay = track->data[track->pos + 1];
			track->pos += 2;
			break;
		default:
			if(driver->unknown_command_cb)
				driver->unknown_command_cb(driver, track_num, track->data[track->pos], driver->data_ptr);
			int l = mdx_cmd_len(track->data, track->pos, driver->mdx_file->data_len - track->pos);
			if(l < 0) mdx_driver_end_track(driver, track_num);
			else track->pos += l;
			break;
	}

	return 0;
}

void mdx_driver_track_tick(struct mdx_driver *driver, int track_num) {
	struct mdx_driver_track *track = &driver->tracks[track_num];

	if(!track->used) return;

	if(track->ended || track->waiting) return;

	if(track->key_on_delay_counter > 0) {
		track->key_on_delay_counter--;
		if(track->key_on_delay_counter == 0 && track->staccato_counter == 0)
			mdx_driver_note_on(driver, track_num);
	}

	if(track->staccato_counter > 0) {
		track->staccato_counter--;
		if(track->staccato_counter == 0 && track->key_on_delay_counter == 0)
			mdx_driver_note_off(driver, track_num);
	}

	track->ticks_remaining--;
	if(track->ticks_remaining == 0) {
		track->portamento = 0;
	}

	if(track_num < 8) {
		if(track->portamento) {
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
			mdx_lfo_tick(&track->pitch_lfo);
		}

		fm_driver_set_pitch(driver->fm_driver, track_num, pitch);

		int opmvol = track->opm_volume + driver->fade_value;
		if(track->amplitude_lfo.enable && track->lfo_delay_counter == 0) {
			mdx_lfo_tick(&track->amplitude_lfo);
			opmvol += track->amplitude_lfo.pitch;
		}

		if(track->voice_num >= 0) {
			uint8_t *v = driver->mdx_file->voices[track->voice_num];
			if(v) {
				fm_driver_set_tl(driver->fm_driver, track_num, opmvol, v);
			}
		}
	} else {
		if(driver->fade_value)
			adpcm_driver_set_volume(driver->adpcm_driver, track_num-8, mdx_adpcm_volume_from_opm(track->opm_volume + driver->fade_value));
	}

	while(track->ticks_remaining <= 0 && !track->ended) {
		mdx_driver_track_advance(driver, track_num);
	}
}

void mdx_driver_tick(struct mdx_driver *driver) {
	if(driver->fade_rate > 0) {
		driver->fade_counter--;
		if(driver->fade_counter == 0) {
			driver->fade_counter = driver->fade_rate;
			driver->fade_value++;
			if(driver->fade_value > 72) {
				driver->ended = 1;
				return;
			}
		}
	}

	for(int i = 0; i < driver->mdx_file->num_tracks; i++) {
		mdx_driver_track_tick(driver, i);
	}
}

static void mdx_timer_driver_tick_callback(struct timer_driver *timer_driver, void *data_ptr) {
	struct mdx_driver *driver = (struct mdx_driver *)data_ptr;

	mdx_driver_tick(driver);
}

void mdx_driver_init(struct mdx_driver *driver, struct timer_driver *timer_driver, struct fm_driver *fm_driver, struct adpcm_driver *adpcm_driver) {
	driver->timer_driver = timer_driver;
	timer_driver_set_tick_callback(timer_driver, mdx_timer_driver_tick_callback, driver);
	driver->fm_driver = fm_driver;
	driver->adpcm_driver = adpcm_driver;

	driver->mdx_file = 0;
	driver->pdx_file = 0;

	driver->track_mask = 0xffff;
	driver->loop_track = -1;
	driver->ended = 0;
	driver->max_loops = 2;
	driver->fade_rate = 0;

	driver->set_tempo = 0;

	driver->unknown_command_cb = 0;

	for(int i = 0; i < 16; i++) {
		mdx_driver_track_init(&driver->tracks[i]);
	}
}

int mdx_driver_load(struct mdx_driver *driver, struct mdx_file *mfile, struct pdx_file *pfile) {
	if(!mfile) return 1;

	driver->mdx_file = mfile;
	driver->pdx_file = pfile;
	for(int i = 0; i < 16; i++) {
		if(i < mfile->num_tracks) {
			driver->tracks[i].used = 1;
			mdx_driver_track_load(&driver->tracks[i], &mfile->tracks[i]);
		} else {
			driver->tracks[i].used = 0;
		}
	}

	return 0;
}

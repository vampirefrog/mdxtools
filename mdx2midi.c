#include <stdio.h>
#include <string.h>
#ifdef __linux__
#include <linux/limits.h>
#endif
#ifdef __APPLE__
#include <sys/syslimits.h>
#endif
#include <errno.h>
#include "cmdline.h"
#include "mdx.h"
#include "tools.h"
#include "midilib/midi.h"
#include "midilib/midi_file.h"
#include "midilib/midi_track.h"
#include "midilib/stream.h"

char *opt_output = 0;
int opt_utf8 = 0;

struct midi_file midi_file;
struct midi_track midi_track;

uint32_t deltaTime, restTime;
uint8_t lastCmd;
uint8_t volume, pan;
uint8_t firstTempo;
uint32_t totalTicks;
uint8_t kTicks, qTicks;
int nextKeyOff;
int nextPortamento;
int lastNote;
int detune;
int channel = 0;

int loopCount = 1;
int midiTracks = 1;
uint32_t totalTicks = 0;

void handleChannelStart(struct mdx_file *f, struct mdx_track *mdx_track, int i) {
	channel = i;
	volume = 127;
	pan = 3;
	firstTempo = 0;
	totalTicks = 0;
	kTicks = 0;
	qTicks = 8;
	nextKeyOff = 0;
	nextPortamento = 0;
	lastNote = -1;
	detune = 0;

	midi_track_init(&midi_track);
}

void handleChannelEnd(struct mdx_file *f, struct mdx_track *mdx_track, int channelNum) {
	if(midi_track.buffer.data_len == 0)
		return;

	struct midi_track *track = midi_file_append_empty_track(&midi_file);

	char buf[256];
	snprintf(buf, sizeof(buf), "Channel %c (%s)", mdx_track_name(channelNum), channelNum < 8 ? "FM" : "ADPCM");
	midi_track_write_track_name(track, 0, buf, -1);
	midi_track_write_bank_select(track, 0, channelNum, 0);

	midi_track_write_control_change(track, 0, channelNum, 126, 127); // Monophonic mode
	if(channelNum >= 8) { // OPM only
		midi_track_write_nrpn_msb(track, 0, channelNum, 0, 112); // OPM Clock = 4MHz
	}
	midi_track_write_control_change(track, 0, channelNum, 12, 0); // Amplitude LFO level
	midi_track_write_control_change(track, 0, channelNum, 13, 0); // Pitch LFO level

	midi_track_write_rpn_msb(track, 0, channelNum, 0, 32); // Pitch Bend Sensitivity
	midi_track_write_pitch_bend(track, 0, channelNum, 0x2000);

	midi_track_write_track(track, &midi_track);

	midi_track_write_track_end(track, 0);
}

void handleNote(uint8_t n, uint8_t duration) {
	int add = channel >= 8 ? 36 : 3;
	uint8_t d;
	if(qTicks <= 8) d = duration * qTicks / 8;
	else d = MIN(duration, 255 - qTicks);
	if(kTicks < d) {
		if(nextPortamento) {
			int nextNote = n + (nextPortamento * duration + (nextPortamento < 0 ? -16383 : 16383)) / 16384;
			if(lastNote >= 0) {
				midi_track_write_note_on(&midi_track, restTime, channel, lastNote + add, 0);
			}
			midi_track_write_control_change(&midi_track, 0, channel, 5, MIN(duration - kTicks, 127)); // Portamento time
			midi_track_write_note_on(&midi_track, kTicks, channel, n + add, volume);
			midi_track_write_note_on(&midi_track, 1, channel, nextNote + add, volume);
			midi_track_write_note_on(&midi_track, d - kTicks - 1, channel, n + add, 0);
			midi_track_write_note_on(&midi_track, 0, channel, nextNote + add, 0);
			nextPortamento = 0;
		} else if(nextKeyOff) {
			midi_track_write_control_change(&midi_track, 0, channel, 5, 0); // Portamento time
			if(lastNote >= 0) {
				if(lastNote != n) {
					midi_track_write_note_on(&midi_track, restTime + kTicks, channel, n + add, volume);
					midi_track_write_note_on(&midi_track, d - kTicks, channel, lastNote + add, 0);
					restTime = 0;
				} else restTime += duration;
			} else {
				midi_track_write_note_on(&midi_track, restTime + kTicks, channel, n + add, volume);
				restTime = duration;
			}
			lastNote = n;
		} else {
			if(lastNote >= 0) {
				if(lastNote != n) {
					midi_track_write_note_on(&midi_track, restTime + kTicks, channel, n + add, volume);
					midi_track_write_note_on(&midi_track, d - kTicks, channel, n + add, 0);
					midi_track_write_note_on(&midi_track, 0, channel, lastNote + add, 0);
				} else {
					midi_track_write_note_on(&midi_track, restTime + d, channel, n + add, 0);
				}
			} else {
				midi_track_write_note_on(&midi_track, restTime + kTicks, channel, n + add, volume);
				midi_track_write_note_on(&midi_track, d - kTicks, channel, n + add, 0);
			}
			restTime = duration - d;
			lastNote = -1;
		}
	} else {
		restTime += duration;
	}
	totalTicks += duration;
	nextPortamento = 0;
	nextKeyOff = 0;
}

void handleRest(uint8_t duration) {
	restTime += duration;
	totalTicks += duration;
}

void handlePortamento(int16_t p) {
	nextPortamento = p;
}

void handleDisableKeyOff() {
	nextKeyOff = 1;
}

void handleDataEnd() {
}

void handleSetVoiceNum(uint8_t voice) {
	midi_track_write_program_change(&midi_track, restTime, channel, voice);
	restTime = 0;
}

static uint32_t calcTempo(uint32_t tempo) {
	uint32_t mdxTempo = 1250000 / (256*(256-tempo));
	return 60000000 / mdxTempo;
}

void handleSetTempo(uint8_t tempo) {
	midi_track_write_tempo(&midi_track, restTime, calcTempo(tempo));
	restTime = 0;
	if(firstTempo == 0) firstTempo = tempo;
}

void handleDetune(int16_t det) {
	midi_track_write_rpn_msb(&midi_track, restTime, channel, 0, 32); // Pitch Bend Sensitivity
	midi_track_write_pitch_bend(&midi_track, 0, channel, 0x2000 + det * 4);
	detune = det;
	restTime = 0;
}

static int volumeVal(uint8_t v) {
	int vol_conv[] = {
		85,  87,  90,  93,  95,  98, 101, 103,
		106, 109, 111, 114, 117, 119, 122, 125
	};

	if(v < 16) return vol_conv[v];
	return 255 - v;
}

void handleSetVolume(uint8_t vol) {
	volume = volumeVal(vol) & 0x7f;
	midi_track_write_control_change(&midi_track, restTime, channel, 7, pan ? volumeVal(vol) & 0x7f : 0);
	restTime = 0;
}

void handlePan(uint8_t p) {
	pan = p;
	if(pan == 0) {
		midi_track_write_control_change(&midi_track, restTime, channel, 7, 0);
		restTime = 0;
	}
	uint8_t pans[] = { 64, 127, 0, 64 };
	midi_track_write_control_change(&midi_track, restTime, channel, 10, pans[pan & 0x03]);
	restTime = 0;
}

void handleSoundLength(uint8_t q) {
	qTicks = q;
}

void handleKeyOnDelay(uint8_t k) {
	kTicks = k;
}

void handleVolumeInc() {
	int v = (volume * 16 + 15) / 127;
	volume = (v + 1) * 127 / 15;
}

void handleVolumeDec() {
	int v = (volume * 16 + 15) / 127;
	volume = (v - 1) * 127 / 15;
}

void handlePerformanceEnd(uint16_t loopOffset) {}
void handleSyncSend(uint8_t channel) {}
void handleSyncWait() {}
void handleADPCMNoiseFreq(uint8_t freq) {}
void handleOPMLFO(uint8_t sync_wave, uint8_t lfrq, uint8_t pmd, uint8_t amd, uint8_t pms_ams) {}
void handleOPMLFOMHON() {}
void handleOPMLFOMHOF() {}
void handlePCM8Enable() {}
void handleFadeOut(uint8_t f) {}
void handleUndefinedCommand(uint8_t b) {}
void handleSetOPMRegister(uint8_t reg, uint8_t val) {}
void handleSetNoteLength(uint8_t ticks) {}
void handleLFOPitch(uint8_t b, uint16_t period, uint16_t change) {}
void handleLFOPitchMPON() {}
void handleLFOPitchMPOF() {}
void handleLFOVolume(uint8_t b, uint16_t period, uint16_t change) {}
void handleLFOVolumeMAON() {}
void handleLFOVolumeMAOF() {}
void handleLFODelaySetting(uint8_t d) {}

static void run_through_file(struct mdx_file *f, int *num_cmds_out, int *pcm8_out, int *pcm_notes_out) {
	*pcm8_out = 0;
	memset(num_cmds_out, 0, 16 * sizeof(int));
	if(pcm_notes_out) memset(pcm_notes_out, 0, 96 * sizeof(int));

	for(int i = 0; i < f->num_tracks; i++) {
		struct mdx_track *track = &f->tracks[i];

		handleChannelStart(f, track, i);

		for(int j = 0; j < track->data_len; /* nothing */) {
			int l = mdx_cmd_len(track->data, j, track->data_len - j);
			if(l < 0) break;

			uint8_t *p = &track->data[j];

			// stop on performance end command
			num_cmds_out[i]++;
			if(*p == 0xf1 && j < track->data_len - 1 && p[1] == 0) {
				break;
			}
			if(*p == 0xe8) *pcm8_out = 1;
			if(pcm_notes_out && i >= 8 && *p >= 0x80 && *p <= 0xdf) {
				pcm_notes_out[*p - 0x80]++;
			}

			// printf("channel=%d file_offset=0x%08x channel_offset=0x%08x length=%d ", i, j + (track->data - f->data), j, l);

			if(*p <= 0x7f) {
				handleRest(*p);
			} else if(*p <= 0xdf) {
				handleNote(*p - 0x80, p[1] + 1);
			} else switch(*p) {
				case 0xff: handleSetTempo(p[1]); break;
				case 0xfe: handleSetOPMRegister(p[1], p[2]); break;
				case 0xfd: handleSetVoiceNum(p[1]); break;
				case 0xfc: handlePan(p[1]); break;
				case 0xfb: handleSetVolume(p[1]); break;
				case 0xfa: handleVolumeDec(); break;
				case 0xf9: handleVolumeInc(); break;
				case 0xf8: handleSetNoteLength(p[1]); break;
				case 0xf7: handleDisableKeyOff(); break;
				case 0xf6:
					{
						p[2] = p[1];
						p += 3;
					}
					break;
				case 0xf5:
					{
						int16_t ofs = p[1] << 8 | p[2];
						p += 3;
						if(p[ofs - 1] > 1) {
							p[ofs - 1]--;
							p += ofs;
						}
					}
					break;
				case 0xf4:
					{
						int16_t ofs = (p[1] << 8) | p[2];
						p += 3;
						int16_t start_ofs = (p[ofs] << 8) | p[ofs + 1];
						if(p[ofs + start_ofs + 1] <= 1) {
							p += ofs + 2;
						}
					}
					break;
				case 0xf3: handleDetune(p[1] << 8 | p[2]); break;
				case 0xf2: handlePortamento(p[1] << 8 | p[2]); break;
				case 0xf1: handlePerformanceEnd(p[1] == 0 ? 0 : (p[1] << 8 | p[2])); break;
				case 0xf0: handleKeyOnDelay(p[1]); break;
				case 0xef: handleSyncSend(p[1]); break;
				case 0xee: handleSyncWait(); break;
				case 0xed: handleADPCMNoiseFreq(p[1]); break;
				case 0xec:
					{
						if(p[1] == 0x80) handleLFOPitchMPOF();
						else if(p[1] == 0x81) handleLFOPitchMPON();
						else handleLFOPitch(p[1], p[2] << 8 | p[3], p[4] << 8 | p[5]);
					}
					break;
				case 0xeb:
					{
						if(p[1] == 0x80) handleLFOVolumeMAOF();
						else if(p[1] == 0x81) handleLFOVolumeMAON();
						else handleLFOVolume(p[1], p[2] << 8 | p[3], p[4] << 8 | p[5]);
					}
					break;
				case 0xea:
					{
						if(p[1] == 0x80) handleOPMLFOMHOF();
						else if(p[1] == 0x81) handleOPMLFOMHON();
						else handleOPMLFO(p[1], p[2], p[3], p[4], p[5]);
					}
					break;
				case 0xe9: handleLFODelaySetting(p[1]); break;
				case 0xe8: handlePCM8Enable(); break;
				case 0xe7: handleFadeOut(p[2]); break;
				default:
					handleUndefinedCommand(*p);
					break;
			}
			j += l;
		}
		handleChannelEnd(f, track, i);
	}
}

static int write_cb(void *buf, size_t len, void *data_ptr) {
	return fwrite(buf, 1, len, (FILE *)data_ptr);
}

int main(int argc, char **argv) {
	int optind = cmdline_parse_args(argc, argv, (struct cmdline_option[]){
		{
			'u', "utf8",
			"Convert strings to UTF-8",
			0,
			TYPE_SWITCH,
			TYPE_INT, &opt_utf8
		},
		{
			'o', "output",
			"Output to this file. `-' means stdout (default).",
			"file",
			TYPE_OPTIONAL,
			TYPE_STRING, &opt_output
		},
		CMDLINE_ARG_TERMINATOR
	}, 1, 1, "<file.mdx>");

	if(optind < 0) exit(-optind);

	size_t data_len = 0;
	uint8_t *data = load_file(argv[optind], &data_len);
	if(!data) {
		fprintf(stderr, "Could not load %s\n", argv[optind]);
		return 1;
	}

	struct mdx_file f;
	mdx_file_load(&f, data, data_len);

	midi_file_init( &midi_file, MIDI_FORMAT_MULTI_TRACKS, 0, 48);

	int num_cmds_out[16], pcm8_out;
	run_through_file(&f, num_cmds_out, &pcm8_out, 0);

	free(data);

	struct midi_track *first_track = midi_file_prepend_empty_track(&midi_file);
	midi_track_write_tempo(first_track, 0, calcTempo(firstTempo ? firstTempo : 200));
	midi_track_write_time_signature(first_track, 0, 4, 2, 0, 0);
	char buf[PATH_MAX];
	snprintf(buf, sizeof(buf), "Converted from %s", argv[optind]);
	midi_track_write_text(first_track, 0, buf, -1);
	midi_track_write_track_end(first_track, totalTicks);
	if(!opt_output || !opt_output[0]) {
		char midbuf[PATH_MAX];
		replace_ext(midbuf, sizeof(midbuf), argv[optind], "mid");
		opt_output = midbuf;
	}
	FILE *o = fopen(opt_output, "wb");
	if(!o) {
		fprintf(stderr, "Could not open %s: %s (%d)\n", opt_output, strerror(errno), errno);
		return -1;
	}
	midi_file_write(&midi_file, write_cb, o);
	fclose(o);
	midi_file_clear(&midi_file);

	return 0;
}

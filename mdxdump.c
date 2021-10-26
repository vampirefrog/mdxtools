#include <stdio.h>
#include <string.h>
#include "mdx.h"
#include "tools.h"

void handleRest(uint8_t duration) { printf("Rest %d (192 / %d)\n", duration, duration == 0 ? 0 : 192 / duration); }
void handleNote(uint8_t note, uint8_t duration) { printf("Note %d (%s%d) duration %d (192 / %d)\n", note, mdx_note_name(note), mdx_note_octave(note), duration, duration == 0 ? 0 : 192 / duration); }
void handleCommand(uint8_t c, ...) { /* printf("Command 0x%02x: %s\n", c, commandName(c)); */ }
void handlePCM8ExpansionShift() { printf("PCM8ExpansionShift\n"); }
void handleUndefinedCommand(uint8_t b) { printf("UndefinedCommand %d\n", b); }
void handleChannelStart(int track) { printf("ChannelStart %c (%d)\n", mdx_track_name(track), track); }
void handleChannelEnd(int track) { printf("ChannelEnd %c (%d)\n", mdx_track_name(track), track); }

/* ff */ void handleSetTempo(uint8_t t) { printf("SetTempo %d BPM (%d)\n", 78125 / (16 * (256 - t)), t); }
/* fe */ void handleSetOPMRegister(uint8_t reg, uint8_t val) { printf("SetOPMRegister 0x%02x 0x%02x;\n", reg, val); }
/* fd */ void handleSetVoiceNum(uint8_t t) { printf("SetVoiceNum %d\n", t); }
/* fc */ void handlePan(uint8_t p) { printf("Pan %d\n", p); }
/* fb */ void handleSetVolume(uint8_t v) { printf("SetVolume %d\n", v); }
/* fa */ void handleVolumeDec() { printf("VolumeDec\n"); }
/* f9 */ void handleVolumeInc() { printf("VolumeInc\n"); }
/* f8 */ void handleSetNoteLength(uint8_t l) { printf("SetNoteLength %d\n", l); }
/* f7 */ void handleDisableKeyOff() { printf("DisableKeyOff\n"); }
/* f6 */ void handleRepeatStart(uint8_t r, uint8_t n) { printf("RepeatStart %d %d\n", r, n); }
/* f5 */ void handleRepeatEnd(int16_t r) { printf("RepeatEnd %d\n", r); }
/* f4 */ void handleRepeatEscape(int16_t r) { printf("RepeatEscape %d\n", r); }
/* f3 */ void handleDetune(int16_t d) { printf("Detune %d\n", d); }
/* f2 */ void handlePortamento(int16_t t) { printf("Portamento %d\n", t); }
/* f1 */ void handlePerformanceEnd(int16_t loop) { printf("PerformanceEnd %d\n", loop); }
/* f0 */ void handleKeyOnDelay(uint8_t d) { printf("KeyOnDelay %d\n", d); }
/* ef */ void handleSyncSend(uint8_t s) { printf("SyncSend %d\n", s); }
/* ee */ void handleSyncWait() { printf("SyncWait\n"); }
/* ed */ void handleADPCMNoiseFreq(uint8_t f) { printf("ADPCMNoiseFreq %d\n", f); }
/* ec */ void handleLFOPitchMPON() { printf("LFOPitchMPON\n"); }
/* ec */ void handleLFOPitchMPOF() { printf("LFOPitchMPOF\n"); }
/* ec */ void handleLFOPitch(uint8_t b, uint16_t period, uint16_t change) { printf("LFOPitch %d %d %d\n", b, period, change); }
/* eb */ void handleLFOVolume(uint8_t b, uint16_t period, uint16_t change) { printf("LFOVolume %d %d %d\n", b, period, change); }
/* eb */ void handleLFOVolumeMAON() { printf("LFOVolumeMAON\n"); }
/* eb */ void handleLFOVolumeMAOF() { printf("LFOVolumeMAOF\n"); }
/* ea */ void handleOPMLFO(uint8_t sync_wave, uint8_t lfrq, uint8_t pmd, uint8_t amd, uint8_t pms_ams) { printf("OPMLFO %d %d %d %d %d\n", sync_wave, lfrq, pmd, amd, pms_ams); }
/* ea */ void handleOPMLFOMHON() { printf("OPMLFOMHON\n"); }
/* ea */ void handleOPMLFOMHOF() { printf("OPMLFOMHOF\n"); }
/* e9 */ void handleLFODelaySetting(uint8_t d) { printf("LFODelaySetting %d\n", d); }
/* e8 */ void handlePCM8Enable() { printf("PCM8Enable\n"); }
/* e7 */ void handleFadeOut(uint8_t f) { printf("FadeOut %d\n", f); }

static void run_through_file(struct mdx_file *f, int *num_cmds_out, int *pcm8_out, int *pcm_notes_out) {
	*pcm8_out = 0;
	memset(num_cmds_out, 0, 16 * sizeof(int));
	if(pcm_notes_out) memset(pcm_notes_out, 0, 96 * sizeof(int));

	for(int i = 0; i < f->num_tracks; i++) {
		struct mdx_track *track = &f->tracks[i];

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
				handleNote(*p - 0x80, p[1]);
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
				case 0xf6: handleRepeatStart(p[1], p[2]); break;
				case 0xf5: handleRepeatEnd(p[1] << 8 | p[2]); break;
				case 0xf4: handleRepeatEscape(p[1] << 8 | p[2]); break;
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
	}
}

int main(int argc, char **argv) {
	if(argc < 2) {
		return 1;
	}

	size_t data_len = 0;
	uint8_t *data = load_file(argv[1], &data_len);
	if(!data) {
		fprintf(stderr, "Could not load %s\n", argv[1]);
		return 1;
	}

	struct mdx_file f;
	mdx_file_load(&f, data, data_len);

	printf("title %s\n", f.title);
	printf("pdxfile %s\n", f.pdx_filename);

	int num_cmds_out[16], pcm8_out;
	run_through_file(&f, num_cmds_out, &pcm8_out, 0);

	free(data);

	return 0;
}

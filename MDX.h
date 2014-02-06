#ifndef MDX_H_
#define MDX_H_

#include "exceptionf.h"
#include "FileStream.h"
#include <iconv.h>

struct MDXVoiceOsc {
	uint8_t dt1, dt2;
	uint8_t mul, tl;
	uint8_t ks, ar;
	uint8_t ame, rr;
	uint8_t d1r, d2r;
	uint8_t d1l;

	void dump() {
		printf("\tdt1=%d dt2=%d mul=%d \n", dt1, dt2, mul);
		printf("\ttl=%d ks=%d ar=%d\n", tl, ks, ar);
		printf("\tame=%d rr=%d\n", ame, rr);
		printf("\td1r=%d d2r=%d d1l=%d\n", d1r, d2r, d1l);
	}
};

struct MDXVoice {
	uint8_t number, fl, con, pan, slot_mask;
	MDXVoiceOsc osc[4];

	bool load(FileStream &s) {
		uint8_t buf[27];
		if(s.read(buf, sizeof(buf)) < sizeof(buf)) return false;
		uint8_t *b = buf;
		number = *b++;
		fl = (*b >> 3) & 0x07; con = (*b++ & 0x07);
		pan = 0xc0;
		slot_mask = *b++;
		for(int i = 0; i < 4; i++, b++) {
			osc[i].dt1 = (*b >> 3) & 0x07;
			osc[i].mul = *b & 0x07;
		}
		for(int i = 0; i < 4; i++, b++) {
			osc[i].tl = *b;
		}
		for(int i = 0; i < 4; i++, b++) {
			osc[i].ks = (*b >> 6) & 0x03;
			osc[i].ar = *b & 0x1f;
		}
		for(int i = 0; i < 4; i++, b++) {
			osc[i].ame = *b & 0x80;
			osc[i].d1r = *b & 0x1f;
		}
		for(int i = 0; i < 4; i++, b++) {
			osc[i].dt2 = (*b >> 6) & 0x03;
			osc[i].d2r = *b & 0x1f;
		}
		for(int i = 0; i < 4; i++, b++) {
			osc[i].d1l = (*b >> 4) & 0x0f;
			osc[i].rr = *b & 0x0f;
		}
		return true;
	}

	void dump() {
		printf("Voice %d\n", number);
		printf("fl=%d con=%d pan=%d slot_mask=0x%02x\n", fl, con, pan, slot_mask);
		for(int i = 0; i < 4; i++) {
			printf("Osc %d\n", i);
			osc[i].dump();
		}
	}
};

class MDX {
private:
	uint16_t Voice_offset, mml_offset[16];
	FileStream s;
public:
	const char *title, *pcm_file;
	MDX(const char *filename) {
		load(filename);
	}
	MDX() {}
	~MDX() {}

	void load(const char *filename) {
		s.open(filename);
		title = s.readLineIconv(0x1a, "Shift_JIS", "UTF-8");
		pcm_file = s.readLineIconv(0, "Shift_JIS", "UTF-8");
		int file_base = s.tell();
		Voice_offset = s.readUint16Big();
		mml_offset[0] = s.readUint16Big();
		for(int i = 1; i < mml_offset[0] / 2 - 1 &&  i < 16; i++) {
			mml_offset[i] = s.readUint16Big();
		}
		printf("file_base=%d Voice_offset=%d\n", file_base, Voice_offset);
		s.seek(file_base + Voice_offset);
		while(!s.eof()) {
			MDXVoice inst;
			if(!inst.load(s)) break;
			//handleVoice(inst);
		}
		s.seek(file_base + mml_offset[0]);
		enum {
			None = 0,
			NoteDuration,
			TempoVal,
			OPMRegisterNum,
			OPMRegisterVal,
			VoiceNum,
			OutputPhaseVal,
			VolumeVal,
			SoundLen,
			RepeatStartCount,
			RepeatStartZero,
			RepeatEndOffsetMSB,
			RepeatEndOffsetLSB,
			RepeatEscapeMSB,
			RepeatEscapeLSB,
			DetuneMSB,
			DetuneLSB,
			PortamentoMSB,
			PortamentoLSB,
			DataEndMSB,
			DataEndLSB,
			KeyOnDelayVal,
			SyncSendChannel,
			ADPCMNoiseFreqVal,
			LFODelayVal,
			LFOPitchB,
			LFOPitchPeriodMSB,
			LFOPitchPeriodLSB,
			LFOPitchChangeMSB,
			LFOPitchChangeLSB,
			LFOVolumeB,
			LFOVolumePeriodMSB,
			LFOVolumePeriodLSB,
			LFOVolumeChangeMSB,
			LFOVolumeChangeLSB,
			OPMLFOB,
			OPMLFOPeriodMSB,
			OPMLFOPeriodLSB,
			OPMLFOChangeMSB,
			OPMLFOChangeLSB,
			FadeOutB1,
			FadeOutValue,
		} state = None;
		uint8_t nn;
		int16_t w, v;
		while(!s.eof()) {
			uint8_t b = s.readUint8();
			if(s.eof()) break;
			switch(state) {
				case None:
					if(b >= 0x00 && b <= 0x7f) {
						handleRest(b + 1);
					} else if(b >= 0x80 && b < 0xdf) {
						nn = b;
						state = NoteDuration;
					} else {
						switch(b) {
							case 0xff: // Set tempo
								state = TempoVal;
								break;
							case 0xfe:
								state = OPMRegisterNum;
								break;
							case 0xfd:
								state = VoiceNum;
								break;
							case 0xfc:
								state = OutputPhaseVal;
								break;
							case 0xfb:
								state = VolumeVal;
								break;
							case 0xfa:
								handleVolumeDec();
								state = None;
								break;
							case 0xf9:
								handleVolumeInc();
								state = None;
								break;
							case 0xf8:
								state = SoundLen;
								break;
							case 0xf7:
								handleDisableKeyOff();
								state = None;
								break;
							case 0xf6:
								state = RepeatStartCount;
								break;
							case 0xf5:
								state = RepeatEndOffsetMSB;
								break;
							case 0xf4:
								state = RepeatEscapeMSB;
								break;
							case 0xf3:
								state = DetuneMSB;
								break;
							case 0xf2:
								state = PortamentoMSB;
								break;
							case 0xf1:
								state = DataEndMSB;
								break;
							case 0xf0:
								state = KeyOnDelayVal;
								break;
							case 0xef:
								state = SyncSendChannel;
								break;
							case 0xee:
								handleSyncWait();
								state = None;
								break;
							case 0xed:
								state = ADPCMNoiseFreqVal;
								break;
							case 0xec:
								state = LFOPitchB;
								break;
							case 0xeb:
								state = LFOVolumeB;
								break;
							case 0xea:
								state = OPMLFOB;
								break;
							case 0xe9:
								state = LFODelayVal;
								break;
							case 0xe8:
								handlePCM8ExpansionShift();
								state = None;
								break;
							case 0xe7:
								state = FadeOutB1;
								break;
							default:
								handleUndefinedCommand(b);
								break;
						}
					}
					break;
				case NoteDuration:
					handleNote(nn, b);
					state = None;
					break;
				case TempoVal:
					handleSetTempo(b);
					state = None;
					break;
				case OPMRegisterNum:
					nn = b;
					state = OPMRegisterVal;
					break;
				case OPMRegisterVal:
					handleSetOPMRegister(nn, b);
					state = None;
					break;
				case VoiceNum:
					handleSetVoiceNum(b);
					state = None;
					break;
				case OutputPhaseVal:
					handleOutputPhase(b);
					state = None;
					break;
				case VolumeVal:
					handleSetVolume(b);
					state = None;
					break;
				case SoundLen:
					handleSoundLength(b);
					state = None;
					break;
				case RepeatStartCount:
					handleRepeatStart(b);
					state = RepeatStartZero;
					break;
				case RepeatStartZero:
					state = None;
					break;
				case RepeatEndOffsetMSB:
					nn = b;
					state = RepeatEndOffsetLSB;
					break;
				case RepeatEndOffsetLSB:
					handleRepeatEnd((nn << 8) | b);
					state = None;
					break;
				case RepeatEscapeMSB:
					nn = b;
					state = RepeatEscapeLSB;
					break;
				case RepeatEscapeLSB:
					handleRepeatEscape((nn << 8) | b);
					state = None;
					break;
				case DetuneMSB:
					nn = b;
					state = DetuneLSB;
					break;
				case DetuneLSB:
					handleDetune((nn << 8) | b);
					state = None;
					break;
				case PortamentoMSB:
					nn = b;
					state = PortamentoLSB;
					break;
				case PortamentoLSB:
					handlePortamento((nn << 8) | b);
					state = None;
					break;
				case DataEndMSB:
					if(b == 0) {
						handleDataEnd();
						state = None;
					} else {
						nn = b;
						state = DataEndLSB;
					}
					break;
				case DataEndLSB:
					handleDataEnd((nn << 8) & b);
					state = None;
					break;
				case KeyOnDelayVal:
					handleKeyOnDelay(b);
					break;
				case SyncSendChannel:
					handleSyncSend(b);
					state = None;
					break;
				case ADPCMNoiseFreqVal:
					handleADPCMNoiseFreq(b);
					state = None;
					break;
				case LFODelayVal:
					handleLFODelaySetting(b);
					state = None;
					break;
				case LFOPitchB:
					if(b == 0x80) {
						handleLFOPitchMPOF();
						state = None;
					} else if(b == 0x81) {
						handleLFOPitchMPON();
						state = None;
					} else {
						nn = b;
						state = LFOPitchPeriodMSB;
					}
					break;
				case LFOPitchPeriodMSB:
					w = b << 8;
					state = LFOPitchPeriodLSB;
					break;
				case LFOPitchPeriodLSB:
					w |= b;
					state = LFOPitchChangeMSB;
					break;
				case LFOPitchChangeMSB:
					v = b << 8;
					state = LFOPitchChangeLSB;
					break;
				case LFOPitchChangeLSB:
					v |= b;
					handleLFOPitch(nn, w, v);
					state = None;
					break;
				case LFOVolumeB:
					if(b == 0x80) {
						handleLFOVolumeMAOF();
						state = None;
					} else if(b == 0x81) {
						handleLFOVolumeMAON();
						state = None;
					} else {
						nn = b;
						state = LFOVolumePeriodMSB;
					}
					break;
				case LFOVolumePeriodMSB:
					w = b << 8;
					state = LFOVolumePeriodLSB;
					break;
				case LFOVolumePeriodLSB:
					w |= b;
					state = LFOVolumeChangeMSB;
					break;
				case LFOVolumeChangeMSB:
					v = b << 8;
					state = LFOVolumeChangeLSB;
					break;
				case LFOVolumeChangeLSB:
					v |= b;
					handleLFOVolume(nn, w, v);
					state = None;
					break;
				case OPMLFOB:
					if(b == 0x80) {
						handleOPMLFOMHOF();
						state = None;
					} else if(b == 0x81) {
						handleOPMLFOMHON();
						state = None;
					} else {
						nn = b;
						state = OPMLFOPeriodMSB;
					}
					break;
				case OPMLFOPeriodMSB:
					w = b << 8;
					state = OPMLFOPeriodLSB;
					break;
				case OPMLFOPeriodLSB:
					w |= b;
					state = OPMLFOChangeMSB;
					break;
				case OPMLFOChangeMSB:
					v = b << 8;
					state = OPMLFOChangeLSB;
					break;
				case OPMLFOChangeLSB:
					v |= b;
					handleOPMLFO(nn, w, v);
					state = None;
					break;
				case FadeOutB1:
					state = FadeOutValue;
					break;
				case FadeOutValue:
					handleFadeOut(b);
					state = None;
					break;
				default:
					printf("Unknown state %d\n", state);
					break;
			}
		}
	}
	const char *commandName(uint8_t c) {
		const char *cmdNames[] = {
			"Informal command", // 0xe6
			"Extended MML", // 0xe7
			"PCM4/8 enable", // 0xe8
			"LFO delay setting", // 0xe9
			"OPM LFO control", // 0xea
			"LFO volume control", // 0xeb
			"LFO pitch control", // 0xec
			"ADPCM/noise freq", // 0xed
			"Sync signal wait", // 0xee
			"Sync signal send", // 0xef
			"Key on delay", // 0xf0
			"Data end", // 0xf1
			"Portamento time", // 0xf2
			"Detune", // 0xf3
			"Repeat escape", // 0xf4
			"Repeat end", // 0xf5
			"Repeat start", // 0xf6
			"Disable key-off", // 0xf7
			"Sound length", // 0xf8
			"Volume decrement", // 0xf9
			"Volume increment", // 0xfa
			"Set volume", // 0xfb
			"Output phase", // 0xfc
			"Set voice #", // 0xfd
			"Set OPM register", // 0xfe
			"Set tempo", // 0xff
		};
		if(c >= 0xe6 && c <= 0xff) return cmdNames[c - 0xe6];
		return "Unknown";
	}
	const char *voiceName(uint8_t n) {
		const char *voiceNames[] = { "M1", "M2", "C1", "C2" };
		return voiceNames[n & 0x03];
	}
	virtual void handleVoice(MDXVoice &v) { v.dump(); }
	virtual void handleRest(uint8_t duration) { printf("Rest %d\n", duration); }
	virtual void handleNote(uint8_t note, uint8_t duration) { printf("Note %d duration %d\n", note, duration); }
	virtual void handleCommand(uint8_t c, ...) { printf("Command 0x%02x: %s\n", c, commandName(c)); }
	virtual void handleVolumeInc() { printf("VolumeInc\n"); }
	virtual void handleVolumeDec() { printf("VolumeDec\n"); }
	virtual void handleDisableKeyOff() { printf("DisableKeyOff\n"); }
	virtual void handleSyncWait() { printf("SyncWait\n"); }
	virtual void handlePCM8Enable() { printf("PCM8Enable\n"); }
	virtual void handleSetTempo(uint8_t t) { printf("SetTempo %d\n", t); }
	virtual void handleSetVoiceNum(uint8_t t) { printf("SetVoiceNum %d\n", t); }
	virtual void handleOutputPhase(uint8_t p) { printf("OutputPhase %d\n", p); }
	virtual void handleSetVolume(uint8_t v) { printf("SetVolume %d\n", v); }
	virtual void handleSoundLength(uint8_t l) { printf("SoundLength %d\n", l); }
	virtual void handleKeyOnDelay(uint8_t d) { printf("KeyOnDelay %d\n", d); }
	virtual void handleSyncSend(uint8_t s) { printf("SyncSend %d\n", s); }
	virtual void handleADPCMNoiseFreq(uint8_t f) { printf("ADPCMNoiseFreq %d\n", f); }
	virtual void handleLFODelaySetting(uint8_t d) { printf("LFODelaySetting %d\n", d); }
	virtual void handleRepeatStart(uint8_t r) { printf("RepeatStart %d\n", r); }
	virtual void handleRepeatEnd(int16_t r) { printf("RepeatEnd %d\n", r); }
	virtual void handleRepeatEscape(int16_t r) { printf("RepeatEscape %d\n", r); }
	virtual void handleDetune(int16_t d) { printf("Detune %d\n", d); }
	virtual void handlePortamento(int16_t t) { printf("Portamento %d\n", t); }
	virtual void handleSetOPMRegister(uint8_t reg, uint8_t val) { printf("SetOPMRegister %d %d;\n", reg, val); }
	virtual void handleDataEnd() { printf("DataEnd\n"); }
	virtual void handleDataEnd(int16_t end) { printf("DataEnd %d\n", end); }
	virtual void handleLFOPitch(uint8_t b, uint16_t period, uint16_t change) { printf("LFOPitch %d %d %d\n", b, period, change); }
	virtual void handleLFOPitchMPON() { printf("LFOPitchMPON\n"); }
	virtual void handleLFOPitchMPOF() { printf("LFOPitchMPOF\n"); }
	virtual void handleLFOVolume(uint8_t b, uint16_t period, uint16_t change) { printf("LFOVolume %d %d %d\n", b, period, change); }
	virtual void handleLFOVolumeMAON() { printf("LFOVolumeMAON\n"); }
	virtual void handleLFOVolumeMAOF() { printf("LFOVolumeMAOF\n"); }
	virtual void handleOPMLFO(uint8_t b, uint16_t period, uint16_t change) { printf("OPMLFO %d %d %d\n", b, period, change); }
	virtual void handleOPMLFOMHON() { printf("OPMLFOMHON\n"); }
	virtual void handleOPMLFOMHOF() { printf("OPMLFOMHOF\n"); }
	virtual void handleFadeOut(uint8_t f) { printf("FadeOut %d\n", f); }
	virtual void handlePCM8ExpansionShift() { printf("PCM8ExpansionShift\n"); }
	virtual void handleUndefinedCommand(uint8_t b) { printf("UndefinedCommand %d\n", b); }
};

#endif /* MDX_H_ */

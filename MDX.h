#ifndef MDX_H_
#define MDX_H_

#include "exceptionf.h"
#include "FileStream.h"
#include <iconv.h>

struct MDXVoiceOsc {
	uint8_t dt1_mul;
	uint8_t tl;
	uint8_t ks_ar;
	uint8_t ame_d1r;
	uint8_t dt2_d2r;
	uint8_t d1l_rr;

	void dump() {
		printf("dt1_mul=0x%02x dt1=%d mul=%d\n", dt1_mul, getDT1(), getMUL());
		printf("tl=0x%02x tl=%d\n", tl, getTL());
		printf("ks_ar=0x%02x ks=%d ar=%d\n", ks_ar, getKS(), getAR());
	}

	inline uint8_t getDT1() { return (dt1_mul >> 4) & 0x07; } // DeTune 1
	inline uint8_t getMUL() { return dt1_mul & 0x0f; }        // detune MULtiplier
	inline uint8_t getTL()  { return tl & 0x7f; }             // Total Level (envelope)
	inline uint8_t getKS()  { return ks_ar >> 6; }            // Key Scaling
	inline uint8_t getAR()  { return ks_ar & 0x1f; }          // Attack Rate (envelope)
	inline uint8_t getAME() { return ame_d1r >> 7; }          // AMS Enable
	inline uint8_t getD1R() { return ame_d1r & 0x1f; }        // Decay Rate 1
	inline uint8_t getDT2() { return dt2_d2r >> 6; }          // DeTune 2
	inline uint8_t getD2R() { return dt2_d2r & 0x1f; }        // Decay Rate 2
	inline uint8_t getD1L() { return d1l_rr >> 4; }           // Decay Level 1
	inline uint8_t getRR()  { return d1l_rr & 0x0f; }         // Release Rate
};

struct MDXVoice {
	uint8_t number, fl_con, slot_mask;
	MDXVoiceOsc osc[4];

	bool load(FileStream &s) {
		uint8_t buf[27];
		if((unsigned long)s.read(buf, sizeof(buf)) < sizeof(buf)) return false;
		uint8_t *b = buf;
		number = *b++;
		fl_con = *b++;
		slot_mask = *b++;
		for(int i = 0; i < 4; i++, b++) osc[i].dt1_mul = *b;
		for(int i = 0; i < 4; i++, b++) osc[i].tl = *b;
		for(int i = 0; i < 4; i++, b++) osc[i].ks_ar = *b;
		for(int i = 0; i < 4; i++, b++) osc[i].ame_d1r = *b;
		for(int i = 0; i < 4; i++, b++) osc[i].dt2_d2r = *b;
		for(int i = 0; i < 4; i++, b++) osc[i].d1l_rr = *b;
		return true;
	}

	void dump() {
		printf("Voice %d\n", number);
		printf("fl_con=0x%02x fl=%d con=%d slot_mask=0x%02x\n", fl_con, getFL(), getCON(), slot_mask);
		for(int i = 0; i < 4; i++) {
			printf("Osc %d\n", i);
			osc[i].dump();
		}
	}

	uint8_t getFL() { return (fl_con >> 3) & 0x07; }
	uint8_t getCON() { return fl_con & 0x07; }
};

class MDX;
class MDXChannelParser {
	friend class MDX;
public:
	int channel;
protected:
	int channelLength, pos;
	uint8_t nn, n2, n3, n4, n5;
	int16_t w, v;
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
		LFOPitchWave,
		LFOPitchPeriodMSB,
		LFOPitchPeriodLSB,
		LFOPitchChangeMSB,
		LFOPitchChangeLSB,
		LFOVolumeWave,
		LFOVolumePeriodMSB,
		LFOVolumePeriodLSB,
		LFOVolumeChangeMSB,
		LFOVolumeChangeLSB,
		OPMLFOSyncWave,
		OPMLFOLFRQ,
		OPMLFOPMD,
		OPMLFOAMD,
		OPMLFOPMSAMS,
		FadeOutB1,
		FadeOutValue,
	} state;
public:
	MDXChannelParser() { reset(); }
	void reset() {
		nn = n2 = n3 = n4 = n5 = w = v = pos = 0;
		state = None;
		cleanup();
	}
	virtual void cleanup() {}
	bool eat(uint8_t b) {
		handleByte(b);
		pos++;
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
							handleCommand(b);
							state = None;
							break;
						case 0xf9:
							handleVolumeInc();
							handleCommand(b);
							state = None;
							break;
						case 0xf8:
							state = SoundLen;
							break;
						case 0xf7:
							handleDisableKeyOff();
							handleCommand(b);
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
							handleCommand(b);
							state = None;
							break;
						case 0xed:
							state = ADPCMNoiseFreqVal;
							break;
						case 0xec:
							state = LFOPitchWave;
							break;
						case 0xeb:
							state = LFOVolumeWave;
							break;
						case 0xea:
							state = OPMLFOSyncWave;
							break;
						case 0xe9:
							state = LFODelayVal;
							break;
						case 0xe8:
							handlePCM8ExpansionShift();
							handleCommand(b);
							state = None;
							break;
						case 0xe7:
							state = FadeOutB1;
							break;
						default:
							handleUndefinedCommand(b);
							handleCommand(b);
							break;
					}
				}
				break;
			case NoteDuration:
				handleNote(nn - 0x80, b + 1);
				state = None;
				break;
			case TempoVal:
				handleSetTempo(b);
				handleCommand(0xff, b);
				state = None;
				break;
			case OPMRegisterNum:
				nn = b;
				state = OPMRegisterVal;
				break;
			case OPMRegisterVal:
				handleSetOPMRegister(nn, b);
				handleCommand(0xfe, nn, b);
				state = None;
				break;
			case VoiceNum:
				handleSetVoiceNum(b);
				handleCommand(0xfd, b);
				state = None;
				break;
			case OutputPhaseVal:
				handleOutputPhase(b);
				handleCommand(0xfc, b);
				state = None;
				break;
			case VolumeVal: {
					int vol_conv[] = {
						85,  87,  90,  93,  95,  98, 101, 103,
						106, 109, 111, 114, 117, 119, 122, 125
					};

					int vol = b < 16 ? vol_conv[b] : 255 - b;
					if(vol > 127) vol = 127;

					handleSetVolume(vol);
					handleCommand(0xfb, b);
					state = None;
				}
				break;
			case SoundLen:
				handleSoundLength(b);
				handleCommand(0xf8, b);
				state = None;
				break;
			case RepeatStartCount:
				handleRepeatStart(b);
				handleCommand(0xf6, b);
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
				handleCommand(0xf5, (nn << 8) | b);
				state = None;
				break;
			case RepeatEscapeMSB:
				nn = b;
				state = RepeatEscapeLSB;
				break;
			case RepeatEscapeLSB:
				handleRepeatEscape((nn << 8) | b);
				handleCommand(0xf4, (nn << 8) | b);
				state = None;
				break;
			case DetuneMSB:
				nn = b;
				state = DetuneLSB;
				break;
			case DetuneLSB:
				handleDetune((nn << 8) | b);
				handleCommand(0xf3, (nn << 8) | b);
				state = None;
				break;
			case PortamentoMSB:
				nn = b;
				state = PortamentoLSB;
				break;
			case PortamentoLSB:
				handlePortamento((nn << 8) | b);
				handleCommand(0xf2, (nn << 8) | b);
				state = None;
				break;
			case DataEndMSB:
				if(b == 0) {
					handleDataEnd();
					handleCommand(0xf1, 0);
					state = None;
					return true;
				} else {
					nn = b;
					state = DataEndLSB;
				}
				break;
			case DataEndLSB:
				handleDataEnd((nn << 8) | b);
				handleCommand(0xf1, (nn << 8) | b);
				state = None;
				return true;
				break;
			case KeyOnDelayVal:
				handleKeyOnDelay(b);
				handleCommand(0xf0, b);
				break;
			case SyncSendChannel:
				handleSyncSend(b);
				handleCommand(0xef, b);
				state = None;
				break;
			case ADPCMNoiseFreqVal:
				handleADPCMNoiseFreq(b);
				handleCommand(0xed, b);
				state = None;
				break;
			case LFOPitchWave:
				if(b == 0x80) {
					handleLFOPitchMPOF();
					handleCommand(0xec, b);
					state = None;
				} else if(b == 0x81) {
					handleLFOPitchMPON();
					handleCommand(0xec, b);
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
				handleLFOPitch(nn, nn == 0 ? w>>2 : w>>1, nn == 1 ? (v + 255) >> 8 : (v * w + 511) >> 9);
				handleCommand(0xec, nn, w, v);
				state = None;
				break;
			case LFOVolumeWave:
				if(b == 0x80) {
					handleLFOVolumeMAOF();
					handleCommand(0xeb, b);
					state = None;
				} else if(b == 0x81) {
					handleLFOVolumeMAON();
					handleCommand(0xeb, b);
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
				handleLFOVolume(nn, nn == 0 ? w>>2 : w>>1, nn == 1 ? (v + 255) >> 8 : (v * w + 255) >> 8);
				handleCommand(0xeb, nn, w, v);
				state = None;
				break;
			case OPMLFOSyncWave:
				if(b == 0x80) {
					handleOPMLFOMHOF();
					handleCommand(0xea, b);
					state = None;
				} else if(b == 0x81) {
					handleOPMLFOMHON();
					handleCommand(0xea, b);
					state = None;
				} else {
					nn = b;
					state = OPMLFOLFRQ;
				}
				break;
			case OPMLFOLFRQ:
				n2 = b;
				state = OPMLFOPMD;
				break;
			case OPMLFOPMD:
				n3 = b;
				state = OPMLFOAMD;
				break;
			case OPMLFOAMD:
				n4 = b;
				state = OPMLFOPMSAMS;
				break;
			case OPMLFOPMSAMS:
				handleOPMLFO(nn, n2, n3, n4, b);
				handleCommand(0xea, nn, n2, n3, n4, b);
				state = None;
				break;
			case LFODelayVal:
				handleLFODelaySetting(b);
				handleCommand(0xe9, b);
				state = None;
				break;
			case FadeOutB1:
				state = FadeOutValue;
				break;
			case FadeOutValue:
				handleFadeOut(b);
				handleCommand(0xe7, b);
				state = None;
				break;
			default:
				printf("Unknown state %d\n", state);
				break;
		}
		return false;
	}

	virtual void handleByte(uint8_t b) {}
	virtual void handleRest(uint8_t duration) {}
	virtual void handleNote(uint8_t note, uint8_t duration) {}
	virtual void handleCommand(uint8_t c, ...) {}
	virtual void handleVolumeInc() {}
	virtual void handleVolumeDec() {}
	virtual void handleDisableKeyOff() {}
	virtual void handleSyncWait() {}
	virtual void handlePCM8Enable() {}
	virtual void handleSetTempo(uint8_t t) {}
	virtual void handleSetVoiceNum(uint8_t t) {}
	virtual void handleOutputPhase(uint8_t p) {}
	virtual void handleSetVolume(uint8_t v) {}
	virtual void handleSoundLength(uint8_t l) {}
	virtual void handleKeyOnDelay(uint8_t d) {}
	virtual void handleSyncSend(uint8_t s) {}
	virtual void handleADPCMNoiseFreq(uint8_t f) {}
	virtual void handleLFODelaySetting(uint8_t d) {}
	virtual void handleRepeatStart(uint8_t r) {}
	virtual void handleRepeatEnd(int16_t r) {}
	virtual void handleRepeatEscape(int16_t r) {}
	virtual void handleDetune(int16_t d) {}
	virtual void handlePortamento(int16_t t) {}
	virtual void handleSetOPMRegister(uint8_t reg, uint8_t val) {}
	virtual void handleDataEnd() {}
	virtual void handleDataEnd(int16_t end) {}
	virtual void handleLFOPitch(uint8_t b, uint16_t period, uint16_t change) {}
	virtual void handleLFOPitchMPON() {}
	virtual void handleLFOPitchMPOF() {}
	virtual void handleLFOVolume(uint8_t b, uint16_t period, uint16_t change) {}
	virtual void handleLFOVolumeMAON() {}
	virtual void handleLFOVolumeMAOF() {}
	virtual void handleOPMLFO(uint8_t sync_wave, uint8_t lfrq, uint8_t pmd, uint8_t amd, uint8_t pms_ams) {}
	virtual void handleOPMLFOMHON() {}
	virtual void handleOPMLFOMHOF() {}
	virtual void handleFadeOut(uint8_t f) {}
	virtual void handlePCM8ExpansionShift() {}
	virtual void handleUndefinedCommand(uint8_t b) {}
	virtual void handleChannelStart(int chan) {}
	virtual void handleChannelEnd(int chan) {}
};

class MDX {
protected:
	uint16_t file_base, Voice_offset, mml_offset[16];
	uint8_t num_channels;
	FileStream s;
public:
	const char *title, *pcm_file;
	MDX(const char *filename) {
		load(filename);
	}
	MDX() {}
	~MDX() {}

	void load(const char *filename, MDXChannelParser &p) {
		s.open(filename);
		readHeader();
		readVoices();
		for(int i = 0; i < num_channels; i++) {
			p.channel = i;
			readChannel(i, p);
		}
	}
	void load(const char *filename) {
		MDXChannelParser p;
		load(filename, p);
	}
	void readHeader() {
		title = s.readLine(0x1a);
		if(title && *title) {
			char *nl = strrchr((char *)title, '\r');
			if(nl) *nl = 0;
		}
		pcm_file = s.readLine(0);
		file_base = s.tell();
		Voice_offset = s.readUint16Big();
		memset(mml_offset, 0, sizeof(mml_offset));
		mml_offset[0] = s.readUint16Big();
		num_channels = mml_offset[0] / 2 - 1;
		if(num_channels > 16) num_channels = 16;
		for(int i = 1; i < num_channels; i++) {
			mml_offset[i] = s.readUint16Big();
		}
		handleHeader();
	}
	void readVoices() {
		s.seek(file_base + Voice_offset);
		while(!s.eof()) {
			MDXVoice inst;
			if(!inst.load(s)) break;
			handleVoice(inst);
		}
	}
	void readChannel(int i, MDXChannelParser &p) {
		s.seek(file_base + mml_offset[i]);
		int chan_endoff = i < num_channels - 1 ? mml_offset[i+1] : Voice_offset;
		int chan_end = file_base + chan_endoff;
		bool done = false;
		p.reset();
		p.channel = i;
		p.channelLength = chan_endoff - mml_offset[i];
		p.handleChannelStart(i);
		while(!s.eof() && !done) {
			uint8_t b = s.readUint8();
			if(s.eof()) break;
			done = p.eat(b);
			if(chan_end && s.tell() >= chan_end) break;
		}
		p.handleChannelEnd(i);
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
	static const char *voiceName(uint8_t n) {
		const char *voiceNames[] = { "M1", "C1", "M2", "C2" };
		return voiceNames[n & 0x03];
	}
	static const char *noteName(int note) {
		const char *noteNames[] = { "c", "c+", "d", "d+" , "e", "f", "f+", "g", "g+", "a", "a+", "b",  };
		return noteNames[(note + 3) % 12];
	}
	static int noteOctave(int note) {
		return (note + 3) / 12;
	}
	static char channelName(uint8_t chan) {
		if(chan < 8) return 'A' + chan;
		if(chan < 16) return 'P' + chan - 8;
		return '!';
	}

	virtual void handleHeader() {} // Called right after header is loaded
	virtual void handleVoice(MDXVoice &v) {}
};

#endif /* MDX_H_ */

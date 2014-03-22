#ifndef MDX_H_
#define MDX_H_

#include <stdint.h>

#include "exceptionf.h"
#include "Stream.h"
#include "FS.h"

struct MDXVoiceOsc {
	uint8_t dt1_mul;
	uint8_t tl;
	uint8_t ks_ar;
	uint8_t ame_d1r;
	uint8_t dt2_d2r;
	uint8_t d1l_rr;

	void dump() {
		printf("dt1_mul=0x%02x dt1=%d mul=%d ", dt1_mul, getDT1(), getMUL());
		printf("tl=0x%02x tl=%d ", tl, getTL());
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

	bool load(ReadStream &s) {
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

	static const char *oscName(uint8_t n) {
		const char *oscNames[] = { "M1", "C1", "M2", "C2" };
		return oscNames[n & 0x03];
	}

	void dump() {
		printf("Voice %d: fl_con=0x%02x fl=%d con=%d slot_mask=0x%02x\n", number, fl_con, getFL(), getCON(), slot_mask);
		for(int i = 0; i < 4; i++) {
			printf("%s: ", oscName(i));
			osc[i].dump();
		}
	}

	uint8_t getFL() { return (fl_con >> 3) & 0x07; }
	uint8_t getCON() { return fl_con & 0x07; }
};

struct MDXChannelPos {
	uint16_t offset, length;
};

struct MDXHeader {
	const char *title, *pcmFile;
	uint16_t fileBase;
	MDXChannelPos channels[16];
	uint8_t numChannels;
	uint16_t voiceOffset;
	MDXVoice *voices[256];
	bool reorderVoices;

	MDXHeader(): title(0), pcmFile(0), fileBase(0), numChannels(0), voiceOffset(0), reorderVoices(false) {
		memset(channels, 0, sizeof(channels));
		memset(voices, 0, sizeof(voices));
	}

	~MDXHeader() {
		if(title) delete title;
		if(pcmFile) delete pcmFile;
		for(size_t i = 0; i < 256; i++) {
			if(voices[i]) delete voices[i];
		}
	}

	void read(FileReadStream &s);
	void dump();
};

class MDXParser {
public:
	int channel;
protected:
	int pos;
	uint8_t nn, n2, n3, n4, n5;
	int16_t w, v;
	enum {
		None = 0,           NoteDuration,       TempoVal,
		OPMRegisterNum,     OPMRegisterVal,     VoiceNum,
		PanVal,             VolumeVal,          SoundLen,
		RepeatStartCount,   RepeatStartZero,    RepeatEndOffsetMSB,
		RepeatEndOffsetLSB, RepeatEscapeMSB,    RepeatEscapeLSB,
		DetuneMSB,          DetuneLSB,          PortamentoMSB,
		PortamentoLSB,      DataEndMSB,         DataEndLSB,
		KeyOnDelayVal,      SyncSendChannel,    ADPCMNoiseFreqVal,
		LFODelayVal,        LFOPitchWave,       LFOPitchPeriodMSB,
		LFOPitchPeriodLSB,  LFOPitchChangeMSB,  LFOPitchChangeLSB,
		LFOVolumeWave,      LFOVolumePeriodMSB, LFOVolumePeriodLSB,
		LFOVolumeChangeMSB, LFOVolumeChangeLSB, OPMLFOSyncWave,
		OPMLFOLFRQ,         OPMLFOPMD,          OPMLFOAMD,
		OPMLFOPMSAMS,       ExtendedCmd,        FadeOutValue,
		FlagValue,
	} state;
public:
	MDXParser(): pos(0), nn(0), n2(0), n3(0), n4(0), n5(0), w(0), v(0), state(None) {}
	~MDXParser() {}

	bool eat(uint8_t b);

	static const char *commandName(uint8_t c) {
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

	static int volumeVal(uint8_t v) {
		int vol_conv[] = {
			85,  87,  90,  93,  95,  98, 101, 103,
			106, 109, 111, 114, 117, 119, 122, 125
		};

		if(v < 16) return vol_conv[v];
		return 255 - v;
	}

	// Callbacks
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
	virtual void handlePan(uint8_t p) {}
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
	virtual void handleKill() {}
	virtual void handleFlag(uint8_t f) {}
};

class MDXMemParser: public MDXParser {
public:
	uint8_t *data;
	bool ended;
	int dataLen;
	int loopIterations;
protected:
	int dataPos;
private:
	int repeatStack[5];
	int repeatStackPos;
public:
	MDXMemParser(): MDXParser(), data(0), ended(false), dataLen(0), loopIterations(0), dataPos(0), repeatStackPos(0) {}

	void feed() {
		eat(data[dataPos++]);
		if(dataPos >= dataLen) ended = true;
	}

	virtual void handleDataEnd() {
		ended = true;
	}
	virtual void handleDataEnd(int16_t ofs) {
		if(loopIterations-- > 0) dataPos -= ofs + 3;
		else ended = true;
	}
	virtual void handleRepeatStart(uint8_t iterations) {
		repeatStack[repeatStackPos++] = iterations - 1;
		if(repeatStackPos >= 5) repeatStackPos = 4;
	}
	virtual void handleRepeatEnd(int16_t ofs) {
		if(repeatStack[repeatStackPos - 1]-- > 0) {
			dataPos += ofs;
			if(dataPos < 0) dataPos = 0;
		} else repeatStackPos--;
		if(repeatStackPos < 0) repeatStackPos = 0;
	}
	virtual void handleRepeatEscape(int16_t ofs) {
		if(repeatStack[repeatStackPos - 1] == 0) {
			dataPos += ofs + 2;
			repeatStackPos--;
			if(repeatStackPos < 0) repeatStackPos = 0;
		}
	}
};

#endif /* MDX_H_ */

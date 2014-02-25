#ifndef MDXMML_H_
#define MDXMML_H_

#include "MDX.h"

class MDXPreParserMML: public MDXChannelParser {
public:
	int loopPosition;
private:
	virtual void cleanup() {
		loopPosition = 1; // greater than -1
	}
	virtual void handleDataEnd(int16_t endPos) { loopPosition = endPos; }
};

class MDXParserMML: public MDXChannelParser {
friend class MDXMML;
	int loopPosition;
	int col, lastOctave;
	int repeat_stack[5];
	int repeat_stack_pointer;
	int lastRest;
	bool nextKeyOff;
	virtual void cleanup() {
		col = 0;
		lastOctave = -1;
		repeat_stack_pointer = 0;
		lastRest = 0;
		lastOctave = -1;
		nextKeyOff = false;
	}
	void mmlf(const char *fmt, ...) {
		if(col == 0) printf("%c ", MDX::channelName(channel));
		va_list ap;
		va_start(ap, fmt);
		int printed = vprintf(fmt, ap);
		va_end(ap);
		col += printed;
		if(col > 79) {
			col = 0;
			printf("\n");
		}
	}

	virtual void handleByte(uint8_t b) {
		if(loopPosition < 0 && pos == channelLength + loopPosition) {
			mmlf(" L ");
		}
	}
	virtual void handleNote(uint8_t note, uint8_t duration) {
		if(channel < 8) {
			int oc = MDX::noteOctave(note);
			if(lastOctave != oc) {
				if(lastOctave == -1 || lastOctave - oc > 1 || oc - lastOctave > 1) {
					mmlf("o%d", oc);
				} else if(lastOctave == oc + 1) {
					mmlf("<");
				} else if(lastOctave == oc - 1) {
					mmlf(">");
				}
			}
			lastOctave = oc;
		}
		if(duration > 0) {
			int mmlDuration;
			bool dot = false, percent = false;
			if(192 % duration == 0) {
				mmlDuration = 192 / duration;
			} else if(192 % (duration * 2 / 3) == 0 && (duration * 2) % 3 == 0) {
				dot = true;
				mmlDuration = 192 * 3 / duration / 2;
			} else {
				percent = true;
			}
			if(channel < 8) {
				mmlf("%s%s%d%s%s", MDX::noteName(note), percent ? "%" : "", percent ? duration : mmlDuration, dot ? "." : "", nextKeyOff ? "&" : "");
			} else {
				mmlf("n%d,%s%d%s%s", note, percent ? "%" : "", percent ? duration : mmlDuration, dot ? "." : "", nextKeyOff ? "&" : "");
			}
		}
		nextKeyOff = false;
	}
	virtual void handleRest(uint8_t duration) {
		if(duration == 128) {
			lastRest += duration;
			return;
		}
		duration += lastRest;
		int mmlDuration;
		bool dot = false, percent = false;
		if(192 % duration == 0) {
			mmlDuration = 192 / duration;
		} else if(192 % (duration * 2 / 3) == 0 && (duration * 2) % 3 == 0) {
			dot = true;
			mmlDuration = 192 * 3 / duration / 2;
		} else {
			percent = true;
		}
		mmlf("r%s%d%s", percent ? "%" : "", percent ? duration : mmlDuration, dot ? "." : "");
		lastRest = 0;
	}
	virtual void handleSetVoiceNum(uint8_t voice) {
		mmlf("@%d", voice);
	}
	virtual void handleSetTempo(uint8_t tempo) {
		mmlf("@t%d", tempo);
	}
	virtual void handleLFOPitch(uint8_t b, uint16_t period, uint16_t change) {
		mmlf("MP%d,%d,%d", b, period, change);
	}
	virtual void handleLFOPitchMPON() {
		mmlf("MPON");
	}
	virtual void handleLFOPitchMPOF() {
		mmlf("MPOF");
	}
	virtual void handleLFOVolume(uint8_t b, uint16_t period, uint16_t change) {
		mmlf("MA%d,%d,%d", b, period, change);
	}
	virtual void handleLFOVolumeMAON() {
		mmlf("MAON");
	}
	virtual void handleLFOVolumeMAOF() {
		mmlf("MAOF");
	}
	virtual void handleSetVolume(uint8_t volume) {
		if(volume < 16) mmlf("v%u", volume);
		else mmlf("@v%u", 255 - volume);
	}
	virtual void handleVolumeInc() { mmlf(")"); }
	virtual void handleVolumeDec() { mmlf("("); }

	virtual void handleRepeatStart(uint8_t times) {
		lastOctave = -1;
		if(repeat_stack_pointer < 5) {
			repeat_stack[repeat_stack_pointer++] = times;
		}
		mmlf("[");
	}
	virtual void handleRepeatEnd(int16_t offset) {
		lastOctave = -1;
		if(repeat_stack_pointer >= 0) {
			int r = repeat_stack[--repeat_stack_pointer];
			if(r > 2) mmlf("]%d", r); else mmlf("]");
		}
	}
	virtual void handleSoundLength(uint8_t l) { mmlf("q%d", l); }
	virtual void handlePan(uint8_t p) { mmlf("p%d", p); }
	virtual void handleDisableKeyOff() { nextKeyOff = true; }
	virtual void handleDetune(int16_t d) { mmlf("D%d", d); }
	virtual void handleOPMLFO(uint8_t sync_wave, uint8_t lfrq, uint8_t pmd, uint8_t amd, uint8_t pms_ams) {
		mmlf("MH%d,%d,%d,%d,%d,%d,%d", sync_wave & 0x1f, lfrq, pmd-128, amd, (pms_ams & 0xf0) >> 4, pms_ams & 0x0f, (sync_wave & 0x40) >> 6);
	}
	virtual void handleOPMLFOMHON() { mmlf("MHON"); }
	virtual void handleOPMLFOMHOF() { mmlf("MHOF"); }
	virtual void handleChannelStart(int chan) {
		printf("\n/* Channel %c (%s) */\n", MDX::channelName(chan), chan < 8 ? "FM" : "PCM");
	}
	virtual void handleChannelEnd(int chan) {
		printf("\n");
	}
};

class MDXMML: public MDX {
public:
	MDXMML(const char *filename) {
		load(filename);
	}

	void load(const char *filename) {
		s.open(filename);
		readHeader();
		readVoices();
		MDXPreParserMML r; // run a pre-pass to determine the loop positions (they're at the end of each channel)
		int loopPositions[16];
		for(int i = 0; i < num_channels; i++) {
			readChannel(i, r);
			loopPositions[i] = r.loopPosition;
		}
		MDXParserMML p;
		for(int i = 0; i < num_channels; i++) {
			p.loopPosition = loopPositions[i];
			readChannel(i, p);
		}
	}

	virtual void handleHeader() {
		printf("#title \"%s\"\n", title);
		printf("#pcmfile \"%s\"\n", pcm_file);
	}
	virtual void handleVoice(MDXVoice &v) {
		printf("@%d={\n\t/* AR DR SR RR SL OL KS ML DT1 DT2 AME */\n", v.number);
		int indices[4] = { 0, 2, 1, 3 };
		for(int n = 0; n < 4; n++) {
			int i = indices[n];
			printf("\t%d, %d, %d, %d,  %d, %d, %d, %d,  %d, %d, %d,\n",
				v.osc[i].getAR(), v.osc[i].getD1R(), v.osc[i].getD2R(),
				v.osc[i].getRR(), v.osc[i].getD1L(), v.osc[i].getTL(), v.osc[i].getKS(),
				v.osc[i].getMUL(), v.osc[i].getDT1(), v.osc[i].getDT2(), v.osc[i].getAME()
			);
		}
		printf("\t%d, %d, %d\n}\n\n", v.getCON(), v.getFL(), v.slot_mask);
	}
	int nextValidDuration(uint8_t d) {
		if(d <= 4) return d;
		int i = 2;
		while(i < 64) {
			if(d < i*2) return i / 2 * 3;
			if(d < i*3) return i * 2;
			i *= 2;
		}
		return i;
	}
};

#endif /* MDXMML_H_ */

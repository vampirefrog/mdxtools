#include "MDX.h"

class MDXPreParserMML: public MDXParser {
public:
	int loopPosition;
private:
	virtual void cleanup() {
		loopPosition = 1; // greater than -1
	}
	virtual void handleDataEnd(int16_t endPos) { loopPosition = endPos; }
};

class MDXParserMML: public MDXParser {
public:
	int channelLength;
	int loopPosition;
	int col, lastOctave;
	int repeatStack[5];
	int repeatStackPointer;
	int lastRest;
	bool nextKeyOff;
	int nextPortamento;
	MDXParserMML(): MDXParser(), channelLength(0), loopPosition(0), col(0), lastOctave(-1), repeatStackPointer(0), lastRest(0), nextKeyOff(false), nextPortamento(0) {}

	void mmlf(const char *fmt, ...) {
		if(col == 0) printf("%c ", channelName(channel));
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
		int octave = noteOctave(note);
		if(channel < 8) {
			if(lastOctave != octave) {
				if(lastOctave == -1 || lastOctave - octave > 1 || octave - lastOctave > 1) {
					mmlf("o%d", octave);
				} else if(lastOctave == octave + 1) {
					mmlf("<");
				} else if(lastOctave == octave - 1) {
					mmlf(">");
				}
			}
			lastOctave = octave;
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
			char buf[100];
			if(channel < 8) {
				sprintf(buf, "%s", noteName(note));
			} else {
				sprintf(buf, "n%d,", note);
			}
			if(percent) sprintf(buf + strlen(buf), "%%%d", duration);
			else sprintf(buf + strlen(buf), "%d", mmlDuration);
			if(dot) strcat(buf, ".");
			if(nextKeyOff) strcat(buf, "%");
			if(nextPortamento) {
				int nextNote = note + (nextPortamento * duration + (nextPortamento < 0 ? -16383 : 16383)) / 16384;
				int nextOctave = noteOctave(nextNote);
				strcat(buf, "_");
				if(nextOctave != octave) sprintf(buf + strlen(buf), "o%d", nextOctave);
				lastOctave = nextOctave;
				sprintf(buf + strlen(buf), "%s", noteName(nextNote));
			}
			mmlf(buf);
		}
		nextKeyOff = false;
		nextPortamento = 0;
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
		if(repeatStackPointer < 5) {
			repeatStack[repeatStackPointer++] = times;
		}
		mmlf("[");
	}
	virtual void handleRepeatEnd(int16_t offset) {
		lastOctave = -1;
		if(repeatStackPointer >= 0) {
			int r = repeatStack[--repeatStackPointer];
			if(r > 2) mmlf("]%d", r); else mmlf("]");
		}
	}
	virtual void handleRepeatEscape(int16_t r) { mmlf("/"); }
	virtual void handlePan(uint8_t p) { mmlf("p%d", p); }
	virtual void handleDisableKeyOff() { nextKeyOff = true; }
	virtual void handleKeyOnDelay(uint8_t d) { mmlf("k%d", d); }
	virtual void handleSoundLength(uint8_t l) {
		if(l <= 8) mmlf("q%d", l);
		else mmlf("@q%d", 255 - l);
	}
	virtual void handleDetune(int16_t d) { mmlf("D%d", d); }
	virtual void handleOPMLFO(uint8_t sync_wave, uint8_t lfrq, uint8_t pmd, uint8_t amd, uint8_t pms_ams) {
		mmlf("MH%d,%d,%d,%d,%d,%d,%d", sync_wave & 0x1f, lfrq, pmd-128, amd, (pms_ams & 0xf0) >> 4, pms_ams & 0x0f, (sync_wave & 0x40) >> 6);
	}
	virtual void handleADPCMNoiseFreq(uint8_t f) {
		if(channel < 8) mmlf("w%d", f & 0x7f);
		else mmlf("F%d", f & 0x7f);
	}
	virtual void handleOPMLFOMHON() { mmlf("MHON"); }
	virtual void handleOPMLFOMHOF() { mmlf("MHOF"); }
	virtual void handleSyncWait() { mmlf("W"); }
	virtual void handleSyncSend(uint8_t chan) {
		if(chan < 16) mmlf("S%d", chan);
	}
	virtual void handleSetOPMRegister(uint8_t r, uint8_t v) {
		mmlf("y%d,%d", r, v);
	}
	virtual void handlePortamento(int16_t p) {
		nextPortamento = p;
	}
};

static void printVoice(MDXVoice *v) {
	printf("@%d={\n\t/* AR DR SR RR SL OL KS ML DT1 DT2 AME */\n", v->number);
	int indices[4] = { 0, 2, 1, 3 };
	for(int n = 0; n < 4; n++) {
		int i = indices[n];
		printf("\t%d, %d, %d, %d,  %d, %d, %d, %d,  %d, %d, %d,\n",
			v->osc[i].getAR(), v->osc[i].getD1R(), v->osc[i].getD2R(),
			v->osc[i].getRR(), v->osc[i].getD1L(), v->osc[i].getTL(), v->osc[i].getKS(),
			v->osc[i].getMUL(), v->osc[i].getDT1(), v->osc[i].getDT2(), v->osc[i].getAME()
		);
	}
	printf("\t%d, %d, %d\n}\n\n", v->getCON(), v->getFL(), v->slot_mask);
}

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		try {
			FileReadStream s(argv[i]);
			MDXHeader h;
			h.read(s);
			printf("/* %s */\n", argv[i]);
			if(h.title) printf("#title \"%s\"\n", h.title);
			if(h.pcmFile) printf("#pcmfile \"%s\"\n", h.pcmFile);
			for(int i = 0; i < 256; i++) {
				if(h.voices[i]) printVoice(h.voices[i]);
			}
			MDXPreParserMML pre[16]; // run a pre-pass to determine the loop positions (they're at the end of each channel)
			for(int i = 0; i < h.numChannels; i++) {
				s.seek(h.fileBase + h.channels[i].offset);
				for(size_t j = 0; !s.eof() && j < h.channels[i].length; j++)
					pre[i].eat(s.readUint8());
			}
			for(int i = 0; i < h.numChannels; i++) {
				printf("/* Channel %c (%s) */\n", MDXParser::channelName(i), i >= 8 ? "ADPCM" : "FM");
				MDXParserMML p;
				p.channel = i;
				p.loopPosition = pre[i].loopPosition;
				p.channelLength = h.channels[i].length;
				s.seek(h.fileBase + h.channels[i].offset);
				for(size_t j = 0; !s.eof() && j < h.channels[i].length; j++)
					p.eat(s.readUint8());
				printf("\n\n");
			}
		} catch(exceptionf *e) {
			printf("Exception caught: %s\n", e->what());
		}
	}

	return 0;
}

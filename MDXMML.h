#ifndef MDXMML_H_
#define MDXMML_H_

#include "MDX.h"

class MDXMML: public MDX {
public:
	MDXMML(const char *filename): cur_chan(0), col(0) { load(filename); }

	virtual void handleHeader() {
		printf("#title \"%s\"\n", title);
		printf("#pcmfile \"%s\"\n", pcm_file);
		cur_chan = 0;
	}
	virtual void handleVoice(MDXVoice &v) {
		printf("@%d={\n", v.number);
		int indices[4] = { 0, 2, 1, 3 };
		for(int n = 0; n < 4; n++) {
			int i = indices[n];
			printf("%d, %d, %d, %d,  %d, %d, %d, %d,  %d, %d, %d,\n", v.osc[i].ar, v.osc[i].d1r, v.osc[i].d2r, v.osc[i].rr, v.osc[i].d1l, v.osc[i].tl, v.osc[i].ks, v.osc[i].mul, v.osc[i].dt1, v.osc[i].dt2, v.osc[i].ame);
		}
		printf("%d, %d, %d}\n\n", v.con, v.fl, v.slot_mask);
	}
	int cur_chan;
	virtual void handleChannelStart(int chan) {
		printf("\n/* Channel %c (%s)*/\n", channelName(chan), chan < 8 ? "FM" : "PCM");
		cur_chan = chan;
		col = 0;
		repeat_stack_pointer = 0;
	}
	virtual void handleChannelEnd(int chan) {
		printf("\n");
	}
	int col;
	void mmlf(const char *fmt, ...) {
		if(col == 0) printf("%c ", channelName(cur_chan));
		va_list ap;
		va_start(ap, fmt);
		int printed = vprintf(fmt, ap);
		va_end(ap);
		col += printed;
		if(col > 80) {
			col = 0;
			printf("\n");
		}
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
	virtual void handleNote(uint8_t note, uint8_t duration) {
		duration = duration / 3 * 4;
		while(duration > 0) {
			int nd = nextValidDuration(duration);
			mmlf("n%d,%d%s", note, nd, nd != duration ? "&" : "");
			duration -= nd;
		}
	}
	virtual void handleRest(uint8_t duration) {
		duration /= 64;
		while(duration > 0) {
			int nd = nextValidDuration(duration);
			mmlf("r%d", nd);
			duration -= nd;
		}
	}
	virtual void handleSetVoiceNum(uint8_t voice) {
		mmlf("@%d", voice);
	}
	virtual void handleSetTempo(uint8_t tempo) {
		mmlf("@t%d", tempo);
	}
	virtual void handleLFOPitch(uint8_t b, uint16_t period, uint16_t change) {
		if(b == 0) { change = period * change / 512; period /= 4; }
		if(b == 1) { change = period * change / 256; period /= 2; }
		if(b == 2) { change = period * change / 256; period /= 2; }
		mmlf("MP%d,%d,%d", b, period, change);
	}
	virtual void handleLFOPitchMPON() {
		mmlf("MPON");
	}
	virtual void handleLFOPitchMPOF() {
		mmlf("MPOF");
	}
	virtual void handleSetVolume(uint8_t volume) {
		if(volume <= 15) mmlf("v%d", volume);
		else mmlf("@v%d", volume & 0x7f);
	}
	virtual void handleVolumeInc() { mmlf(")"); }
	virtual void handleVolumeDec() { mmlf("("); }

	int repeat_stack[5];
	int repeat_stack_pointer;
	virtual void handleRepeatStart(uint8_t times) {
		if(repeat_stack_pointer < 5) {
			repeat_stack[repeat_stack_pointer++] = times;
		}
		mmlf("[");
	}
	virtual void handleRepeatEnd(int16_t offset) {
		if(repeat_stack_pointer >= 0) {
			mmlf("]%d", repeat_stack[--repeat_stack_pointer]);
		}
	}
};

#endif /* MDXMML_H_ */

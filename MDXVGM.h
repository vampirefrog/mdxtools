#ifndef MDXVGM_H_
#define MDXVGM_H_

#include "MDXSerializer.h"
#include "VGMWriter.h"

class MDXVGM: public MDXSerializer {
	VGMWriter w;
	int currentVoices[16];
public:
	MDXVGM() {}
	MDXVGM(const char *filename, const char *outfile) {
		memset(&voices, 0, sizeof(voices));
		memset(currentVoices, 0, sizeof(currentVoices));
		w.version = 0x150;
		w.ym2151_clock = 4000000;
		w.rate = 60; // japanese
		w.writeYM2151(0x01, 0x00);
		w.writeYM2151(0x0f, 0x00);
		w.writeYM2151(0x14, 0x00);
		w.writeYM2151(0x18, 0x00);
		w.writeYM2151(0x19, 0x80);
		w.writeYM2151(0x1b, 0x00);

		load(filename);

		w.write(outfile);
	}
private:
	virtual void handleSetVoiceNum(MDXSerialParser *p, uint8_t voice) {
		if(p->channel >= 8) return;
		MDXVoice &v = voices[voice];
		w.writeYM2151(0x20 + p->channel, (p->pan << 6) | ((v.getFL() & 0x07) << 3) | (v.getCON() & 0x07));
		for(int i = 0; i < 4; i++) {
			MDXVoiceOsc &o = v.osc[i];
			w.writeYM2151(0x40 + i * 8 + p->channel, (o.getDT1() << 4) | (o.getMUL()));
			w.writeYM2151(0x60 + i * 8 + p->channel, o.getTL());
			w.writeYM2151(0x80 + i * 8 + p->channel, (o.getKS() << 6) | o.getAR());
			w.writeYM2151(0xa0 + i * 8 + p->channel, (o.getAME() << 7) | o.getD1R());
			w.writeYM2151(0xc0 + i * 8 + p->channel, (o.getDT2() << 6) | o.getD2R());
			w.writeYM2151(0xe0 + i * 8 + p->channel, (o.getD1L() << 4) | o.getRR());
		}
	}
	virtual void handleNote(MDXSerialParser *p, int n) {
		if(p->channel >= 8) return;
		w.writeYM2151(0x28 + p->channel, ((n / 12) << 4) | ((n % 12) * 16 / 12));
		w.writeYM2151(0x08, (voices[p->curVoice].slot_mask << 3) + (p->channel & 0x07)); // Key ON/OFF
	}
	virtual void handleNoteEnd(MDXSerialParser *p) {
		if(p->channel >= 8) return;
		w.writeYM2151(0x08, p->channel & 0x07);
	}
	virtual void handleRest(int r) {
		// divided 44100 and 4000000 by 100, to avoid int overflow
		int samples = r * 441 * 1024 * (256 - tempo) / 40000;
		w.writeWait(samples);
	}
	virtual void handleSetVolume(MDXSerialParser *p, uint8_t v) {
		int vol_conv[] = {
			85,  87,  90,  93,  95,  98, 101, 103,
			106, 109, 111, 114, 117, 119, 122, 125
		};

		int vol = v < 16 ? vol_conv[v] : 255 - v;
		if(v > 127) v = 127;
		for(int i = 0; i < 4; i++) {
			w.writeYM2151(0x60 + i * 8 + p->channel, 127 - (127 - p->curVoiceData.osc[i].getTL()) * vol / 127);
		}
	}
	virtual void handleDetune(MDXSerialParser *p, int16_t d) {
		w.writeYM2151(0x30 + p->channel, (d & 0x3f) << 2);
	}
};

#endif /* MDXVGM_H_ */

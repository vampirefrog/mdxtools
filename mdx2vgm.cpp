#include <stdio.h>

#include "exceptionf.h"
#include "MDXSerializer.h"
#include "VGMWriter.h"

class MDXVGM: public MDXSerializer {
	VGMWriter w;
	MDXVoice voices[256];
	int currentVoices[16];
public:
	MDXVGM() {}
	MDXVGM(const char *filename, const char *outfile) {
		memset(&voices, 0, sizeof(voices));
		memset(currentVoices, 0, sizeof(currentVoices));
		load(filename);
		w.version = 0x150;
		w.ym2151_clock = 4000000;
		w.rate = 60; // japanese
		printf("writing %s\n", outfile);
		w.write(outfile);
	}
private:
	virtual void handleVoice(MDXVoice &v) {
		voices[v.number] = v;
	}
	void loadVoice(int chan, uint8_t voice, int volume = 127) {
		currentVoices[chan] = voice;
		MDXVoice &v = voices[voice];
		printf("v.pan = %d\n", v.pan);
		w.writeYM2151(0x20 + chan, ((v.pan & 0xa0)) | ((v.fl & 0x07) << 3) | (v.con & 0x07));
		for(int i = 0; i < 4; i++) {
			MDXVoiceOsc &o = v.osc[i];
			w.writeYM2151(0x40 + i * 8 + chan, ((o.dt1 & 0x07) << 4) | ((o.mul & 0x0f))); // DT1 & MUL
			w.writeYM2151(0x60 + i * 8 + chan, ((o.tl * volume) / 127) & 0x7f); // TL
			w.writeYM2151(0x80 + i * 8 + chan, ((o.ks & 0x03) << 6) | (o.ar & 0x1f));
			w.writeYM2151(0xa0 + i * 8 + chan, ((o.ame & 0x01) << 7) | (o.d1r & 0x1f));
			w.writeYM2151(0xc0 + i * 8 + chan, ((o.dt2 & 0x03) << 6) | (o.d2r & 0x1f));
			w.writeYM2151(0xe0 + i * 8 + chan, ((o.d1l & 0x0f) << 4) | (o.rr & 0x0f));
		}
	}
	virtual void handleSetVoiceNum(int chan, uint8_t voice) {
		if(chan >= 8) return;
		loadVoice(chan, voice);
	}
	virtual void handleNote(int chan, int n) {
		if(chan >= 8) return;
		w.writeYM2151(0x28 + chan, ((n / 12) << 4) | ((n % 12) * 16 / 12));
		w.writeYM2151(0x08, (voices[currentVoices[chan]].slot_mask << 3) + (chan & 0x07)); // Key ON/OFF
	}
	virtual void handleNoteEnd(int chan) {
		if(chan >= 8) return;
		w.writeYM2151(0x08, chan & 0x07);
	}
	virtual void handleRest(int r) {
		// divided 44100 and 4000000 by 100, to avoid int overflow
		int samples = r * 441 * 1024 * (256 - tempo) / 40000;
		w.writeWait(samples);
	}
	virtual void handleSetVolume(int channel, uint8_t v) {
		int vol_conv[] = {
			85,  87,  90,  93,  95,  98, 101, 103,
			106, 109, 111, 114, 117, 119, 122, 125
		};

		int vol = v < 16 ? vol_conv[v] : 255 - v;
		loadVoice(channel, currentVoices[channel], vol);
		printf("Set volume %d %d -> %d\n", channel, v, 127 - vol);
	}
};

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		try {
			char buf[256];
			snprintf(buf, sizeof(buf), "%s.vgm", argv[i]);
			MDXVGM s(argv[i], buf);
		} catch(exceptionf &e) {
			fprintf(stderr, "Error: %s\n", e.what());
		}
	}
	return 0;
}

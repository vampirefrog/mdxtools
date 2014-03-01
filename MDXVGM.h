#ifndef MDXVGM_H_
#define MDXVGM_H_

#include "MDXSerializer.h"
#include "VGMWriter.h"
#include "PDX.h"
#include "tools.h"
#include "exceptionf.h"

class MDXVGM: public MDXSerializer {
	VGMWriter w;
public:
	MDXVGM() {}
	MDXVGM(const char *filename, const char *outfile) {
		memset(&voices, 0, sizeof(voices));
		w.version = 0x150;
		w.ym2151_clock = 4000000;
		w.okim6258_clock = 8000000;
		w.okim6258_flags = 0x06;
		w.rate = 60; // japanese

		load(filename);

		w.write(outfile);
	}
private:
	PDXLoader pdx;
	virtual void handleHeader() {
		if(pcm_file && *pcm_file) {
			try {
				pdx.open(pcm_file);
				for(int i = 0; i < PDX_NUM_SAMPLES; i++) {
						w.buf.writeUint8(0x67);
						w.buf.writeUint8(0x66);
						w.buf.writeUint8(0x04); // OKIM6295 ROM data
						w.buf.writeLittleUint32(pdx.samples[i].length);
					if(pdx.samples[i].length > 0) {
						uint8_t *s = pdx.loadSample(i);
						w.buf.write(s, pdx.samples[i].length);
						delete s;
					}
				}
			} catch(exceptionf e) {
				printf("PDX Error: %s\n", e.what());
			}
		}
		w.writeYM2151(0x01, 0x00);
		w.writeYM2151(0x0f, 0x00);
		w.writeYM2151(0x14, 0x00);
		w.writeYM2151(0x18, 0x00);
		w.writeYM2151(0x19, 0x80);
		w.writeYM2151(0x1b, 0x00);
		uint8_t pcm_cmds[] = {
			0x90, 0x00, 0x17, 0x00, 0x01, // Stream #0 - Setup Chip: OKIM6258: Reg 00 01
			0x91, 0x00, 0x04, 0x01, 0x00, // DAC Ctrl:	Stream #0 - Set Data: Bank n (note value), Step Size 01, Step Base 00
		};
		for(unsigned int i = 0; i < sizeof(pcm_cmds); i++) {
			w.buf.writeUint8(pcm_cmds[i]);
		}
	}
	virtual void handleSetVoiceNum(MDXSerialParser *p, uint8_t voice) {
		if(p->channel >= 8) return;
		w.writeYM2151(0x20 + p->channel, (p->pan << 6) | ((p->curVoiceData.getFL() & 0x07) << 3) | (p->curVoiceData.getCON() & 0x07));
		for(int i = 0; i < 4; i++) {
			MDXVoiceOsc &o = p->curVoiceData.osc[i];
			w.writeYM2151(0x40 + i * 8 + p->channel, (o.getDT1() << 4) | (o.getMUL()));
			w.writeYM2151(0x60 + i * 8 + p->channel, 127 - (127 - o.getTL()) * p->volume / 127);
			w.writeYM2151(0x80 + i * 8 + p->channel, (o.getKS() << 6) | o.getAR());
			w.writeYM2151(0xa0 + i * 8 + p->channel, (o.getAME() << 7) | o.getD1R());
			w.writeYM2151(0xc0 + i * 8 + p->channel, (o.getDT2() << 6) | o.getD2R());
			w.writeYM2151(0xe0 + i * 8 + p->channel, (o.getD1L() << 4) | o.getRR());
		}
	}
	virtual void handleNote(MDXSerialParser *p, int n) {
		if(p->channel == 8) {
			uint8_t pcm_cmds[] = {
				0xb7, 0x02, 0x00, // OKIM6258:	Pan Write: LR
				0xb7, 0x00, 0x02, // OKIM6258:	Control Write: Play: On, Record: Off
				0xb7, 0x0c, 0x02, // OKIM6258:	Set Clock Divider to 512
			};
			for(unsigned int i = 0; i < sizeof(pcm_cmds); i++) {
				w.buf.writeUint8(pcm_cmds[i]);
			}
			w.buf.writeUint8(0x92);
			w.buf.writeUint8(0x00);
			w.buf.writeLittleUint32(7813); // Set Stream Frequency

			w.buf.writeUint8(0x95);
			w.buf.writeUint8(0x00);
			w.buf.writeLittleUint16(uint16_t(n & 0xff));
			w.buf.writeUint8(0x00); // DAC Ctrl:	Stream #0 - Play: Block 0x00

		} else if(p->channel >= 8) return;
		w.writeYM2151(0x28 + p->channel, ((n / 12) << 4) | ((n % 12) * 16 / 12));
		w.writeYM2151(0x08, (voices[p->curVoice].slot_mask << 3) + (p->channel & 0x07)); // Key ON/OFF
	}
	virtual void handleNoteEnd(MDXSerialParser *p) {
		if(p->channel == 8) {
			uint8_t pcm_cmds[] = {
				0x94, 0x00, //      DAC Ctrl:	Stream #0 - Stop Stream
				0xB7, 0x01, 0x80, //   OKIM6258:	Data Write: 0x80
				0xB7, 0x02, 0x03, //   OKIM6258:	Pan Write: --
			};
			for(unsigned int i = 0; i < sizeof(pcm_cmds); i++) {
				w.buf.writeUint8(pcm_cmds[i]);
			}
		} else if(p->channel >= 8) return;
		w.writeYM2151(0x08, p->channel & 0x07);
	}
	virtual void handleRest(int r) {
		// divided 44100 and 4000000 by 100, to avoid int overflow
		int samples = r * 441 * 1024 * (256 - tempo) / 40000;
		w.writeWait(samples);
	}
	virtual void handleSetVolume(MDXSerialParser *p, uint8_t v) {
		if(p->channel >= 8) return;
		for(int i = 1; i < 4; i++) {
			uint8_t nvol = p->curVoiceData.osc[i].getTL() + (127 - p->volume);
			if(nvol > 127) nvol = 127;
			w.writeYM2151(0x60 + i * 8 + p->channel, nvol);
		}
	}
	virtual void handleDetune(MDXSerialParser *p, int16_t d) {
		if(p->channel >= 8) return;
		w.writeYM2151(0x30 + p->channel, (d & 0x3f) << 2);
	}
};

#endif /* MDXVGM_H_ */

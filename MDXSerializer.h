#ifndef MDXSERIALIZER_H_
#define MDXSERIALIZER_H_

#include "MDX.h"

class MDXSerializer;
class MDXSerialParser: public MDXMemParser {
friend class MDXSerializer;
public:
	MDXSerializer *mdx;
	int ticks, note;
	int curVoice;
	MDXVoice curVoiceData;
	uint8_t pan, volume;
	int16_t detune;
public:
	MDXSerialParser(): MDXMemParser(),
		ticks(0), curVoice(0), pan(3), volume(127), detune(0) {

	}

	void nextRest() {
		ticks = 0;
		while(ticks <= 0 && !ended) feed();
	}

private:
	virtual void handleSetTempo(uint8_t tempo);
	virtual void handleNote(uint8_t n, uint8_t duration);
	virtual void handleRest(uint8_t duration);
	virtual void handleSetVoiceNum(uint8_t voice);
	virtual void handleVolumeInc();
	virtual void handleVolumeDec();
	virtual void handleSetVolume(uint8_t v);
	virtual void handleDetune(int16_t d);
public:
};

class MDXSerializer: public MDX {
friend class MDXSerialParser;
	MDXSerialParser parsers[16];
	int loopCount;
public:
	int tempo;
	MDXVoice voices[256]; // FIXME
	MDXSerializer() {}
	MDXSerializer(const char *filename): loopCount(1), tempo(200) {
		load(filename);
	}
	void load(const char *filename) {
		s.open(filename);
		readHeader();
		readVoices();
		for(int i = 0; i < num_channels; i++) {
			parsers[i].mdx = this;
			parsers[i].channel = i;
			parsers[i].loopIterations = loopCount;
			parsers[i].dataLen = (i < num_channels - 1 ? mml_offset[i+1] : Voice_offset) - mml_offset[i];
			if(parsers[i].dataLen > 0) {
				parsers[i].data = new uint8_t[parsers[i].dataLen];
				s.seek(file_base + mml_offset[i]);
				s.read(parsers[i].data, parsers[i].dataLen);
			}
		}
		bool done = false;
		while(!done) {
			done = true;
			for(int i = 0; i < num_channels; i++) {
				if(parsers[i].ticks <= 0 && !parsers[i].ended)
					parsers[i].nextRest();
			}
			// At this point, parsers should have remaining ticks, unless they're ended
			int minTicks = 0x7fffffff;
			for(int i = 0; i < num_channels; i++) {
				if(parsers[i].ticks > 0) { // don't check if ended
					if(parsers[i].ticks < minTicks) minTicks = parsers[i].ticks;
				}
			}
			bool tickEnded = false;
			for(int i = 0; i < num_channels; i++) {
				if(parsers[i].ticks > 0) {
					parsers[i].ticks -= minTicks;
					if(parsers[i].ticks <= 0) {
						if(!tickEnded) handleRest(minTicks);
						tickEnded = true;
						handleNoteEnd(&parsers[i]);
					}
				}
			}
			// Check if we're done
			done = true;
			for(int i = 0; i < num_channels; i++) {
				if(!parsers[i].ended || parsers[i].ticks > 0) done = false;
			}
		}
	}
private:
	virtual void handleVoice(MDXVoice &v) {
		voices[v.number] = v;
	}
	virtual void handleRest(int r) {}
	virtual void handleNote(MDXSerialParser *p, int n) {}
	virtual void handleNoteEnd(MDXSerialParser *p) {}
	virtual void handleSetVoiceNum(MDXSerialParser *p, uint8_t t) {}
	virtual void handleVolumeInc(MDXSerialParser *p) {}
	virtual void handleVolumeDec(MDXSerialParser *p) {}
	virtual void handleSetVolume(MDXSerialParser *p, uint8_t v) {}
	virtual void handleDetune(MDXSerialParser *p, int16_t d) {}
};

void MDXSerialParser::handleSetTempo(uint8_t tempo) { mdx->tempo = tempo; }

void MDXSerialParser::handleNote(uint8_t n, uint8_t duration) {
	ticks = duration;
	note = n;
	mdx->handleNote(this, n + (detune >> 6));
}

void MDXSerialParser::handleRest(uint8_t duration) {
	ticks = duration;
	note = -1;
}

void MDXSerialParser::handleSetVoiceNum(uint8_t t) {
	curVoice = t;
	curVoiceData = mdx->voices[t];
	mdx->handleSetVoiceNum(this, t);
}

void MDXSerialParser::handleVolumeInc() { mdx->handleVolumeInc(this); }
void MDXSerialParser::handleVolumeDec() { mdx->handleVolumeDec(this); }
void MDXSerialParser::handleSetVolume(uint8_t v) { volume = v; mdx->handleSetVolume(this, v); }
void MDXSerialParser::handleDetune(int16_t d) { detune = d; mdx->handleDetune(this, d); }

#endif /* MDXSERIALIZER_H_ */

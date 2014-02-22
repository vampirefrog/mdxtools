#ifndef MDXSERIALIZER_H_
#define MDXSERIALIZER_H_

#include "MDX.h"

class MDXSerialParser;
template <class T=MDXSerialParser> class MDXSerializer;
class MDXSerialParser: public MDXChannelParser {
friend class MDXSerializer<MDXSerialParser>;
	uint8_t *data;
	int dataLen, dataPos;
	bool ended;
	MDXSerializer<MDXSerialParser> *mdx;
	int ticks, note;
	int loopIterations;
	int repeatStack[5];
	int repeatStackPos;

	MDXSerialParser(): data(0), dataLen(0), dataPos(0), ended(false), ticks(0), loopIterations(0), repeatStackPos(0) {}

	virtual void handleDataEnd() {
		ended = true;
	}
	virtual void handleDataEnd(int16_t ofs) {
		if(loopIterations-- > 0) dataPos -= ofs + 3;
		else ended = true;
	}
	virtual void handleRepeatStart(uint8_t iterations) {
		repeatStack[repeatStackPos++] = iterations - 1;
		printf("repeat start repeatStackPos=%d\n", repeatStackPos);
		if(repeatStackPos >= 5) repeatStackPos = 4;
	}
	virtual void handleRepeatEnd(int16_t ofs) {
		printf("repeat end ofs=%d repeatStackPos=%d\n", ofs, repeatStackPos);
		if(repeatStack[repeatStackPos-1]-- > 0) {
			dataPos += ofs;
			if(dataPos < 0) dataPos = 0;
		} else repeatStackPos--;
		if(repeatStackPos < 0) repeatStackPos = 0;
	}
	virtual void handleSetTempo(uint8_t tempo);
	virtual void handleNote(uint8_t n, uint8_t duration);
	virtual void handleRest(uint8_t duration);

	void feed() {
		eat(data[dataPos++]);
		if(dataPos >= dataLen) ended = true;
	}

	void nextRest() {
		ticks = 0;
		while(ticks <= 0 && !ended) feed();
	}
};

template <class T>
class MDXSerializer: public MDX {
	T parsers[16];
public:
	int tempo;
	MDXSerializer(const char *filename): tempo(200) {
		load(filename);
	}
	void load(const char *filename) {
		s.open(filename);
		readHeader();
		readVoices();
		for(int i = 0; i < num_channels; i++) {
			parsers[i].mdx = this;
			parsers[i].channel = i;
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
			for(int i = 0; i < num_channels; i++) {
				if(parsers[i].ticks > 0) {
					parsers[i].ticks -= minTicks;
					if(parsers[i].ticks <= 0) {
						printf("tick end on %d, minTicks=%d\n", i, minTicks);
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
};

void MDXSerialParser::handleSetTempo(uint8_t tempo) {
	printf("%d: setTempo %d\n", channel, tempo);
	mdx->tempo = tempo;
}

void MDXSerialParser::handleNote(uint8_t n, uint8_t duration) {
	ticks = duration;
	printf("%d: note %s, %d ticks=%d\n", channel, MDX::noteName(n), duration, ticks);
	note = n;
}

void MDXSerialParser::handleRest(uint8_t duration) {
	ticks = duration;
	printf("%d: rest %d ticks=%d\n", channel, duration, ticks);
	note = -1;
}

#endif /* MDXSERIALIZER_H_ */

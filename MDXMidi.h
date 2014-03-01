#ifndef MDXMIDI_H_
#define MDXMIDI_H_

#include "MDX.h"
#include "Stream.h"
#include "FS.h"

class MDXMidiChannelParser: public MDXMemParser {
public:
	Buffer dataMid;
	uint32_t deltaTime, restTime;
	uint8_t lastCmd;

	void writeDeltaTime(uint32_t t) {
		if(t > 0x1fffff) dataMid.writeUint8(0x80 | ((t >> 21) & 0x7f));
		if(t > 0x3fff) dataMid.writeUint8(0x80 | ((t >> 14) & 0x7f));
		if(t > 0x7f) dataMid.writeUint8(0x80 | ((t >> 7) & 0x7f));
		dataMid.writeUint8(t & 0x7f);
	}
	virtual void handleNote(uint8_t n, uint8_t duration) {
		writeDeltaTime(restTime);
		restTime = 0;
		uint8_t cmd = 0x90 | (channel & 0x0f);
		if(lastCmd != cmd) {
			lastCmd = cmd;
			dataMid.writeUint8(cmd);
		}
		dataMid.writeUint8(27 + n);
		dataMid.writeUint8(127);
		writeDeltaTime(duration);
		dataMid.writeUint8(27 + n);
		dataMid.writeUint8(0);
	}
	virtual void handleRest(uint8_t duration) {
		restTime += duration;
	}
	virtual void handleSetVoiceNum(uint8_t voice) {
		dataMid.writeUint8(0x00); // deltaT
		uint8_t cmd = 0xc0 | (channel & 0x0f);
		if(lastCmd != cmd) {
			lastCmd = cmd;
			dataMid.writeUint8(cmd); // Program change
		}
		dataMid.writeUint8(voice & 0x7f);
	}
	virtual void handleSetTempo(uint8_t tempo) {
		dataMid.writeUint8(0x00);
		uint8_t cmd = 0xff;
		if(lastCmd != cmd) {
			lastCmd = cmd;
			dataMid.writeUint8(cmd);
		}
		dataMid.writeUint8(0x51);
		dataMid.writeUint8(0x03);
		uint32_t mdxTempo = 1250000 / (256*(256-tempo));
		uint32_t msTempo = 60000000 / mdxTempo;
		dataMid.writeUint8((msTempo >> 16) & 0xff);
		dataMid.writeUint8((msTempo >> 8) & 0xff);
		dataMid.writeUint8((msTempo >> 0) & 0xff);
	}
	virtual void handleChannelEnd(int chan) {
		dataMid.writeUint8(0x00);
		dataMid.writeUint8(0xff);
		dataMid.writeUint8(0x2f);
		dataMid.writeUint8(0x00);
	}
};

class MDXMidi: public MDX {
	MDXMidiChannelParser parsers[16];
public:
	void open(const char *filename, const char *outfilename) {
		s.open(filename);
		readHeader();
		readVoices();
		int loopCount = 1;
		for(int i = 0; i < num_channels; i++) {
			parsers[i].channel = i;
			parsers[i].loopIterations = loopCount;
			parsers[i].dataLen = (i < num_channels - 1 ? mml_offset[i+1] : Voice_offset) - mml_offset[i];
			parsers[i].deltaTime = 0;
			parsers[i].restTime = 0;
			parsers[i].lastCmd = 0;
			if(parsers[i].dataLen > 0) {
				parsers[i].data = new uint8_t[parsers[i].dataLen];
				s.seek(file_base + mml_offset[i]);
				s.read(parsers[i].data, parsers[i].dataLen);
			}
		}

		FileWriteStream o(outfilename);
		int totalChans = 0;
		for(int i = 0; i < num_channels; i++) {
			while(!parsers[i].ended) parsers[i].feed();
			if(parsers[i].dataMid.len > 0) totalChans++;
		}
		o.write("MThd", 4);
		o.writeBigUint32(0x06);
		o.writeBigUint16(0x01);
		o.writeBigUint16(totalChans);
		o.writeBigUint16(48);
		for(int i = 0; i < num_channels; i++) {
			if(parsers[i].dataMid.len > 0) {
				parsers[i].dataMid.writeUint8(0x00);
				parsers[i].dataMid.writeUint8(0xff);
				parsers[i].dataMid.writeUint8(0x2f);
				parsers[i].dataMid.writeUint8(0x00);
				o.write("MTrk", 4);
				o.writeBigUint32(parsers[i].dataMid.len);
				o.write(parsers[i].dataMid);
			}
		}
	}
};

#endif /* MDXMIDI_H_ */

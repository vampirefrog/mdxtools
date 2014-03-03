#ifndef MDXMIDI_H_
#define MDXMIDI_H_

#include "MDX.h"
#include "Stream.h"
#include "FS.h"
#include "Midi.h"

class MDXMidiChannelParser: public MDXMemParser {
public:
	MidiWriteChannel mid;
	uint32_t deltaTime, restTime;
	uint8_t lastCmd;
	uint8_t volume, pan;

	MDXMidiChannelParser(): volume(127), pan(3) {}

	virtual void handleNote(uint8_t n, uint8_t duration) {
		int add = channel >= 8 ? 0 : 3;
		mid.writeNoteOn(restTime, channel, n + add, 100);
		mid.writeNoteOn(duration, channel, n + add, 0);
		restTime = 0;
	}
	virtual void handleRest(uint8_t duration) {
		restTime += duration;
	}
	virtual void handleSetVoiceNum(uint8_t voice) {
		mid.writeProgramChange(0, channel, voice);
	}
	virtual void handleSetTempo(uint8_t tempo) {
		uint32_t mdxTempo = 1250000 / (256*(256-tempo));
		uint32_t midiTempo = 60000000 / mdxTempo;
		mid.writeTempo(0, midiTempo);
	}
	virtual void handleChannelEnd(int chan) {
		mid.writeTrackEnd(0);
	}
	virtual void handleDetune(int16_t det) {
		mid.writePitchBend(0, channel, 0x2000 + det * 0xfff);
	}
	virtual void handleSetVolume(uint8_t vol) {
		volume = vol;
		mid.writeControlChange(0, channel, 7, pan ? MDX::volumeVal(vol) & 0x7f : 0);
	}
	virtual void handlePan(uint8_t p) {
		pan = p;
		if(pan == 0) {
			mid.writeControlChange(0, channel, 7, 0);
		}
		uint8_t pans[] = { 64, 127, 0, 64 };
		mid.writeControlChange(0, channel, 10, pans[pan & 0x03]);
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
			parsers[i].restTime = 192;
			parsers[i].lastCmd = 0;
			if(parsers[i].dataLen > 0) {
				parsers[i].data = new uint8_t[parsers[i].dataLen];
				s.seek(file_base + mml_offset[i]);
				s.read(parsers[i].data, parsers[i].dataLen);
			}
		}

		FileWriteStream o(outfilename);
		o.write("MThd", 4);
		o.writeBigUint32(0x06);
		o.writeBigUint16(0x01);
		o.writeBigUint16(num_channels);
		o.writeBigUint16(48);
		MidiWriteChannel chan;
		char buf[256];
		snprintf(buf, sizeof(buf), "Converted from %s", filename);
//		chan.writeText(0, buf);
		chan.writeTrackEnd(0);
		o.write("MTrk", 4);
		o.writeBigUint32(chan.len);
		o.write(chan);
		for(int i = 0; i < num_channels; i++) {
			char buf[256];
			snprintf(buf, sizeof(buf), "Track %d: Channel %c", i+1, channelName(i));
			parsers[i].mid.writeTrackName(0, buf);
			parsers[i].mid.writeRPN(0, i, 0, 32); // Pitch Bend Sensitivity
			parsers[i].mid.writeControlChange(0, i, 13, 0);
			parsers[i].mid.writeControlChange(0, i, 12, 0);
			parsers[i].mid.writePitchBend(0, i, 0x2000);
			while(!parsers[i].ended) parsers[i].feed();
			parsers[i].mid.writeTrackEnd(0);
			o.write("MTrk", 4);
			o.writeBigUint32(parsers[i].mid.len);
			o.write(parsers[i].mid);
		}
	}
};

#endif /* MDXMIDI_H_ */

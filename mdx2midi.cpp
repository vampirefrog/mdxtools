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
		mid.writeControlChange(0, channel, 7, pan ? volumeVal(vol) & 0x7f : 0);
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

class MDXMidi {
	MDXMidiChannelParser parsers[16];
public:
	void open(const char *filename, const char *outfilename) {
		FileReadStream s(filename);
		MDXHeader h;
		h.read(s);
		int loopCount = 1;
		for(int i = 0; i < h.numChannels; i++) {
			parsers[i].channel = i;
			parsers[i].loopIterations = loopCount;
			parsers[i].dataLen = h.channels[i].length;
			parsers[i].deltaTime = 0;
			parsers[i].restTime = 192;
			parsers[i].lastCmd = 0;
			if(parsers[i].dataLen > 0) {
				parsers[i].data = new uint8_t[parsers[i].dataLen];
				s.seek(h.fileBase + h.channels[i].offset);
				s.read(parsers[i].data, parsers[i].dataLen);
			}
		}

		FileWriteStream o(outfilename);
		o.write("MThd", 4);
		o.writeBigUint32(0x06);
		o.writeBigUint16(0x01);
		o.writeBigUint16(h.numChannels);
		o.writeBigUint16(48);
		MidiWriteChannel chan;
		char buf[256];
		snprintf(buf, sizeof(buf), "Converted from %s", filename);
		chan.writeText(0, buf);
		chan.writeTrackEnd(0);
		o.write("MTrk", 4);
		o.writeBigUint32(chan.len);
		o.write(chan);
		for(int i = 0; i < h.numChannels; i++) {
			char buf[256];
			snprintf(buf, sizeof(buf), "Track %d: Channel %c", i+1, MDXParser::channelName(i));
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

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		char buf[256];
		snprintf(buf, sizeof(buf), "%s.mid", argv[i]);
		MDXMidi m;
		m.open(argv[i], buf);
	}
	return 0;
}

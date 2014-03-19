#include "MDX.h"
#include "Stream.h"
#include "FS.h"
#include "Midi.h"

#include <exception>

class MDXMidiChannelParser: public MDXMemParser {
public:
	MidiWriteTrack mid;
	uint32_t deltaTime, restTime;
	uint8_t lastCmd;
	uint8_t volume, pan;
	uint8_t firstTempo;
	uint32_t totalTicks;

	MDXMidiChannelParser(): volume(127), pan(3), firstTempo(0), totalTicks(0) {}

	virtual void handleNote(uint8_t n, uint8_t duration) {
		int add = channel >= 8 ? 0 : 3;
		mid.writeNoteOn(restTime, channel, n + add, 100);
		mid.writeNoteOn(duration, channel, n + add, 0);
		restTime = 0;
		totalTicks += duration;
	}
	virtual void handleRest(uint8_t duration) {
		restTime += duration;
		totalTicks += duration;
	}
	virtual void handleSetVoiceNum(uint8_t voice) {
		mid.writeProgramChange(0, channel, voice);
	}
	static uint32_t calcTempo(uint32_t tempo) {
		uint32_t mdxTempo = 1250000 / (256*(256-tempo));
		return 60000000 / mdxTempo;
	}
	virtual void handleSetTempo(uint8_t tempo) {
		mid.writeTempo(0, calcTempo(tempo));
		if(firstTempo == 0) firstTempo = tempo;
	}
	virtual void handleChannelEnd(int chan) {
		mid.writeTrackEnd(0);
	}
	virtual void handleDetune(int16_t det) {
		mid.writePitchBend(0, channel, 0x2000 + det * 64);
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
		int midiTracks = 1;
		int firstTempo = 0;
		uint32_t totalTicks = 0;
		for(int i = 0; i < h.numChannels; i++) {
			parsers[i].channel = i;
			parsers[i].loopIterations = loopCount;
			parsers[i].dataLen = h.channels[i].length;
			parsers[i].deltaTime = 0;
			parsers[i].restTime = 0;
			parsers[i].lastCmd = 0;
			if(parsers[i].dataLen > 0) {
				parsers[i].data = new uint8_t[parsers[i].dataLen];
				s.seek(h.fileBase + h.channels[i].offset);
				s.read(parsers[i].data, parsers[i].dataLen);
				while(!parsers[i].ended) parsers[i].feed();
				if(parsers[i].mid.len > 0) {
					midiTracks++;
					if(parsers[i].firstTempo > 0 && firstTempo == 0)
						firstTempo = parsers[i].firstTempo;
					if(parsers[i].totalTicks > totalTicks) totalTicks = parsers[i].totalTicks;
				}
			}
		}

		MidiWriteStream o(outfilename);
		o.writeHeader(midiTracks, 48);
		MidiWriteTrack firstTrack;
		firstTrack.writeTempo(0, MDXMidiChannelParser::calcTempo(firstTempo ? firstTempo : 200));
		firstTrack.writeTimeSignature(0, 4, 2);
		char buf[256];
		snprintf(buf, sizeof(buf), "Converted from %s", filename);
		firstTrack.writeText(0, buf);
		firstTrack.writeTrackEnd(totalTicks);
		o.writeTrack(firstTrack);
		for(int i = 0; i < h.numChannels; i++) {
			if(parsers[i].mid.len == 0) continue;
			MidiWriteTrack track;
			char buf[256];
			snprintf(buf, sizeof(buf), "Channel %c (%s)", MDXParser::channelName(i), i > 8 ? "ADPCM" : "FM");
			track.writeTrackName(0, buf);
			track.writeBankSelect(0, i, 0);

			// VOPM specific sends
			track.writeRPN(0, i, 0, 32); // Pitch Bend Sensitivity
			track.writeControlChange(0, i, 6, 112); // set 4MHz clock
			track.writeControlChange(0, i, 12, 0); // Amplitude LFO level
			track.writeControlChange(0, i, 13, 0); // Pitch LFO level
			track.writeControlChange(0, i, 126, 127); // Monophonic mode
			track.writeControlChange(0, i, 5, 63);

			track.writePitchBend(0, i, 0x2000);


			track.write(parsers[i].mid);

			track.writeTrackEnd(0);
			o.writeTrack(track);
		}
	}
};

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		try {
			char buf[256];
			snprintf(buf, sizeof(buf), "%s.mid", argv[i]);
			MDXMidi m;
			m.open(argv[i], buf);
		} catch(exceptionf e) {
			fprintf(stderr, "Error: %s\n", e.what());
		}
	}
	return 0;
}

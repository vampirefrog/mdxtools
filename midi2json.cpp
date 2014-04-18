#include <stdio.h>

#include "Midi.h"

class MidiJSONStream: public MidiReadStream {
public:
	MidiJSONStream(const char *s) {
		load(s);
		printf("{\"format\":%d,\"ticks\":%d,\"tracks\":[", fileFormat, ticksPerQuarterNote);
	}
private:
	virtual void handleTrack(int number, int length) {
		printf("Track %d (%db)\n", number, length);
	}

	virtual void handleNoteOn(uint8_t channel, int duration, int note, int vel) {
		uint8_t octave;
		const char *noteN = noteName(note, &octave);
		printf("%08x %02u NoteOn %s%d (%d) %d\n", duration, channel, noteN, octave, note, vel);
	}

	virtual void handleNoteOff(uint8_t channel, int duration, int note, int vel) {
		uint8_t octave = 0;
		const char *noteN = noteName(note, &octave);
		printf("%08x %02u NoteOff %s%d (%d) %d\n", duration, channel, noteN, octave, note, vel);
	}

	virtual void handleKeyAfterTouch(uint8_t channel, int duration, int note, int vel) {
		printf("%08x %02u AfterTouch %d %d);\n", duration, channel, note, vel);
	}

	virtual void handleControlChange(uint8_t channel, int duration, int controller, int value) {
		printf("%08x %02u ControlChange controller=%d (%s) value=%d\n", duration, channel, controller, CCName(controller), value);
	}

	virtual void handleProgramChange(uint8_t channel, int duration, int program) {
		printf("%08x %02u ProgramChange program=%d;\n", duration, channel, program);
	}

	virtual void handleChannelAfterTouch(uint8_t channel, int duration, int value) {
		printf("%08x %02u AfterTouch value=%d\n", duration, channel, value);
	}

	virtual void handlePitchWheelChange(uint8_t channel, uint32_t duration, uint16_t value) {
		printf("%08x %02u PitchWheel value=%d\n", duration, channel, value);
	}

	virtual void handleMetaEvent(int duration, int cmd, int len, uint8_t *data) {
		printf("%08x MetaEvent 0x%02x len=%d ", duration, cmd, len);
		hexDump(data, len);
	}

	virtual void handleMetaSequenceNumber(int duration, uint16_t seq) {
		printf("%08x SequenceNumber %d\n", duration, seq);
	}

	virtual void handleTextEvent(int duration, const char *txt) {
		printf("%08x Text \"%s\"\n", duration, txt);
	}

	virtual void handleCopyrightInfoEvent(int duration, const char *txt) {
		printf("%08x Copyright \"%s\"\n", duration, txt);
	}

	virtual void handleTrackNameEvent(int duration, const char *txt) {
		printf("%08x TrackName \"%s\"\n", duration, txt);
	}

	virtual void handleInstrumentNameEvent(int duration, const char *txt) {
		printf("%08x InstrumentName \"%s\"\n", duration, txt);
	}

	virtual void handleLyricEvent(int duration, const char *txt) {
		printf("%08x Lyric \"%s\"\n", duration, txt);
	}

	virtual void handleMarkerTextEvent(int duration, const char *txt) {
		printf("%08x MarkerText \"%s\"\n", duration, txt);
	}

	virtual void handleProgramNameEvent(int duration, const char *txt) {
		printf("%08x ProgramName \"%s\"\n", duration, txt);
	}

	virtual void handleDeviceNameEvent(int duration, const char *txt) {
		printf("%08x DeviceName \"%s\"\n", duration, txt);
	}

	virtual void handleTempo(int duration, uint32_t tempo) {
		printf("%08x Tempo %d BPM (%d)\n", duration, tempo == 0 ? 0 : 60000000 / tempo, tempo);
	}

	virtual void handleTimeSignature(int duration, uint8_t numerator, uint8_t denominator, uint8_t ticksPerClick, uint8_t quarterNote32ndNotes) {
		printf("%08x TimeSignature %d %d %d %d\n", duration, numerator, denominator, ticksPerClick, quarterNote32ndNotes);
	}

	virtual void handleTrackEnd(int duration) {
		printf("%08x TrackEnd\n", duration);
	}

	virtual void handleRPN(uint8_t channel, int duration, uint16_t number, uint8_t msb) {
		printf("%08x %02d RPN %d (%s) = %d\n", channel, duration, number, RPNName(number), msb);
	}
	virtual void handleRPNLSB(uint8_t channel, int duration, uint16_t number, uint8_t lsb) {
		printf("%08x %02d RPN %d (%s) LSB = %d\n", channel, duration, number, RPNName(number), lsb);
	}
	virtual void handleNRPN(uint8_t channel, int duration, uint16_t number, uint8_t msb) {
		printf("%08x %02d NRPN %d = %d\n", channel, duration, number, msb);
	}
	virtual void handleNRPNLSB(uint8_t channel, int duration, uint16_t number, uint8_t lsb) {
		printf("%08x %02d NRPN %d LSB = %d\n", channel, duration, number, lsb);
	}
};


int main(int argc, char **argv) {
	try {
		MidiJSONStream f(argv[1]);
	} catch(exceptionf e) {
		fprintf(stderr, "Error: %s\n", e.what());
		return 1;
	}
	return 0;
}

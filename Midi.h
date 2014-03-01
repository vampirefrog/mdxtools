#ifndef MIDI_H_
#define MIDI_H_

#include "FS.h"
#include "exceptionf.h"
#include "tools.h"

/** MIDI file parser.
 *
 * Uses a state machine and callbacks  (virtual functions).
 */
class MidiReadStream {
private:
	FileReadStream s;
public:
	MidiReadStream(const char *filename) {
		load(filename);
	}
	MidiReadStream() {}
	~MidiReadStream() {}

	void load(const char *filename) {
		s.open(filename);
		if(!s.readCompare("MThd")) {
			throw exceptionf("%s not a MIDI file", filename);
		}
		int header_len = s.readBigUint32();
		printf("header_len=%d\n", header_len);
		int file_format = s.readBigUint16();
		int num_tracks = s.readBigUint16();
		int ticks = s.readBigUint16();
		printf("file_format=%s (%d) num_tracks=%d ticks=%d\n", fileFormatName(file_format), file_format, num_tracks, ticks);
		for(int i = 0; i < num_tracks; i++) {
			if(!s.readCompare("MTrk")) {
				throw exceptionf("%s: invalid track header at %p", filename, s.tell());
			}
			int track_length = s.readBigUint32();
			handleTrack(i, track_length);
			enum {
				None = 0,
				Duration,
				AfterDuration, // either a command or running mode
				AfterCommand,
				NoteOffN,
				NoteOffV,
				NoteOnN,
				NoteOnV,
				KeyAfterTouchN,
				KeyAfterTouchV,
				ControlChangeC,
				ControlChangeV,
				ProgramChangeP,
				ChannelAfterTouchC,
				PitchWheelChangeB,
				PitchWheelChangeT,
				MetaEventC,
				MetaEventL,
				MetaEventD
			} state = None;
			uint8_t nn, xx, metaLen, metaCount, *metaBuf, command;
			uint32_t duration = 0;
			uint8_t channel = 0;
			for(int j = track_length; j > 0 && !s.eof(); j--) {
				uint8_t b = s.readUint8();
//				printf("MIDI [%02x]\n", b);
				switch(state) {
					case None:
						duration = 0;
					case Duration:
						duration <<= 7;
						duration |= (b & 0x7f);
						if(b & 0x80) {
							state = Duration;
						} else {
							state = AfterDuration;
						}
						break;
					case AfterDuration:
						if(b & 0x80) {
							command = b;
							channel = b & 0x0f;
						} else {
							switch(command) {
								case 0xff:
									xx = b;
									state = MetaEventL;
									break;
								case 0xf8:
									// Timing clock
									break;
								case 0xfa:
									break;
								case 0xfb:
									break;
								case 0xfc:
									break;
								default:
									switch(command & 0xf0) {
										case 0x80:
											nn = b;
											state = NoteOffV;
											break;
										case 0x90:
											nn = b;
											state = NoteOnV;
											break;
										case 0xa0:
											nn = b;
											state = KeyAfterTouchV;
											break;
										case 0xb0:
											nn = b;
											state = ControlChangeV;
											break;
										case 0xc0:
											handleProgramChange(channel, duration, b);
											state = None;
											break;
										case 0xd0:
											handleChannelAfterTouch(channel, duration, b);
											state = None;
											break;
										case 0xe0:
											nn = b;
											state = PitchWheelChangeT;
											break;
										default:
											printf("Unknown command 0x%x\n", b);
											state = None;
											break;
									}
									break;
							}
						}
						break;
					case MetaEventL:
						metaLen = b;
						metaCount = 0;
						if(metaLen == 0) {
							handleMetaEvent(duration, xx, 0, 0);
							switch(xx) {
								case 0x2f:
									handleTrackEnd(duration);
									break;
							}
							state = None;
						} else {
							metaBuf = new uint8_t[metaLen + 1]; // add +1 for trailing zero
							state = MetaEventD;
						}
						break;
					case MetaEventD:
						metaBuf[metaCount++] = b;
						if(metaCount == metaLen) {
							state = None;
							metaBuf[metaLen] = 0;
							handleMetaEvent(duration, xx, metaLen, metaBuf);
							switch(xx) {
								case 0x00: // Track sequence number
									if(metaLen >= 2) {
										handleMetaSequenceNumber(duration, (metaBuf[0] << 8) | metaBuf[1]);
									}
									break;
								case 0x01: // Text event
									handleTextEvent(duration, (const char *)metaBuf);
									break;
								case 0x02: // Copyright info text
									handleCopyrightInfoEvent(duration, (const char *)metaBuf);
									break;
								case 0x03: // Track name text
									handleTrackNameEvent(duration, (const char *)metaBuf);
									break;
								case 0x04: // Instrument name text
									handleInstrumentNameEvent(duration, (const char *)metaBuf);
									break;
								case 0x05: // Lyric text
									handleLyricEvent(duration, (const char *)metaBuf);
									break;
								case 0x06: // Marker text
									handleMarkerTextEvent(duration, (const char *)metaBuf);
									break;
								case 0x51: // Set tempo
									if(metaLen >= 3)
										handleTempo(duration, (metaBuf[0] << 16) | (metaBuf[1] << 8) | metaBuf[2]);
									break;
								case 0x58: {
										uint8_t numerator = 0;
										uint8_t denominator = 0;
										uint8_t ticksPerClick = 0;
										uint8_t quarterNote32ndNotes = 0;
										if(metaLen >= 1) numerator = metaBuf[0];
										if(metaLen >= 2) denominator = metaBuf[1];
										if(metaLen >= 3) ticksPerClick = metaBuf[2];
										if(metaLen >= 4) quarterNote32ndNotes = metaBuf[3];
										handleTimeSignature(duration, numerator, denominator, ticksPerClick, quarterNote32ndNotes);
									}
									break;

							}
							delete[] metaBuf;
							metaBuf = 0;
						}
						break;
					case NoteOffV:
						handleNoteOff(channel, duration, nn, b);
						state = None;
						break;
					case NoteOnV:
						handleNoteOn(channel, duration, nn, b);
						state = None;
						break;
					case KeyAfterTouchV:
						handleKeyAfterTouch(channel, duration, nn, b);
						state = None;
						break;
					case ControlChangeV:
						handleControlChange(channel, duration, nn, b);
						state = None;
						break;
					case PitchWheelChangeT:
						handlePitchWheelChange(channel, duration, nn | (b << 8));
						state = None;
						break;
					default:
						state = None;
				}
			}
		}
	}

	const char *fileFormatName(uint16_t n) {
		const char *formatNames[] = {
			"single-track",
			"multiple tracks, synchronous",
			"multiple tracks, asynchronous"
		};
		if(n < 3) return formatNames[n];
		return "unknown";
	}

	const char *noteName(uint8_t n, uint8_t *octave) {
		const char *noteNames[] = {
			"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
		};
		if(octave) *octave = n / 12;
		return noteNames[n % 12];
	}

	virtual void handleTrack(int number, int length) {}
	virtual void handleNoteOn(int channel, int duration, int note, int vel) {}
	virtual void handleNoteOff(int channel, int duration, int note, int vel) {}
	virtual void handleKeyAfterTouch(int channel, int duration, int note, int vel) {}
	virtual void handleControlChange(int channel, int duration, int controller, int value) {}
	virtual void handleProgramChange(int channel, int duration, int program) {}
	virtual void handleChannelAfterTouch(int channel, int duration, int value) {}
	virtual void handlePitchWheelChange(int channel, int duration, uint16_t value) {}
	virtual void handleMetaEvent(int duration, int cmd, int len, uint8_t *data) {}
	virtual void handleMetaSequenceNumber(int duration, uint16_t seq) {}
	virtual void handleTextEvent(int duration, const char *txt) {}
	virtual void handleCopyrightInfoEvent(int duration, const char *txt) {}
	virtual void handleTrackNameEvent(int duration, const char *txt) {}
	virtual void handleInstrumentNameEvent(int duration, const char *txt) {}
	virtual void handleLyricEvent(int duration, const char *txt) {}
	virtual void handleMarkerTextEvent(int duration, const char *txt) {}
	virtual void handleTempo(int duration, uint32_t tempo) {}
	virtual void handleTimeSignature(int duration, uint8_t numerator, uint8_t denominator, uint8_t ticksPerClick, uint8_t quarterNote32ndNotes) {}
	virtual void handleTrackEnd(int duration) {}

	void beginSave() {
	}

	void addTrack();
	void addNoteOn();
};

class MidiDumpStream: public MidiReadStream {
public:
	MidiDumpStream(const char *s) { load(s); }
private:
	virtual void handleTrack(int number, int length) {
		printf("Track %d (%db)\n", number, length);
	}

	virtual void handleNoteOn(int channel, int duration, int note, int vel) {
		uint8_t octave;
		const char *noteN = noteName(note, &octave);
		printf("%08x %02u NoteOn %s%d (%d) %d\n", duration, channel, noteN, octave, note, vel);
	}

	virtual void handleNoteOff(int channel, int duration, int note, int vel) {
		uint8_t octave = 0;
		const char *noteN = noteName(note, &octave);
		printf("%08x %02u NoteOff %s%d (%d) %d\n", duration, channel, noteN, octave, note, vel);
	}

	virtual void handleKeyAfterTouch(int channel, int duration, int note, int vel) {
		printf("%08x %02u AfterTouch %d %d);\n", duration, channel, note, vel);
	}

	virtual void handleControlChange(int channel, int duration, int controller, int value) {
		printf("%08x %02u ControlChange controller=%d value=%d\n", duration, channel, controller, value);
	}

	virtual void handleProgramChange(int channel, int duration, int program) {
		printf("%08x %02u ProgramChange program=%d;\n", duration, channel, program);
	}

	virtual void handleChannelAfterTouch(int channel, int duration, int value) {
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

	virtual void handleTempo(int duration, uint32_t tempo) {
		printf("%08x Tempo %d BPM (%d)\n", duration, tempo == 0 ? 0 : 60000000 / tempo, tempo);
	}

	virtual void handleTimeSignature(int duration, uint8_t numerator, uint8_t denominator, uint8_t ticksPerClick, uint8_t quarterNote32ndNotes) {
		printf("%08x TimeSignature %d %d %d %d\n", duration, numerator, denominator, ticksPerClick, quarterNote32ndNotes);
	}

	virtual void handleTrackEnd(int duration) {
		printf("%08x TrackEnd\n", duration);
	}
};

#endif /* MIDI_H_ */

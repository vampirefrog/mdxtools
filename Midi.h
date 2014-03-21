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
	uint8_t rpn_msb, rpn_lsb;
	bool nrpn;
public:
	bool dumpBytes;
	MidiReadStream(const char *filename): rpn_msb(127), rpn_lsb(127), nrpn(false), dumpBytes(false) {
		load(filename);
	}
	MidiReadStream(): rpn_msb(127), rpn_lsb(127), nrpn(false), dumpBytes(false) {}
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
				if(dumpBytes) printf("MIDI [%02x]\n", b);
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
								case 0x08:
									handleProgramNameEvent(duration, (const char *)metaBuf);
									break;
								case 0x09:
									handleDeviceNameEvent(duration, (const char *)metaBuf);
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
						switch(nn) {
							case 98:
								rpn_lsb = b;
								nrpn = true;
								break;
							case 99:
								rpn_msb = b;
								nrpn = true;
								break;
							case 100:
								rpn_lsb = b;
								nrpn = false;
								break;
							case 101:
								rpn_msb = b;
								nrpn = false;
								break;
							case 6:
								if(rpn_lsb < 127 && rpn_msb < 127) {
									if(nrpn) handleNRPN(duration, channel, ((rpn_msb & 0x7f) << 7) | (rpn_lsb & 0x7f), b);
									else handleRPN(duration, channel, ((rpn_msb & 0x7f) << 7) | (rpn_lsb & 0x7f), b);
								}
								break;
							case 38:
								if(rpn_lsb < 127 && rpn_msb < 127) {
									if(nrpn) handleNRPNLSB(duration, channel, ((rpn_msb & 0x7f) << 7) | (rpn_lsb & 0x7f), b);
									else handleRPNLSB(duration, channel, ((rpn_msb & 0x7f) << 7) | (rpn_lsb & 0x7f), b);
								}
								break;
						}
						state = None;
						break;
					case PitchWheelChangeT:
						handlePitchWheelChange(channel, duration, nn | (b << 7));
						state = None;
						break;
					default:
						state = None;
				}
			}
		}
	}

	static const char *fileFormatName(uint16_t n) {
		const char *formatNames[] = {
			"single-track",
			"multiple tracks, synchronous",
			"multiple tracks, asynchronous"
		};
		if(n < 3) return formatNames[n];
		return "unknown";
	}

	static const char *noteName(uint8_t n, uint8_t *octave) {
		const char *noteNames[] = {
			"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
		};
		if(octave) *octave = n / 12;
		return noteNames[n % 12];
	}

	static const char *CCName(uint8_t cc) {
		const char *CCNames[] = {
			"Bank select",
			"Modulation",
			"Breath Controller",
			"Controller 3 (undefined)",
			"Foot Controller",
			"Portamento Time",
			"Data Entry MSB",
			"Channel Volume (formerly Main Volume)",
			"Balance",
			"Controller 9 (undefined)",
			"Pan",
			"Expression",
			"Effect Control 1",
			"Effect Control 2",
			"Controller 14 (undefined)",
			"Controller 15 (undefined)",
			"General Purpose 1",
			"General Purpose 2",
			"General Purpose 3",
			"General Purpose 4",
			"Controller 20 (undefined)",
			"Controller 21 (undefined)",
			"Controller 22 (undefined)",
			"Controller 23 (undefined)",
			"Controller 24 (undefined)",
			"Controller 25 (undefined)",
			"Controller 26 (undefined)",
			"Controller 27 (undefined)",
			"Controller 28 (undefined)",
			"Controller 29 (undefined)",
			"Controller 30 (undefined)",
			"Controller 31 (undefined)",
			"Bank select LSB",
			"Modulation LSB",
			"Breath Controller LSB",
			"Controller 35 (undefined)",
			"Foot Controller LSB",
			"Portamento Time LSB",
			"Data Entry LSB",
			"Channel Volume LSB (formerly Main Volume)",
			"Balance LSB",
			"Controller 41 (undefined)",
			"Pan LSB",
			"Expression LSB",
			"Effect Control 1 LSB",
			"Effect Control 2 LSB",
			"Controller 46 (undefined)",
			"Controller 47 (undefined)",
			"General Purpose 1 LSB",
			"General Purpose 2 LSB",
			"General Purpose 3 LSB",
			"General Purpose 4 LSB",
			"Controller 52 (undefined)",
			"Controller 53 (undefined)",
			"Controller 54 (undefined)",
			"Controller 55 (undefined)",
			"Controller 56 (undefined)",
			"Controller 57 (undefined)",
			"Controller 58 (undefined)",
			"Controller 59 (undefined)",
			"Controller 60 (undefined)",
			"Controller 61 (undefined)",
			"Controller 62 (undefined)",
			"Controller 63 (undefined)",
			"Damper Pedal (Sustain)",
			"Portamento On/Off",
			"Sostenuto On/Off",
			"Soft Pedal On/Off",
			"Legato Footswitch",
			"Hold 2",
			"Sound Controller 1 (Sound Variation)",
			"Sound Controller 2 (Resonance/Timbre)",
			"Sound Controller 3 (Release Time)",
			"Sound Controller 4 (Attack Time)",
			"Sound Controller 5 (Cut-off Frequency/Brightness)",
			"Sound Controller 6 (Decay Time)",
			"Sound Controller 7 (Vibrato Rate)",
			"Sound Controller 8 (Vibrato Depth)",
			"Sound Controller 9 (Vibrato Delay)",
			"Sound Controller 10 (undefined)",
			"General Purpose 5",
			"General Purpose 6",
			"General Purpose 7",
			"General Purpose 7",
			"Portamento Control",
			"Controller 85 (undefined)",
			"Controller 86 (undefined)",
			"Controller 87 (undefined)",
			"Controller 88 (undefined)",
			"Controller 89 (undefined)",
			"Controller 90 (undefined)",
			"Effects 1 Depth (Reverb)",
			"Effects 2 Depth (Tremolo)",
			"Effects 3 Depth (Chorus)",
			"Effects 4 Depth (Celeste/Detune)",
			"Effects 5 Depth (Phaser)",
			"Data Entry +1 (Increment)",
			"Data Entry -1(Decrement)",
			"NRPN LSB",
			"NRPN MSB",
			"RPN LSB",
			"RPN MSB",
			"Controller 102 (undefined)",
			"Controller 103 (undefined)",
			"Controller 104 (undefined)",
			"Controller 105 (undefined)",
			"Controller 106 (undefined)",
			"Controller 107 (undefined)",
			"Controller 108 (undefined)",
			"Controller 109 (undefined)",
			"Controller 110 (undefined)",
			"Controller 111 (undefined)",
			"Controller 112 (undefined)",
			"Controller 113 (undefined)",
			"Controller 114 (undefined)",
			"Controller 115 (undefined)",
			"Controller 116 (undefined)",
			"Controller 117 (undefined)",
			"Controller 118 (undefined)",
			"Controller 119 (undefined)",
			"All Sound Off",
			"Reset All Controllers",
			"Local Control On/Off",
			"All Notes Off",
			"Omni Mode Off",
			"Omni Mode On",
			"Poly Mode Off/Mono Mode On",
			"Poly Mode On/Mono Mode Off",
		};
		return CCNames[cc & 0x7f];
	}

	static const char *RPNName(uint16_t rpn) {
		const char *names[] = {
			"Pitch Bend Sensitivity",
			"Fine Tuning",
			"Coarse Tuning",
			"Tuning Program Select",
			"Tuning Bank Select",
		};
		if(rpn < 5) return names[rpn];
		return "Unknown";
	}

	virtual void handleTrack(int number, int length) {}
	virtual void handleNoteOn(uint8_t channel, int duration, int note, int vel) {}
	virtual void handleNoteOff(uint8_t channel, int duration, int note, int vel) {}
	virtual void handleKeyAfterTouch(uint8_t channel, int duration, int note, int vel) {}
	virtual void handleControlChange(uint8_t channel, int duration, int controller, int value) {}
	virtual void handleProgramChange(uint8_t channel, int duration, int program) {}
	virtual void handleChannelAfterTouch(uint8_t channel, int duration, int value) {}
	virtual void handlePitchWheelChange(uint8_t channel, uint32_t duration, uint16_t value) {}
	virtual void handleMetaEvent(int duration, int cmd, int len, uint8_t *data) {}
	virtual void handleMetaSequenceNumber(int duration, uint16_t seq) {}
	virtual void handleTextEvent(int duration, const char *txt) {}
	virtual void handleCopyrightInfoEvent(int duration, const char *txt) {}
	virtual void handleTrackNameEvent(int duration, const char *txt) {}
	virtual void handleInstrumentNameEvent(int duration, const char *txt) {}
	virtual void handleLyricEvent(int duration, const char *txt) {}
	virtual void handleMarkerTextEvent(int duration, const char *txt) {}
	virtual void handleProgramNameEvent(int duration, const char  *txt) {}
	virtual void handleDeviceNameEvent(int duration, const char  *txt) {}
	virtual void handleTempo(int duration, uint32_t tempo) {}
	virtual void handleTimeSignature(int duration, uint8_t numerator, uint8_t denominator, uint8_t ticksPerClick, uint8_t quarterNote32ndNotes) {}
	virtual void handleTrackEnd(int duration) {}
	virtual void handleRPN(uint8_t channel, int duration, uint16_t number, uint8_t msb) {}
	virtual void handleRPNLSB(uint8_t channel, int duration, uint16_t number, uint8_t lsb) {}
	virtual void handleNRPN(uint8_t channel, int duration, uint16_t number, uint8_t msb) {}
	virtual void handleNRPNLSB(uint8_t channel, int duration, uint16_t number, uint8_t lsb) {}

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

class MidiWriteTrack: public Buffer {
	uint8_t lastCmd;
public:
	MidiWriteTrack(): lastCmd(0) {}
	void writeDeltaTime(uint32_t t) {
		if(t > 0x1fffff) writeUint8(0x80 | ((t >> 21) & 0x7f));
		if(t > 0x3fff) writeUint8(0x80 | ((t >> 14) & 0x7f));
		if(t > 0x7f) writeUint8(0x80 | ((t >> 7) & 0x7f));
		writeUint8(t & 0x7f);
	}
	void writeCmd(uint8_t cmd, bool noReuse = false) {
		if(lastCmd != cmd || noReuse == true) {
			lastCmd = cmd;
			writeUint8(cmd);
		}
	}
	void writeNoteOn(uint32_t deltaTime, uint8_t channel, uint8_t note, uint8_t velocity) {
		writeDeltaTime(deltaTime);
		writeCmd(0x90 | (channel & 0x0f));
		writeUint8(note & 0x7f);
		writeUint8(velocity & 0x7f);
	}
	void writeTempo(uint32_t deltaTime, uint32_t tempo) {
		writeDeltaTime(deltaTime);
		writeCmd(0xff);
		writeUint8(0x51);
		writeUint8(0x03);
		writeUint8((tempo >> 16) & 0xff);
		writeUint8((tempo >> 8) & 0xff);
		writeUint8((tempo >> 0) & 0xff);
	}
	void writeMetaEvent(uint32_t deltaTime, uint8_t ev, uint8_t len, uint8_t *buf) {
		writeDeltaTime(deltaTime);
		writeCmd(0xff, true);
		writeUint8(ev);
		writeUint8(len);
		write(buf, len);
	}
	void writeMetaEvent(uint32_t deltaTime, uint8_t ev, uint8_t len, ...) {
		writeDeltaTime(deltaTime);
		writeCmd(0xff, true);
		writeUint8(ev);
		writeUint8(len);
		if(len > 0) {
			va_list ap;
			va_start(ap, len);
			for(int i = 0; i < len; i++) writeUint8(va_arg(ap, unsigned int));
			va_end(ap);
		}
	}
	void writeTimeSignature(uint32_t deltaTime, uint8_t numerator, uint8_t denominator, uint8_t ticksPerClick = 0, uint8_t quarterNote32ndNotes = 0) {
		if(quarterNote32ndNotes > 0 || ticksPerClick > 0)
			writeMetaEvent(deltaTime, 0x58, 4, numerator, denominator, ticksPerClick, quarterNote32ndNotes);
		else
			writeMetaEvent(deltaTime, 0x58, 2, numerator, denominator);
	}
	void writeTrackEnd(uint32_t deltaTime) {
		writeMetaEvent(deltaTime, 0x2f, 0);
	}
	void writeProgramChange(uint32_t deltaTime, uint8_t channel, uint8_t program) {
		writeDeltaTime(deltaTime);
		uint8_t cmd = 0xc0 | (channel & 0x0f);
		writeCmd(cmd);
		writeUint8(program & 0x7f);
	}
	void writeTextEvent(uint8_t event, uint32_t deltaTime, const char *text, int len = -1) {
		if(len < 0) len = strlen(text);
		if(len > 127) len = 127;
		writeMetaEvent(deltaTime, event, len, (uint8_t *)text);
	}
	void writeText(uint32_t deltaTime, const char *text, int len = -1) {
		writeTextEvent(0x01, deltaTime, text, len);
	}
	void writeTrackName(uint32_t deltaTime, const char *text, int len = -1) {
		writeTextEvent(0x03, deltaTime, text, len);
	}
	void writePitchBend(uint32_t deltaTime, uint8_t channel, uint16_t bend) {
		writeDeltaTime(deltaTime);
		writeCmd(0xe0 | (channel & 0x0f));
		writeUint8(bend & 0x7f);
		writeUint8((bend >> 7) & 0x7f);
	}
	void writeControlChange(uint32_t deltaTime, uint8_t channel, uint8_t controller, uint8_t value) {
		writeDeltaTime(deltaTime);
		writeCmd(0xb0 | (channel & 0x0f), true);
		writeUint8(controller & 0x7f);
		writeUint8(value & 0x7f);
	}
	void writeNRPN(uint32_t deltaTime, uint8_t channel, uint16_t number, uint8_t value_msb) {
		writeControlChange(deltaTime, channel, 99, (number >> 7) & 0x7f);
		writeControlChange(0, channel, 98, number & 0x7f);
		writeControlChange(0, channel, 6, value_msb & 0x7f);
	}
	void writeNRPN(uint32_t deltaTime, uint8_t channel, uint16_t number, uint8_t value_msb, uint8_t value_lsb) {
		writeControlChange(deltaTime, channel, 99, (number >> 7) & 0x7f);
		writeControlChange(0, channel, 98, number & 0x7f);
		writeControlChange(0, channel, 6, value_msb & 0x7f);
		writeControlChange(0, channel, 38, value_lsb & 0x7f);
	}
	void writeRPN(uint32_t deltaTime, uint8_t channel, uint16_t number, uint8_t value_msb) {
		writeControlChange(deltaTime, channel, 101, (number >> 7) & 0x7f);
		writeControlChange(0, channel, 100, number & 0x7f);
		writeControlChange(0, channel, 6, value_msb & 0x7f);
	}
	void writeRPN(uint32_t deltaTime, uint8_t channel, uint16_t number, uint8_t value_msb, uint8_t value_lsb) {
		writeControlChange(deltaTime, channel, 101, (number >> 7) & 0x7f);
		writeControlChange(0, channel, 100, number & 0x7f);
		writeControlChange(0, channel, 6, value_msb & 0x7f);
		writeControlChange(0, channel, 38, value_lsb & 0x7f);
	}
	void writeBankSelect(uint32_t deltaTime, uint8_t channel, uint16_t bank) {
		writeControlChange(deltaTime, channel, 0, (bank >> 7) & 0x7f);
		writeControlChange(0, channel, 32, bank & 0x7f);
	}
};

class MidiWriteStream: public FileWriteStream {
public:
	MidiWriteStream(const char *filename): FileWriteStream(filename) {}
	void writeHeader(int numTracks, int ticksPerQuarterNote = 960, int fileType = 1) {
		write("MThd", 4);
		writeBigUint32(0x06);
		writeBigUint16(fileType);
		writeBigUint16(numTracks);
		writeBigUint16(ticksPerQuarterNote);
	}
	void writeTrack(MidiWriteTrack &t) {
		write("MTrk", 4);
		writeBigUint32(t.len);
		write(t);
	}
};

#endif /* MIDI_H_ */

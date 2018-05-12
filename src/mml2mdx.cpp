#include "MDX.h"
#include "MML.h"
#include "Stream.h"
#include "FS.h"
#include "tools.h"

#include <exception>

class MMLCompiler: public MMLParser {
	Buffer chanBufs[16]; // A-H, P-W = 16 channeru
	Buffer voiceDataBytes;
	int numVoices, numChannels;
	char *title, *pcmFile;
	bool pcm8;
	void processChannelMask(uint32_t channelMask) {
		if(channelMask > 0x1ff) {
			pcm8 = true;
			numChannels = 16;
		}
	}

	void writeData(uint32_t channelMask, uint8_t *data, int len) {
		processChannelMask(channelMask);
		int i, m;
		for(i = 0, m = 1; i < numChannels; i++, m <<= 1) {
			if(channelMask & m) {
				chanBufs[i].write(data, len);
			}
		}
	}

	int curOctave, curNoteLength, curTempo;
public:
	void handleDirective(char *directive, char *value) {
		if(!strcmp(directive, "title")) title = strdup(value);
		else if(!strcmp(directive, "pcmfile")) pcmFile = strdup(value);
		else fprintf(stderr, "Warning: Unknown directive: \"%s\"\n", directive);
	}

	void handleVoice(int voiceNum, int *voiceData) {
		numVoices++;
		voiceDataBytes.writeUint8(voiceNum);
		voiceDataBytes.writeUint8(((voiceData[45] & 0x07) << 3) | (voiceData[44] & 0x07)); // FL & CON
		voiceDataBytes.writeUint8(voiceData[46]); // Slot mask
		for(int i = 0; i < 44; i+=11) voiceDataBytes.writeUint8(((voiceData[8 + i] & 0x07) << 4) | (voiceData[7 + i] & 0x0f)); // DT1 & MUL;
		for(int i = 0; i < 44; i+=11) voiceDataBytes.writeUint8(voiceData[5 + i] & 0x7f); // TL;
		for(int i = 0; i < 44; i+=11) voiceDataBytes.writeUint8((voiceData[6 + i] << 6) | (voiceData[0 + i] & 0x1f)); // KS & AR;
		for(int i = 0; i < 44; i+=11) voiceDataBytes.writeUint8((voiceData[10 + i] << 7) | (voiceData[1 + i] & 0x1f)); // AME & D1R
		for(int i = 0; i < 44; i+=11) voiceDataBytes.writeUint8((voiceData[9 + i] << 6) | ((voiceData[2 + i] >> 5) & 0x1f)); // DT2 & D2R
		for(int i = 0; i < 44; i+=11) voiceDataBytes.writeUint8((voiceData[4 + i] << 4) | (voiceData[3 + i] & 0x0f)); // D1L & RR
	}

	void handleVolume(uint32_t channelMask, int volume) {
		processChannelMask(channelMask);
		// printf("Volume %d\n", volume);
	}
	void handleOPMTempo(uint32_t channelMask, int tempo) {
		processChannelMask(channelMask);
		uint8_t buf[2];
		curTempo = tempo;
		buf[0] = 0xff;
		buf[1] = tempo;
		writeData(channelMask, buf, 2);
	}
	void handleTempo(uint32_t channelMask, int tempo) {
		processChannelMask(channelMask);
		uint8_t buf[2];
		curTempo = 256 - (78125/(16*tempo));
		buf[0] = 0xff;
		buf[1] = curTempo;
		writeData(channelMask, buf, 2);
	}
	void handleSetVoice(uint32_t channelMask, int voiceNum) {
		processChannelMask(channelMask);
		uint8_t buf[2];
		buf[0] = 0xfd;
		buf[1] = voiceNum;
		writeData(channelMask, buf, 2);
	}
	void handleDetune(uint32_t channelMask, int detune) {
		processChannelMask(channelMask);
		// printf("Detune %d\n", detune);
	}
	void handlePan(uint32_t channelMask, int pan) {
		processChannelMask(channelMask);
		// printf("Pan %d\n", pan);
	}
	void handleOctave(uint32_t channelMask, int octave) {
		processChannelMask(channelMask);
		curOctave = octave;
		// printf("Octave %d\n", octave);
	}
	void handleOctaveDown(uint32_t channelMask) {
		processChannelMask(channelMask);
		curOctave--;
		// printf("OctaveDown\n");
	}
	void handleOctaveUp(uint32_t channelMask) {
		processChannelMask(channelMask);
		curOctave++;
		// printf("OctaveUp\n");
	}
	void handleLegato(uint32_t channelMask) {
		processChannelMask(channelMask);
		// printf("Legato\n");
	}
	void handleNote(uint32_t channelMask, int note, int noteLength, uint32_t noteFlags) {
		// printf("%02x Note %c length=%d%s%s%s%s\n", channelMask, note, noteLength, noteFlags & NOTE_FLAG_SHARP ? " sharp" : "", noteFlags & NOTE_FLAG_FLAT ? " flat" : "", noteFlags & NOTE_FLAG_DOTTED ? " dotted" : "", noteFlags & NOTE_FLAG_OPM_LENGTH ? " OPMLength" : "");
		uint8_t buf[2];
		buf[0] = 0x80 + note - 'g' + curOctave * 12;
		buf[1] = (noteLength ? noteLength : curNoteLength) - 1;
		writeData(channelMask, buf, 2);
	}
	void handleRest(uint32_t channelMask, int noteLength, uint32_t noteFlags) {
		processChannelMask(channelMask);
		uint8_t buf[1];
		buf[0] = 0x00;
		writeData(channelMask, buf, 1);
		//printf("Rest length=%d%s%s\n", noteLength, noteFlags & NOTE_FLAG_DOTTED ? " dotted" : "", noteFlags & NOTE_FLAG_OPM_LENGTH ? " OPMLength" : "");
	}

	void handleError(const char *file, int line, const char *fmt, ...) {
		va_list ap;
		va_start(ap, fmt);
		fprintf(stderr, "MML Parser error(%s:%d): %d:%d:", file, line, tok.lineNum, tok.charNum);
		vfprintf(stderr, fmt, ap);
		fprintf(stderr, " state=%s\n", getStateName());
		va_end(ap);
		exit(1);
	}

	void open(const char *filename, const char *outfilename) {
		title = pcmFile = 0;
		pcm8 = false;
		numVoices = 0;
		numChannels = 9;

		curOctave = 4;
		curNoteLength = 4;
		curTempo = 200;

		FileReadStream s(filename);

		while(1) {
			uint8_t b = s.readUint8();
			if(s.eof()) break;
			eat(b);
		}
		eat('\n');

		FileWriteStream o(outfilename);
		int titleLen = title ? strlen(title) : 0, pcmFileLen = pcmFile ? strlen(pcmFile) : 0;

		// title
		if(title && title[0])
			o.write(title, titleLen);
		o.writeUint8(0x0d);
		o.writeUint8(0x0a);
		o.writeUint8(0x1a);

		// PCM file
		if(pcmFile && pcmFile[0])
			o.write(pcmFile, pcmFileLen);
		o.writeUint8(0x00);

		// Voice data offset
		int voiceDataOffset = numChannels * 2 + 2;
		for(int i = 0; i < numChannels; i++) {
			voiceDataOffset += chanBufs[i].len;
		}
		o.writeBigUint16(voiceDataOffset);

		// MML data offset
		int mmlDataStart = numChannels * 2 + 2;
		for(int i = 0; i < numChannels; i++) {
			o.writeBigUint16(mmlDataStart);
			mmlDataStart += chanBufs[i].len;
		}

		// MML data
		for(int i = 0; i < numChannels; i++) {
			o.write(chanBufs[i]);
		}

		// Voice data
		o.write(voiceDataBytes);
	}
};

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		try {
			MMLCompiler c;
			c.open(argv[i], replaceExtension(argv[i], "mdx"));
		} catch(exceptionf e) {
			fprintf(stderr, "%s:%s\n", argv[i], e.what());
		}
	}
	return 0;
}

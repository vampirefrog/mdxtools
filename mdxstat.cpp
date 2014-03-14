#include "MDX.h"

class MDXStatParser: public MDXParser {
public:
	int cmds[32];
	int notes;
	int rests;
	int duration;
	MDXStatParser(): notes(0), rests(0), duration(0) {
		memset(cmds, 0, 32 * sizeof(int));
	}

	virtual void handleRest(uint8_t d) {
		rests++;
		duration += d;
	}
	virtual void handleNote(uint8_t note, uint8_t d) {
		notes++;
		duration += d;
	}
	virtual void handleCommand(uint8_t c, ...) {
		cmds[c - 0xe0]++;
	}

	void add(MDXStatParser &p) {
		for(int i = 0; i < 32; i++) {
			cmds[i] += p.cmds[i];
		}
		notes += p.notes;
		rests += p.rests;
		duration += p.duration;
	}

	void output() {
		printf("notes rests duration ");
		for(int i = 0; i < 32; i++) {
			printf("%s%s%02x ", cmds[i] > 0xff ? " " : "", cmds[i] > 0xfff ? " " : "", i + 0xe0);
		}
		printf("\n");
		printf(" %04x  %04x   %06x ", notes, rests, duration);
		for(int i = 0; i < 32; i++) {
			printf("%02x ", cmds[i]);
		}
		printf("\n");
	}
};

int main(int argc, char **argv) {
	MDXStatParser total;
	for(int i = 1; i < argc; i++) {
		try {
			FileReadStream s(argv[i]);
			MDXHeader h;
			h.read(s);
			MDXStatParser fileTotal;
			for(int i = 0; i < h.numChannels; i++) {
				MDXStatParser p;
				s.seek(h.fileBase + h.channels[i].offset);
				for(size_t j = 0; !s.eof() && j < h.channels[i].length; j++)
					p.eat(s.readUint8());
				fileTotal.add(p);
			}
			total.add(fileTotal);
		} catch(exceptionf e) {
			fprintf(stderr, "Error: %s\n", e.what());
		}
	}
	total.output();
}

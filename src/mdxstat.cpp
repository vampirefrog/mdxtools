#include "MDX.h"

class MDXStatParser: public MDXParser {
public:
	int cmds[32];
	int extCmds[256];
	int notes;
	int rests;
	int duration;
	MDXStatParser(): notes(0), rests(0), duration(0) {
		memset(cmds, 0, sizeof(cmds));
		memset(extCmds, 0, sizeof(extCmds));
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
		va_list ap;
		va_start(ap, c);
		if(c == 0xe7) {
			uint8_t e = va_arg(ap, int);
			extCmds[e]++;
		}
		va_end(ap);
	}

	void add(MDXStatParser &p) {
		for(int i = 0; i < 32; i++) {
			cmds[i] += p.cmds[i];
		}
		notes += p.notes;
		rests += p.rests;
		duration += p.duration;
		for(int i = 0; i < 255; i++) {
			extCmds[i] += p.extCmds[i];
		}
	}

	void output() {
		if(0) {
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
		} else {
			printf("notes %d  rests %d  duration %d\n", notes, rests, duration);
			int x = 0;
			for(int i = 0; i < 32; i++) {
				const char *c = commandName(i + 0xe0);
				printf("%02x %s:", i + 0xe0, c);
				char buf[30];
				snprintf(buf, sizeof(buf), "%d", cmds[i]);
				int bl = strlen(buf);
				for(int j = strlen(c); j < 20 - bl; j++) { putchar(' '); }
				printf("%d", cmds[i]);
				x++;
				if(x >= 4) { printf("\n"); x = 0; }
				else if(i < 31) printf(" | ");
			}
			for(int i = 0; i < 255; i++) {
				if(extCmds[i]) printf("Extended %02x: %d\n", i, extCmds[i]);
			}
			printf("\n");
		}
	}
};

int main(int argc, char **argv) {
	MDXStatParser total;
	for(int i = 1; i < argc; i++) {
		try {
			printf("%s\n", argv[i]);
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
			fileTotal.output();
			total.add(fileTotal);
		} catch(exceptionf e) {
			fprintf(stderr, "Error: %s\n", e.what());
		}
	}
	total.output();
}

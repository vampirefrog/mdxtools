#include "MDX.h"

class MDXStatParser: public MDXChannelParser {
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
};

class MDXStat: public MDX {
public:
	int total_cmds[32];
	int total_notes;
	int total_rests;
	int total_duration;
	MDXStat() {
		memset(total_cmds, 0, 32 * sizeof(int));
		total_notes = total_rests = total_duration = 0;
	}
	void load(const char *filename) {
		MDXStatParser p;
		MDX::load(filename, p);
		total_notes += p.notes;
		total_rests += p.rests;
		total_duration += p.duration;
		for(int i = 0; i < 32; i++) {
			total_cmds[i] += p.cmds[i];
		}
	}
	void outputStats(bool total = false) {
		int *c = total_cmds;
		printf("notes rests duration ");
		for(int i = 0; i < 32; i++) {
			printf("%s%s%02x ", c[i] > 0xff ? " " : "", c[i] > 0xfff ? " " : "", i + 0xe0);
		}
		printf("\n");
		printf(" %04x  %04x   %06x ", total_notes, total_rests, total_duration);
		for(int i = 0; i < 32; i++) {
			printf("%02x ", c[i]);
		}
		printf("\n");
		//printf("%s (%x): %d\n", commandName(i + 0xe0), i + 0xe0,  c[i]);
	}
};

int main(int argc, char **argv) {
	MDXStat mdx;
	for(int i = 1; i < argc; i++) {
		try {
			mdx.load(argv[i]);
			printf("%s:\n", argv[i]);
			mdx.outputStats();
		} catch(exceptionf e) {
			fprintf(stderr, "Error: %s\n", e.what());
		}
	}
	printf("TOTAL:\n");
	mdx.outputStats(true);
}

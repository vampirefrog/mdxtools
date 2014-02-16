#include "MDX.h"

class MDXStat: public MDX {
public:
	int cmds[32];
	int total_cmds[32];
	int notes, total_notes;
	int rests, total_rests;
	int duration, total_duration;
	MDXStat() {
		memset(total_cmds, 0, 32 * sizeof(int));
		total_notes = total_rests = total_duration = 0;
	}
	void load(const char *filename) {
		memset(cmds, 0, 32 * sizeof(int));
		notes = rests = duration = 0;
		MDX::load(filename);
	}
	virtual void handleRest(uint8_t d) {
		rests++; total_rests++;
		total_duration += d;
		duration += d;
	}
	virtual void handleNote(uint8_t note, uint8_t d) {
		notes++; total_notes++;
		total_duration += d;
		duration += d;
	}
	virtual void handleCommand(uint8_t c, ...) {
		cmds[c - 0xe0]++;
		total_cmds[c - 0xe0]++;
	}
	void outputStats(bool total = false) {
		int *c = total ? total_cmds: cmds;
		printf("notes rests duration ");
		for(int i = 0; i < 32; i++) {
			printf("%s%s%02x ", c[i] > 0xff ? " " : "", c[i] > 0xfff ? " " : "", i + 0xe0);
		}
		printf("\n");
		printf(" %04x  %04x   %06x ", total ? total_notes : notes, total ? total_rests : rests, total ? total_duration : duration);
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
		mdx.load(argv[i]);
		printf("%s:\n", argv[i]);
		mdx.outputStats();
	}
	printf("TOTAL:\n");
	mdx.outputStats(true);
}

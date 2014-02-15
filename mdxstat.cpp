#include "MDX.h"

class MDXStat: public MDX {
public:
	int cmds[32];
	int total_cmds[32];
	MDXStat() {
		memset(total_cmds, 0, 32 * sizeof(int));
	}
	void load(const char *filename) {
		memset(cmds, 0, 32 * sizeof(int));
		MDX::load(filename);
	}
	virtual void handleRest(uint8_t duration) {
		
	}
	virtual void handleNote(uint8_t note, uint8_t duration) {
	}
	virtual void handleCommand(uint8_t c, ...) {
		cmds[c - 0xe0]++;
		total_cmds[c - 0xe0]++;
	}
	void outputStats(bool total = false) {
		int *c = total ? total_cmds: cmds;
		for(int i = 0; i < 32; i++) {
			printf("%s%s%02x ", c[i] > 0xff ? " " : "", c[i] > 0xfff ? " " : "", i + 0xe0);
		}
		printf("\n");
		for(int i = 0; i < 32; i++) {
			printf("%02x ", c[i]);
		}
		printf("\n");
		//			printf("%s (%x): %d\n", commandName(i + 0xe0), i + 0xe0,  c[i]);

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

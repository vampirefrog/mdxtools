#include "MDX.h"

class MDXOPM: public MDX {
public:
	MDXOPM(const char *filename) {
		load(filename);
	}
	void load(const char *filename) {
		s.open(filename);
		readHeader();
		printf("// Converted from \"%s\" using mdx2opm\n", filename);
		printf("// https://github.com/vampirefrog/mdxtools\n");
		printf("// LFO: LFRQ AMD PMD WF NFRQ\n");
		printf("// @:[Num] [Name]\n");
		printf("// CH: PAN   FL CON AMS PMS SLOT NE\n");
		printf("// [OPname]: AR D1R D2R  RR D1L  TL  KS MUL DT1 DT2 AMS-EN\n");
		readVoices();
		// And we're done
	}

private:
	virtual void handleHeader() {
	}
	virtual void handleVoice(MDXVoice &v) {
		printf("@:%d Instrument %d\n", v.number, v.number);
		printf("LFO: 0 0 0 0 0\n");
		printf("CH: %d %d 4 0 0 %d 0\n", v.fl, v.con, v.slot_mask);
		for(int i = 0; i < 4; i++) {
			printf("%s: %d %d %d %d %d %d %d %d %d %d %d\n", MDX::voiceName(i), v.osc[i].ar, v.osc[i].d1r, v.osc[i].d2r, v.osc[i].rr, v.osc[i].d1l, v.osc[i].tl, v.osc[i].ks, v.osc[i].mul, v.osc[i].dt1, v.osc[i].dt2, v.osc[i].ame);
		}
	}
};

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		try {
			MDXOPM o(argv[i]);
		} catch(exceptionf &e) {
			fprintf(stderr, "Error: %s\n", e.what());
			return 1;
		}
	}
	return 0;
}

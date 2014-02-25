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
		printf("CH: %d %d 4 0 0 %d 0\n", v.getFL(), v.getCON(), v.slot_mask);
		for(int i = 0; i < 4; i++) {
			printf("%s: %d %d %d %d %d %d %d %d %d %d %d\n",
				MDXVoice::oscName(i), v.osc[i].getAR(), v.osc[i].getD1R(), v.osc[i].getD2R(),
				v.osc[i].getRR(), v.osc[i].getD1L(), v.osc[i].getTL(), v.osc[i].getKS(),
				v.osc[i].getMUL(), v.osc[i].getDT1(), v.osc[i].getDT2(), v.osc[i].getAME());
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

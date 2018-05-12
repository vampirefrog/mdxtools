#include "MDX.h"
#include "tools.h"

static void printVoice(MDXVoice *v, int num, FileWriteStream *o) {
	o->printf("@:%d MDX Instrument @%d\n", num, v->number);
	o->printf("LFO: 128 64 64 0 0\n");
	o->printf("CH: 192 %d %d 0 0 %d 0\n", v->getFL(), v->getCON(), v->slot_mask << 3);
	for(int j = 0; j < 4; j++) {
		int i = j;
		if(i == 1) i = 2;
		else if(i == 2) i = 1;
		o->printf("%s: %d %d %d %d %d %d %d %d %d %d %d\n",
			MDXVoice::oscName(j), v->osc[i].getAR(), v->osc[i].getD1R(), v->osc[i].getD2R(),
			v->osc[i].getRR(), v->osc[i].getD1L(), v->osc[i].getTL(), v->osc[i].getKS(),
			v->osc[i].getMUL(), v->osc[i].getDT1(), v->osc[i].getDT2(), v->osc[i].getAME() << 7);
	}
}

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		try {
			FileReadStream s(argv[i]);
			MDXHeader h;
			h.read(s);
			FileWriteStream o(replaceExtension(argv[i], "opm"));
			o.printf("// Converted from \"%s\" using mdx2opm\n", argv[i]);
			o.printf("// https://github.com/vampirefrog/mdxtools\n");
			o.printf("// LFO: LFRQ AMD PMD WF NFRQ\r\n");
			o.printf("// @:[Num] [Name]\r\n");
			o.printf("// CH: PAN   FL CON AMS PMS SLOT NE\r\n");
			o.printf("// [OPname]: AR D1R D2R  RR D1L  TL  KS MUL DT1 DT2 AMS-EN\r\n");
			int num = 0;
			for(int i = 0; i < 256; i++) {
				if(h.voices[i]) printVoice(h.voices[i], num++, &o);
			}
		} catch(exceptionf &e) {
			fprintf(stderr, "Error: %s\n", e.what());
			return 1;
		}
	}
	return 0;
}

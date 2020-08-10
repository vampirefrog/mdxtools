#include <stdio.h>

#include "mdx.h"
#include "tools.h"

static void printVoice(uint8_t *v, int num) {
	printf("@:%d MDX Instrument @%d\n", num, mdx_voice_get_id(v));
	printf("LFO: 128 64 64 0 0\n");
	printf("CH: 192 %d %d 0 0 %d 0\n", mdx_voice_get_fl(v), mdx_voice_get_con(v), mdx_voice_get_slot_mask(v) << 3);
	int op[4] = { 0, 2, 1, 3 };
	const char *opNames[4] = { "M1", "C1", "M2", "C2" };
	for(int j = 0; j < 4; j++) {
		int i = op[j];
		printf("%s: %d %d %d %d %d %d %d %d %d %d %d\n",
			opNames[j],
			mdx_voice_osc_get_ar(v, i),
			mdx_voice_osc_get_d1r(v, i),
			mdx_voice_osc_get_d2r(v, i),
			mdx_voice_osc_get_rr(v, i),
			mdx_voice_osc_get_d1l(v, i),
			mdx_voice_osc_get_tl(v, i),
			mdx_voice_osc_get_ks(v, i),
			mdx_voice_osc_get_mul(v, i),
			mdx_voice_osc_get_dt1(v, i),
			mdx_voice_osc_get_dt2(v, i),
			mdx_voice_osc_get_ame(v, i) << 7
		);
	}
}

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		size_t l;
		uint8_t *mdx_data = load_file(argv[i], &l);
		if(!mdx_data) {
			return 1;
		}
		struct mdx_file f;
		int er = mdx_file_load(&f, mdx_data, l);
		if(er != MDX_SUCCESS) {
			printf("Error loading MDX file \"%s\": %s (%d)\n", argv[i], mdx_error_name(er), er);
			return 2;
		}

		printf("// Converted from \"%s\" using mdx2opm\n", argv[i]);
		printf("// https://github.com/vampirefrog/mdxtools\n");
		printf("// LFO: LFRQ AMD PMD WF NFRQ\r\n");
		printf("// @:[Num] [Name]\r\n");
		printf("// CH: PAN   FL CON AMS PMS SLOT NE\r\n");
		printf("// [OPname]: AR D1R D2R  RR D1L  TL  KS MUL DT1 DT2 AMS-EN\r\n");
		for(int i = 0; i < 256; i++) {
			if(i < f.num_voices) printVoice(f.voices[i], i);
			// else printEmptyVoice(i+1);
		}
	}
	return 0;
}

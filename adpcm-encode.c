#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "adpcm.h"

int main(int argc, char **argv) {
	struct adpcm_status st;
	adpcm_init(&st);

	FILE *fin = stdin;
	if(argc >= 2) {
		fin = fopen(argv[1], "rb");
		if(!fin) {
			fprintf(stderr, "Could not open input file %s: %s (%d)\n", argv[1], strerror(errno), errno);
			return 1;
		}
	}

	FILE *fout = stdout;
	if(argc >= 3) {
		fout = fopen(argv[2], "wb");
		if(!fout) {
			fprintf(stderr, "Could not open output file %s: %s (%d)\n", argv[2], strerror(errno), errno);
			return 1;
		}
	}

	while(!feof(fin)) {
		int16_t i;

		fread(&i, sizeof(i), 1, fin);
		i /= 16;
		uint8_t u1 = adpcm_encode(i, &st);
		fread(&i, sizeof(i), 1, fin);
		i /= 16;
		uint8_t u2 = adpcm_encode(i, &st);
		uint8_t u = (u2 << 4) | (u1 & 0x0f);
		fwrite(&u, 1, 1, fout);
	}

	return 0;
}

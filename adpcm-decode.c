#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "adpcm.h"

int main(int argc, char **argv) {
	struct adpcm_status st;
	adpcm_init(&st);
	while(!feof(stdin)) {
		int8_t i;
		fread(&i, 1, 1, stdin);
		uint16_t o;
		o = adpcm_decode(i >> 4, &st);
		o *= 16;
		fwrite(&o, 1, 2, stdout);
		o = adpcm_decode(i & 0x0f, &st);
		o *= 16;
		fwrite(&o, 1, 2, stdout);
	}

	return 0;
}

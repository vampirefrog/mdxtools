#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "adpcm.h"

int main(int argc, char **argv) {
	struct adpcm_status st;
	adpcm_init(&st);
	while(!feof(stdin)) {
		int16_t i;
		fread(&i, 1, 2, stdin);
		i /= 16;
		uint8_t u1 = adpcm_encode(i, &st);
		fread(&i, 1, 2, stdin);
		i /= 16;
		uint8_t u2 = adpcm_encode(i, &st);
		uint8_t u = (u2 << 4) | (u1 & 0x0f);
		fwrite(&u, 1, 1, stdout);
	}

	return 0;
}

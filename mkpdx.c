#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void write_big_uint32(FILE *f, uint32_t i) {
	uint8_t buf[4] = {
		i >> 24,
		(i >> 16) & 0xff,
		(i >> 8) & 0xff,
		i & 0xff
	};
	fwrite(buf, 1, 4, f);
}

int main(int argc, char **argv) {
	int ofs = 96 * 8;
	for(int i = 1; i <= 96; i++) {
		int size = 0;
		if(i < argc) {
			struct stat st;
			int r = stat(argv[i], &st);
			if(r < 0) {
				fprintf(stderr, "Could not stat %s: %s (%d)\n", argv[i], strerror(errno), errno);
				continue;
			}
			size = st.st_size;
		}
		write_big_uint32(stdout, ofs);
		write_big_uint32(stdout, size);
		ofs += size;
	}
	for(int i = 1; i < argc; i++) {
		FILE *f = fopen(argv[i], "rb");
		if(!f) {
			fprintf(stderr, "Could not open %s: %s (%d)\n", argv[i], strerror(errno), errno);
			continue;
		}
		uint8_t buf[1024];
		while(!feof(f)) {
			int r = fread(buf, 1, sizeof(buf), f);
			fwrite(buf, 1, r, stdout);
		}
		fclose(f);
	}

	return 0;
}

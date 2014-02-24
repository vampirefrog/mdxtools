#include <stdio.h>

#include "exceptionf.h"
#include "MDXVGM.h"

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		try {
			char buf[256];
			snprintf(buf, sizeof(buf), "%s.vgm", argv[i]);
			MDXVGM s(argv[i], buf);
		} catch(exceptionf &e) {
			fprintf(stderr, "Error: %s\n", e.what());
		}
	}
	return 0;
}

#include <stdio.h>

#include "exceptionf.h"
#include "MDXSerializer.h"

class MDXVGM: public MDXSerializer {
public:
	MDXVGM() {}
	MDXVGM(const char *filename) {
		load(filename);
	}
private:
	virtual void handleNote(int chan, int n) {
		printf("%d: note %d (%s)\n", chan, n, MDX::noteName(n));
	}
	virtual void handleRest(int r) {
		// divided 44100 and 4000000 by 100, to avoid int overflow
		printf("rest %d (%d samples)\n", r, r * 441 * 1024 * (256 - tempo) / 40000);
	}
};

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		try {
			MDXVGM s(argv[i]);
		} catch(exceptionf &e) {
			fprintf(stderr, "Error: %s\n", e.what());
		}
	}
	return 0;
}

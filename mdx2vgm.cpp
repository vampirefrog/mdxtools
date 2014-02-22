#include <stdio.h>

#include "exceptionf.h"
#include "MDXSerializer.h"

class MDXVGMSerialParser: public MDXSerialParser {

};

class MDXVGM: public MDXSerializer<> {
public:
	MDXVGM(const char *filename): MDXSerializer(filename) {}
private:
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

#include "MDXMidi.h"

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		MDXMidi m(argv[i]);
		char buf[256];
		snprintf(buf, sizeof(buf), "%s.mid", argv[i]);
		m.write(buf);
	}
	return 0;
}

#include "MDXMidi.h"

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		char buf[256];
		snprintf(buf, sizeof(buf), "%s.mid", argv[i]);
		MDXMidi m;
		m.open(argv[i], buf);
	}
	return 0;
}

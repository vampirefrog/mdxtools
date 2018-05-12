#include "Midi.h"

int main(int argc, char **argv) {
	try {
		MidiDumpStream f(argv[1]);
	} catch(exceptionf e) {
		fprintf(stderr, "Error: %s\n", e.what());
		return 1;
	}
	return 0;
}

#include "VGM.h"

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		VGM vgm(argv[i]);
		vgm.dumpInfo();
	}
	return 0;
}

#include "VGMWriter.h"

int main(int argc, char **argv) {
	VGMWriter w;
	w.version = 0x150;
	w.sn76489_clock = 3579545;
	w.writePSG(0b10010000); // Volume full
	w.writePSG(0b10000011);
	w.writePSG(0b00000000);
	w.writeWait(44100); // 1 sec
	w.write("test.vgm");
	return 0;
}

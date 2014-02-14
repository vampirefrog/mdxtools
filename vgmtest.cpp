#include "VGMWriter.h"

int main(int argc, char **argv) {
	VGMWriter w;
	w.version = 0x150;
	w.sn76489_clock = 3579545;
	w.rate = 60; // japanese
	float doremi[] = { 261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25 };

	for(int i = 0; i < 8; i++) {
		w.writePSGTone(0, 0, w.sn76489_clock / 32 / doremi[i]);
		w.writePSGTone(1, 0, w.sn76489_clock / 32 / doremi[7-i/2]);
		w.writeWait(22050); // 1/2 sec
	}

	w.write("test.vgm");
	return 0;
}

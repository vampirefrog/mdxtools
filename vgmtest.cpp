#include "VGMWriter.h"

int main(int argc, char **argv) {
	VGMWriter w;
	w.version = 0x150;
	w.rate = 60; // japanese

#if 0
	w.sn76489_clock = 3579545;
	w.rate = 60; // japanese
	float doremi[] = { 261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25 };

	w.writePSGVolume(0, 0);
	w.writePSGVolume(1, 20);
	for(int i = 0; i < 8; i++) {
		w.writePSGTone(0, w.sn76489_clock / 32 / doremi[i]);
		w.writePSGTone(1, w.sn76489_clock / 32 / doremi[7-i/2]);
		w.writeWait(22050); // 1/2 sec
	}
#else
	w.ym2151_clock = 4000000;

	w.writeYM2151(0x58, 0x01); // C2: DT1 & MUL
	w.writeYM2151(0x78, 0x10); // C2: TL
	w.writeYM2151(0x98, 0x1F); // C2: KS & AR
	w.writeYM2151(0xB8, 0x00); // C2: AMS-EN & D1R
	w.writeYM2151(0xD8, 0x00); // C2: DT2 & D2R
	w.writeYM2151(0xF8, 0x0F); // C2: D1L & RR
	w.writeYM2151(0x20, 0xC0); // Channel 1: RL & FL & CONECT -

	uint8_t notes[] = {
		0x4f, 0x4e, 0x4d, 0x4c,
		0x4b, 0x4a, 0x49, 0x48,
		0x47, 0x46, 0x45, 0x44,
		0x43, 0x42, 0x41, 0x40
	};

	for(int i = 0; i < sizeof(notes); i++) {
		w.writeYM2151(0x28, ((notes[i] / 12) << 4) | ((notes[i] % 12) * 16 / 12)); // Channel 1: KC (Key Code)
		w.writeYM2151(0x08, 0x78); // Key ON/OFF
		w.writeWait(11025);
		w.writeYM2151(0x08, 0x00);
		w.writeWait(11025);
	}
#endif

	w.write("test.vgm");
	return 0;
}

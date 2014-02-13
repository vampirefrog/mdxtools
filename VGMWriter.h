#ifndef VGMWRITER_H_
#define VGMWRITER_H_

#include "VGM.h"
#include "FileStream.h"
#include "Buffer.h"

struct VGMWriter: VGM {
	Buffer buf;
	void writeGGPSG(uint8_t nn) {
		buf.putUint8(0x4f);
		buf.putUint8(nn);
	}
	void writePSG(uint8_t nn) {
		buf.putUint8(0x50);
		buf.putUint8(nn);
	}
	void writeRest(uint16_t len) {
		if(len == 735) buf.putUint8(0x62);
		else if(len == 882) buf.putUint8(0x63);
		else {
			buf.putUint8(0x61);
			buf.putUint8(len & 0xff);
			buf.putUint8(len >> 8);
		}
	}
	void write(const char *filename) {
		FileStream f(filename, "w0");
		f.write("Vgm ");
		f.writeUint32(buf.len + 256);
	}
};

#endif /* VGMWRITER_H_ */

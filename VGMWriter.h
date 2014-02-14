#ifndef VGMWRITER_H_
#define VGMWRITER_H_

#include "VGM.h"
#include "FileStream.h"
#include "Buffer.h"

struct VGMWriter: VGM {
	Buffer buf;
	VGMWriter() { reset(); }
	void writeGGPSG(uint8_t nn) {
		buf.putUint8(0x4f);
		buf.putUint8(nn);
	}
	void writePSG(uint8_t nn) {
		buf.putUint8(0x50);
		buf.putUint8(nn);
	}
	void writePSGTone(uint8_t channel, uint8_t vol, uint16_t freq) {
		uint8_t chan_bits = (channel & 0x03) << 5;
		writePSG(0b10010000 | chan_bits | (vol & 0x0f));
		writePSG(0b10000000 | chan_bits | (freq & 0x0f));
		writePSG(0b00000000 | ((freq & 0x3f0) >> 4));
	}
	void writeWait(uint16_t len) {
		if(len == 735) buf.putUint8(0x62);
		else if(len == 882) buf.putUint8(0x63);
		else {
			buf.putUint8(0x61);
			buf.putUint8(len & 0xff);
			buf.putUint8(len >> 8);
		}
		total_samples += len;
	}
	void write(const char *filename) {
		FileStream f(filename, "w");

		f.write("Vgm ");
		f.writeUint32(buf.len + 256);
		f.writeUint32(version);
		f.writeUint32(sn76489_clock);

		gd3_offset = 0;
		f.writeUint32(ym2413_clock);
		f.writeUint32(gd3_offset);
		f.writeUint32(total_samples);
		f.writeUint32(loop_offset);

		f.writeUint32(loop_samples);
		f.writeUint32(rate);
		f.writeUint16(sn76489_feedback);
		f.writeUint8(sn76489_shift_reg_width);
		f.writeUint8(sn76489_flags);
		f.writeUint32(ym2612_clock);

		vgm_data_offset = 0x100 - 0x34;
		f.writeUint32(ym2151_clock); f.writeUint32(vgm_data_offset); f.writeUint32(sega_pcm_clock); f.writeUint32(sega_pcm_interface_reg);
		f.writeUint32(rf5c68_clock); f.writeUint32(ym2203_clock); f.writeUint32(ym2608_clock); f.writeUint32(ym2610b_clock);
		f.writeUint32(ym3812_clock); f.writeUint32(ym3526_clock); f.writeUint32(ym8950_clock); f.writeUint32(ymf262_clock);
		f.writeUint32(ymf278b_clock); f.writeUint32(ymf271_clock); f.writeUint32(ymz280b_clock); f.writeUint32(rf5c164_clock);
		f.writeUint32(pwm_clock); f.writeUint32(ay8910_clock);
		f.writeUint32((ay8910_chip_type << 24) | (ay_flags & 0x00ffffff));
		f.writeUint8(volume_modifier); f.writeUint8(0x00); f.writeUint8(loop_base); f.writeUint8(loop_modifier);
		f.writeUint32(gb_dmg_clock); f.writeUint32(nes_apu_clock); f.writeUint32(multipcm_clock); f.writeUint32(upd7759_clock);
		f.writeUint32(okim6258_clock);
		f.writeUint8(okim6258_flags); f.writeUint8(k051649_flags); f.writeUint8(c140_chip_type); f.writeUint8(0x00);
		f.writeUint32(okim6295_clock); f.writeUint32(k051649_clock);
		f.writeUint32(k054539_clock); f.writeUint32(huc6280_clock); f.writeUint32(c140_clock); f.writeUint32(k053260_clock);
		f.writeUint32(pokey_clock); f.writeUint32(qsound_clock);
		f.writeUint32(0);
		f.writeUint32(extra_header_offset);
		f.fill(4 * 16);
		buf.putUint8(0x66); // EOF
		f.write(buf);
	}
};

#endif /* VGMWRITER_H_ */

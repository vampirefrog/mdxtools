#ifndef VGMWRITER_H_
#define VGMWRITER_H_

#include "VGM.h"
#include "FS.h"

enum VGMChipType {
	VGM_OKIM6258 = 0x17,
};

struct VGMWriter: VGM {
	Buffer buf;
	VGMWriter() { reset(); }
	void writeGGPSG(uint8_t nn) {
		buf.writeUint8(0x4f);
		buf.writeUint8(nn);
	}
	void writePSG(uint8_t nn) {
		buf.writeUint8(0x50);
		buf.writeUint8(nn);
	}
	void writePSGVolume(int channel, uint8_t vol) {
		uint8_t chan_bits = (channel & 0x03) << 5;
		writePSG(0b10010000 | chan_bits | (vol & 0x0f));
	}
	void writePSGTone(uint8_t channel, uint16_t freq) {
		uint8_t chan_bits = (channel & 0x03) << 5;
		writePSG(0b10000000 | chan_bits | (freq & 0x0f));
		writePSG(0b00000000 | ((freq & 0x3f0) >> 4));
	}
	void writeYM2151(uint8_t reg, uint8_t val) {
		buf.writeUint8(0x54);
		buf.writeUint8(reg);
		buf.writeUint8(val);
	}
	void writeYM2413(uint8_t reg, uint8_t val) {
		buf.writeUint8(0x51);
		buf.writeUint8(reg);
		buf.writeUint8(val);
	}
	void writeOKIM6258(uint8_t reg, uint8_t data) {
		buf.writeUint8(0xb7);
		buf.writeUint8(reg);
		buf.writeUint8(data);
	}
	void writeOKIM6258Data(uint8_t data) {
		writeOKIM6258(0x01, data);
	}
	void writeOKIM6258Pan(uint8_t pan) {
		writeOKIM6258(0x02, pan);
	}
	void writeDataBlock(uint8_t type, uint32_t size, uint8_t *data) {
		buf.writeUint8(0x67);
		buf.writeUint8(0x66);
		buf.writeUint8(type);
		buf.writeLittleUint32(size);
		buf.write(data, size);
	}
	void writeSetupStreamControl(uint8_t streamId, VGMChipType chipType, uint8_t port, uint8_t reg) {
		buf.writeUint8(0x90);
		buf.writeUint8(streamId);
		buf.writeUint8(chipType);
		buf.writeUint8(port);
		buf.writeUint8(reg);
	}
	void writeSetStreamData(uint8_t streamId, uint8_t dataBankId, uint8_t stepSize, uint8_t stepBase) {
		buf.writeUint8(0x91);
		buf.writeUint8(streamId);
		buf.writeUint8(dataBankId);
		buf.writeUint8(stepSize);
		buf.writeUint8(stepBase);
	}
	void writeSetStreamFrequency(uint8_t streamId, uint32_t freq) {
		buf.writeUint8(0x92);
		buf.writeUint8(streamId);
		buf.writeLittleUint32(freq);
	}
	void writeStartStream(uint8_t streamId, uint16_t blockId, uint8_t flags) {
		buf.writeUint8(0x95);
		buf.writeUint8(streamId);
		buf.writeLittleUint16(blockId);
		buf.writeUint8(flags);
	}
	void writeStopStream(uint8_t streamId) {
		buf.writeUint8(0x94);
		buf.writeUint8(streamId); // 0xff stops all streams
	}
	void writeWait(uint16_t len) {
		if(len == 735) buf.writeUint8(0x62);
		else if(len == 882) buf.writeUint8(0x63);
		else {
			buf.writeUint8(0x61);
			buf.writeUint8(len & 0xff);
			buf.writeUint8(len >> 8);
		}
		total_samples += len;
	}
	void write(const char *filename) {
		FileWriteStream f(filename);

		buf.writeUint8(0x66); // EOF

		f.write("Vgm ");
		f.writeLittleUint32(buf.len + 256 - 4);
		f.writeLittleUint32(version);
		f.writeLittleUint32(sn76489_clock);

		gd3_offset = 0;
		f.writeLittleUint32(ym2413_clock);
		f.writeLittleUint32(gd3_offset);
		f.writeLittleUint32(total_samples);
		f.writeLittleUint32(loop_offset);

		f.writeLittleUint32(loop_samples);
		f.writeLittleUint32(rate);
		f.writeLittleUint16(sn76489_feedback);
		f.writeUint8(sn76489_shift_reg_width);
		f.writeUint8(sn76489_flags);
		f.writeLittleUint32(ym2612_clock);

		vgm_data_offset = 0x100 - 0x34;
		f.writeLittleUint32(ym2151_clock); f.writeLittleUint32(vgm_data_offset); f.writeLittleUint32(sega_pcm_clock); f.writeLittleUint32(sega_pcm_interface_reg);
		f.writeLittleUint32(rf5c68_clock); f.writeLittleUint32(ym2203_clock); f.writeLittleUint32(ym2608_clock); f.writeLittleUint32(ym2610b_clock);
		f.writeLittleUint32(ym3812_clock); f.writeLittleUint32(ym3526_clock); f.writeLittleUint32(ym8950_clock); f.writeLittleUint32(ymf262_clock);
		f.writeLittleUint32(ymf278b_clock); f.writeLittleUint32(ymf271_clock); f.writeLittleUint32(ymz280b_clock); f.writeLittleUint32(rf5c164_clock);
		f.writeLittleUint32(pwm_clock); f.writeLittleUint32(ay8910_clock);
		f.writeLittleUint32((ay8910_chip_type << 24) | (ay_flags & 0x00ffffff));
		f.writeUint8(volume_modifier); f.writeUint8(0x00); f.writeUint8(loop_base); f.writeUint8(loop_modifier);
		f.writeLittleUint32(gb_dmg_clock); f.writeLittleUint32(nes_apu_clock); f.writeLittleUint32(multipcm_clock); f.writeLittleUint32(upd7759_clock);
		f.writeLittleUint32(okim6258_clock);
		f.writeUint8(okim6258_flags); f.writeUint8(k051649_flags); f.writeUint8(c140_chip_type); f.writeUint8(0x00);
		f.writeLittleUint32(okim6295_clock); f.writeLittleUint32(k051649_clock);
		f.writeLittleUint32(k054539_clock); f.writeLittleUint32(huc6280_clock); f.writeLittleUint32(c140_clock); f.writeLittleUint32(k053260_clock);
		f.writeLittleUint32(pokey_clock); f.writeLittleUint32(qsound_clock);
		f.writeLittleUint32(0);
		f.writeLittleUint32(extra_header_offset);
		f.fill(4 * 16);
		f.write(buf);
	}
};

#endif /* VGMWRITER_H_ */

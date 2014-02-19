#ifndef WAVWRITER_H_
#define WAVWRITER_H_

class WAVWriter {
public:
	static void write(const char *filename, void *data, int data_length, int sample_rate = 44100, int bits_per_sample = 16, int num_channels = 2) {
		FileStream s(filename, "wb");
		int multiplier = 1;
		if(bits_per_sample == 4) {
			multiplier = 2;
		}
		s.write("RIFF");
		s.writeUint32(4 + 24 + 8 + data_length * multiplier);
		s.write("WAVE");
		s.write("fmt ");
		s.writeUint32(16);
		s.writeUint16(1); // PCM format
		s.writeUint16(num_channels);
		s.writeUint32(sample_rate);
		s.writeUint32(sample_rate * num_channels * bits_per_sample * multiplier / 8);
		s.writeUint16(num_channels * bits_per_sample * multiplier / 8);
		s.writeUint16(bits_per_sample * multiplier);
		s.write("data");
		s.writeUint32(data_length * multiplier);
		if(multiplier == 2) {
			printf("MULTIPLIER\n");
			for(int i = 0; i < data_length; i++) {
				uint8_t b1 = ((uint8_t *)data)[i] & 0x0f;
				uint8_t b2 = (((uint8_t *)data)[i] & 0xf0) >> 4;
				unsigned int samples = b2 | (b2 << 4) | (b1 << 8) | (b1 << 12);
				s.writeUint16(samples);
			}
		} else s.write((uint8_t *)data, data_length);
	}
};

#endif /* WAVEWRITER_H_ */

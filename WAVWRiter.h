#ifndef WAVWRITER_H_
#define WAVWRITER_H_

class WAVWriter {
public:
	static void write(const char *filename, void *data, int data_length, int sample_rate = 44100, int bits_per_sample = 16, int num_channels = 2) {
		FileStream s(filename, "wb");
		s.write("RIFF");
		s.writeUint32(4 + 24 + 8 + data_length);
		s.write("WAVE");
		s.write("fmt ");
		s.writeUint32(16);
		s.writeUint16(1); // PCM format
		s.writeUint16(num_channels);
		s.writeUint32(sample_rate);
		s.writeUint32(sample_rate * num_channels * bits_per_sample / 8);
		s.writeUint16(num_channels * bits_per_sample / 8);
		s.writeUint16(bits_per_sample);
		s.write("data");
		s.writeUint32(data_length);
		s.write((uint8_t *)data, data_length);
	}
};

#endif /* WAVEWRITER_H_ */

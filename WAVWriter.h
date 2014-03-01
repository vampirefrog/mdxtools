#ifndef WAVWRITER_H_
#define WAVWRITER_H_

#include "FS.h"

class WAVWriter {
public:
	static void write(const char *filename, void *data, int data_length, int sample_rate = 44100, int bits_per_sample = 16, int num_channels = 2) {
		FileWriteStream s(filename);
		s.write("RIFF");
		s.writeLittleUint32(4 + 24 + 8 + data_length);
		s.write("WAVE");
		s.write("fmt ");
		s.writeLittleUint32(16);
		s.writeLittleUint16(1); // PCM format
		s.writeLittleUint16(num_channels);
		s.writeLittleUint32(sample_rate);
		s.writeLittleUint32(sample_rate * num_channels * bits_per_sample / 8);
		s.writeLittleUint16(num_channels * bits_per_sample / 8);
		s.writeLittleUint16(bits_per_sample);
		s.write("data");
		s.writeLittleUint32(data_length);
		s.write((uint8_t *)data, data_length);
	}
};

#endif /* WAVEWRITER_H_ */

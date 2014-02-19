#include <stdio.h>
#include "PDX.h"
#include "WAVWriter.h"

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		PDXLoader p(argv[i]);
		for(int j = 0; j < PDX_NUM_SAMPLES; j++) {
			if(p.samples[j].length > 0) {
				char buf[256];
				snprintf(buf, sizeof(buf), "%s-%d.wav", argv[i], j);
				uint8_t *data = p.loadSample(j);
				WAVWriter::write(buf, data, p.samples[j].length, 15600, 16, 1);
				delete[] data;
			}
		}
	}
}

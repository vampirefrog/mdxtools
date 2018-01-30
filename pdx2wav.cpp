#include <stdio.h>
#include "PDX.h"
#include "WAVWriter.h"
#include "ADPCMDecoder.h"

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		try {
			PDXLoader p(argv[i]);
			for(int j = 0; j < PDX_NUM_SAMPLES; j++) {
				if(p.samples[j].length > 0) {
					uint8_t *data = p.loadSample(j);
					int16_t *wavData = new int16_t[p.samples[j].length * 2];
					ADPCMDecoder d;
					d.decode(data, p.samples[j].length * 2, wavData);
					char buf[256];
					snprintf(buf, sizeof(buf), "%s-%d.wav", argv[i], j);
					WAVWriter::write(buf, wavData, p.samples[j].length * 2 * 2, 15600, 16, 1);
					delete[] data;
					delete[] wavData;
				}
			}
		} catch(exceptionf &e) {
			printf("Error loading %s: %s\n", argv[i], e.what());
		}
	}
}

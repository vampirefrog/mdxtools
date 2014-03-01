#ifndef PDX_H_
#define PDX_H_

#include "FS.h"
#include "exceptionf.h"

#define PDX_NUM_SAMPLES 96

struct PDXLoaderSample {
	uint32_t offset;
	uint32_t length;
};

class PDXLoader {
private:
	FileReadStream s;

public:
	PDXLoaderSample samples[PDX_NUM_SAMPLES];

	PDXLoader() {}
	PDXLoader(const char * filename) {
		open(filename);
	}

	void open(const char *filename) {
		s.openCaseInsensitive(filename);
		for(int i = 0; i < PDX_NUM_SAMPLES; i++) {
			samples[i].offset = s.readBigUint32();
			samples[i].length = s.readBigUint32();
		}
	}

	uint8_t *loadSample(uint8_t sampleNum) {
		if(sampleNum >= 96) return NULL;
		uint8_t *ret = new uint8_t[samples[sampleNum].length];
		s.seek(samples[sampleNum].offset);
		s.read(ret, samples[sampleNum].length);
		return ret;
	}
};

#endif /* PDX_H_ */

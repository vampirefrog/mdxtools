#ifndef MDXDUMPER_H_
#define MDXDUMPER_H_

#include "MDX.h"
#include "tools.h"

class MDXDumper {
public:
	MDXDumper(const char *filename) {
		load(filename);
	}

	virtual void handleHeader() {
		char *t = iconvAlloc(title, "utf-8", "Shift_JIS");
		printf("Title: \"%s\"\n", t);
		free(t);
		printf("PCM file: \"%s\"\n", pcm_file);
		printf("Voice offset: 0x%04x\n", file_base + Voice_offset);
		printf("MML offsets (%d):\n", mml_offset[0] / 2 - 1);
		for(int i = 0; i < mml_offset[0] / 2 - 1 &&  i < 16; i++) {
			printf("  %c: 0x%04x\n", channelName(i), mml_offset[i]);
		}
	}
	virtual void handleVoice(MDXVoice &v) { v.dump(); }
};

#endif /* MDXDUMPER_H_ */

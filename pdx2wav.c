#include <stdio.h>
#include <audiofile.h>
#include "tools.h"
#include "pdx.h"
#include "adpcm.h"

int main(int argc, char **argv) {
	if(argc < 2) {
		fprintf(stderr, "Usage: %s <file.pdx> [prefix]\n", argv[0]);
		return 1;
	}

	size_t data_len;
	uint8_t *data = load_file(argv[1], &data_len);

	struct pdx_file pdx;
	pdx_file_load(&pdx, data, data_len);

	for(int i = 0; i < 96; i++) {
		if(pdx.samples[i].len == 0) continue;

		AFfilesetup setup = afNewFileSetup();
		afInitFileFormat(setup, AF_FILE_WAVE);
		afInitChannels(setup, AF_DEFAULT_TRACK, 1);
		afInitRate(setup, AF_DEFAULT_TRACK, 15625);
		afInitSampleFormat(setup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);
		char buf[1024];
		snprintf(buf, sizeof(buf), "%s.%03d.wav", argv[1], i);
		printf("Opening %s\n", buf);
		AFfilehandle file = afOpenFile(buf, "w", setup);
		if(file == AF_NULL_FILEHANDLE) {
			fprintf(stderr, "Error opening output file.\n");
			return 1;
		}

		struct adpcm_status st;
		adpcm_init(&st);
		printf("samples %d len=%d\n", i, pdx.samples[i].len);
		for(int j = 0; j < pdx.samples[i].len; j++) {
			int16_t frame[2];
			frame[0] = adpcm_decode(pdx.samples[i].data[j] & 0x0f, &st) << 3;
			frame[1] = adpcm_decode(pdx.samples[i].data[j] >> 4, &st) << 3;
			afWriteFrames(file, AF_DEFAULT_TRACK, frame, 1);
		}
		for(int j = 0; j < 24; j++) {
			int16_t frame[2];
			frame[0] = adpcm_decode(8, &st) << 3;
			frame[1] = adpcm_decode(0, &st) << 3;
			afWriteFrames(file, AF_DEFAULT_TRACK, frame, 1);
		}

		afCloseFile(file);
		afFreeFileSetup(setup);
	}

	return 0;
}

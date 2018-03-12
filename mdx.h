#ifndef MDX_H_
#define MDX_H_

#include <stdint.h>

#include "pdx.h"

#define OPM_CLOCK 4000000

enum {
	MDX_SUCCESS = 0,
	MDX_ERR_BAD_TITLE,
	MDX_ERR_BAD_PCM_FILENAME,
	MDX_ERR_LZX, // MDX file is compressed with LZX
	MDX_MAX_ERR
};

struct mdx_file;

struct mdx_lfo {
	int enable, waveform, period, amplitude, phase, pitch;
};

struct mdx_channel {
	uint8_t *data;
	int data_len;
	int pos;
};

struct mdx_file {
	uint32_t data_start_ofs;
	uint8_t *title, title_len;
	uint8_t *pdx_filename, pdx_filename_len;

	uint8_t *data;
	int data_len;

	uint8_t *voices[256];
	int num_voices;

	struct pdx pdx;

	struct mdx_channel channels[16];
	int num_channels;
};

int mdx_file_load(struct mdx_file *f, uint8_t *data, int len);

int mdx_cmd_len(uint8_t *data, int pos, int len);
const char *mdx_lfo_waveform_name(uint8_t waveform);
const char *mdx_error_name(int err);

#endif /* MDX_H_ */

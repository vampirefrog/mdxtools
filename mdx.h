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

struct mdx_voice_osc {
	uint8_t dt1_mul;
	uint8_t tl;
	uint8_t ks_ar;
	uint8_t ame_d1r;
	uint8_t dt2_d2r;
	uint8_t d1l_rr;
} __attribute__((__packed__));

struct mdx_voice {
	uint8_t voice_id;
	uint8_t fl_con;
	uint8_t slot_mask;
	struct mdx_voice_osc osc[4];
} __attribute__((__packed__));

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

uint8_t mdx_voice_get_id(uint8_t *v);
uint8_t mdx_voice_get_fl(uint8_t *v);
uint8_t mdx_voice_get_con(uint8_t *v);
uint8_t mdx_voice_get_slot_mask(uint8_t *v);
uint8_t mdx_voice_osc_get_dt1(uint8_t *v, int osc);
uint8_t mdx_voice_osc_get_mul(uint8_t *v, int osc);
uint8_t mdx_voice_osc_get_tl(uint8_t *v, int osc);
uint8_t mdx_voice_osc_get_ks(uint8_t *v, int osc);
uint8_t mdx_voice_osc_get_ar(uint8_t *v, int osc);
uint8_t mdx_voice_osc_get_ame(uint8_t *v, int osc);
uint8_t mdx_voice_osc_get_d1r(uint8_t *v, int osc);
uint8_t mdx_voice_osc_get_dt2(uint8_t *v, int osc);
uint8_t mdx_voice_osc_get_d2r(uint8_t *v, int osc);
uint8_t mdx_voice_osc_get_d1l(uint8_t *v, int osc);
uint8_t mdx_voice_osc_get_rr(uint8_t *v, int osc);

#endif /* MDX_H_ */

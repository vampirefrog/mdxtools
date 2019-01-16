#pragma once

#include <stdint.h>
#include "mamedef.h"

struct okim6258 {
	uint8_t status;

	uint32_t master_clock;      /* master clock frequency */
	uint32_t divider;           /* master clock divider */
	uint8_t adpcm_type;         /* 3/4 bit ADPCM select */
	uint8_t data_in;            /* ADPCM data-in register */
	uint8_t nibble_shift;       /* nibble select */

	uint8_t output_bits, internal_10_bit;
	int32_t output_mask;

	// Valley Bell: Added a small queue to prevent race conditions.
	uint8_t data_buf[8];
	uint8_t data_in_last;
	uint8_t data_buf_pos;
	// Data Empty Values:
	//  00 - data written, but not read yet
	//  01 - read data, waiting for next write
	//  02 - tried to read, but had no data
	uint8_t data_empty;
	// Valley Bell: Added pan
	uint8_t pan;
	int32_t last_smpl;

	int32_t signal;
	int32_t step;

	uint8_t clock_buffer[0x04];
	uint32_t initial_clock;
	uint8_t initial_div;

	SRATE_CALLBACK SmpRateFunc;
	void* SmpRateData;
};

#define FOSC_DIV_BY_1024    0
#define FOSC_DIV_BY_768     1
#define FOSC_DIV_BY_512     2

#define TYPE_3BITS          0
#define TYPE_4BITS          1

#define OUTPUT_10BITS       0
#define OUTPUT_12BITS       1

int okim6258_init(struct okim6258 *chip, int clock, int divider, int adpcm_type, int output_12bits);

void okim6258_stop(struct okim6258 *chip);
void okim6258_reset(struct okim6258 *chip);

void okim6258_set_divider(struct okim6258 *chip, int val);
void okim6258_set_clock(struct okim6258 *chip, int val);
int okim6258_get_vclk(struct okim6258 *chip);

void okim6258_update(struct okim6258 *chip, stream_sample_t **outputs, int samples);

void okim6258_write(struct okim6258 *chip, uint8_t Port, uint8_t Data);

void okim6258_set_options(struct okim6258 *chip, uint16_t options);
void okim6258_set_srchg_cb(struct okim6258 *chip, SRATE_CALLBACK CallbackFunc, void* DataPtr);

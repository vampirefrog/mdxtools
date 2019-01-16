/**********************************************************************************************
 *
 *   OKI MSM6258 ADPCM
 *
 *   TODO:
 *   3-bit ADPCM support
 *   Recording?
 *
 **********************************************************************************************/

#include <stddef.h>	// for NULL
#include "mamedef.h"

#ifdef _DEBUG
#include <stdio.h>
#endif

#include <math.h>
#include "okim6258.h"

#define COMMAND_STOP		(1 << 0)
#define COMMAND_PLAY		(1 << 1)
#define	COMMAND_RECORD		(1 << 2)

#define STATUS_PLAYING		(1 << 1)
#define STATUS_RECORDING	(1 << 2)

static const int dividers[4] = { 1024, 768, 512, 512 };

#define QUEUE_SIZE	(1 << 1)
#define QUEUE_MASK	(QUEUE_SIZE - 1)

/* step size index shift table */
static const int index_shift[8] = { -1, -1, -1, -1, 2, 4, 6, 8 };

/* lookup table for the precomputed difference */
static int diff_lookup[49*16];

/* tables computed? */
static int tables_computed = 0;

/**********************************************************************************************

     compute_tables -- compute the difference tables

***********************************************************************************************/

static void compute_tables(void) {
	if (tables_computed)
		return;

	/* nibble to bit map */
	static const int nbl2bit[16][4] = {
		{ 1, 0, 0, 0}, { 1, 0, 0, 1}, { 1, 0, 1, 0}, { 1, 0, 1, 1},
		{ 1, 1, 0, 0}, { 1, 1, 0, 1}, { 1, 1, 1, 0}, { 1, 1, 1, 1},
		{-1, 0, 0, 0}, {-1, 0, 0, 1}, {-1, 0, 1, 0}, {-1, 0, 1, 1},
		{-1, 1, 0, 0}, {-1, 1, 0, 1}, {-1, 1, 1, 0}, {-1, 1, 1, 1}
	};

	int step, nib;

	/* loop over all possible steps */
	for (step = 0; step <= 48; step++) {
		/* compute the step value */
		int stepval = floor(16.0 * pow(11.0 / 10.0, (double)step));

		/* loop over all nibbles and compute the difference */
		for (nib = 0; nib < 16; nib++) {
			diff_lookup[step*16 + nib] = nbl2bit[nib][0] *
				(stepval   * nbl2bit[nib][1] +
				 stepval/2 * nbl2bit[nib][2] +
				 stepval/4 * nbl2bit[nib][3] +
				 stepval/8);
		}
	}

	tables_computed = 1;
}


static int16_t clock_adpcm(struct okim6258 *chip, uint8_t nibble) {
	int32_t max = chip->output_mask - 1;
	int32_t min = -chip->output_mask;

	// original MAME algorithm (causes a DC offset over time)
	//chip->signal += diff_lookup[chip->step * 16 + (nibble & 15)];

	// awesome algorithm ported from XM6 - it works PERFECTLY
	int sample = diff_lookup[chip->step * 16 + (nibble & 15)];
	chip->signal = ((sample << 8) + (chip->signal * 245)) >> 8;

	/* clamp to the maximum */
	if (chip->signal > max)
		chip->signal = max;
	else if (chip->signal < min)
		chip->signal = min;

	/* adjust the step size and clamp */
	chip->step += index_shift[nibble & 7];
	if (chip->step > 48)
		chip->step = 48;
	else if (chip->step < 0)
		chip->step = 0;

	/* return the signal scaled up to 32767 */
	return chip->signal << 4;
}

/**********************************************************************************************

     okim6258_update -- update the sound chip so that it is in sync with CPU execution

***********************************************************************************************/
void okim6258_update(struct okim6258 *chip, stream_sample_t **outputs, int samples) {
	stream_sample_t *bufL = outputs[0];
	stream_sample_t *bufR = outputs[1];

	if (chip->status & STATUS_PLAYING) {
		int nibble_shift = chip->nibble_shift;

		while (samples) {
			/* Compute the new amplitude and update the current step */
			//int nibble = (chip->data_in >> nibble_shift) & 0xf;
			int nibble;
			int16_t sample;

			if (! nibble_shift) {
				// 1st nibble - get data
				if (! chip->data_empty) {
					chip->data_in = chip->data_buf[chip->data_buf_pos >> 4];
					chip->data_buf_pos += 0x10;
					chip->data_buf_pos &= 0x7F;
					if ((chip->data_buf_pos >> 4) == (chip->data_buf_pos & 0x0F))
						chip->data_empty ++;
				} else {
					//chip->data_in = chip->data_in_last;
					if (chip->data_empty < 0x80)
						chip->data_empty ++;
				}
			}
			nibble = (chip->data_in >> nibble_shift) & 0xf;

			/* Output to the buffer */
			//int16_t sample = clock_adpcm(chip, nibble);
			if (chip->data_empty < 0x02) {
				sample = clock_adpcm(chip, nibble);
				chip->last_smpl = sample;
			} else {
				// Valley Bell: data_empty behaviour (loosely) ported from XM6
				if (chip->data_empty >= 0x02 + 0x01) {
					chip->data_empty -= 0x01;
					chip->signal = chip->signal * 15 / 16;
					chip->last_smpl = chip->signal << 4;
				}
				sample = chip->last_smpl;
			}

			nibble_shift ^= 4;

			*bufL++ = (chip->pan & 0x02) ? 0x00 : sample;
			*bufR++ = (chip->pan & 0x01) ? 0x00 : sample;
			samples--;
		}

		/* Update the parameters */
		chip->nibble_shift = nibble_shift;
	} else {
		/* Fill with 0 */
		while (samples--) {
			//*buffer++ = 0;
			*bufL++ = 0;
			*bufR++ = 0;
		}
	}
}

/**********************************************************************************************

     OKIM6258_start -- start emulation of an OKIM6258-compatible chip

***********************************************************************************************/
static int get_vclk(struct okim6258 *chip) {
	int clk_rnd;

	clk_rnd = chip->master_clock;
	clk_rnd += chip->divider / 2;	 // for better rounding - should help some of the streams

	return clk_rnd / chip->divider;
}

int okim6258_init(struct okim6258 *chip, int clock, int divider, int adpcm_type, int output_12bits) {
	compute_tables();

	chip->initial_clock = clock;
	chip->initial_div = divider;
	chip->master_clock = clock;
	chip->adpcm_type = /*intf->*/adpcm_type;
	chip->clock_buffer[0x00] = (clock & 0x000000FF) >>  0;
	chip->clock_buffer[0x01] = (clock & 0x0000FF00) >>  8;
	chip->clock_buffer[0x02] = (clock & 0x00FF0000) >> 16;
	chip->clock_buffer[0x03] = (clock & 0xFF000000) >> 24;
	chip->SmpRateFunc = NULL;

	/* D/A precision is 10-bits but 12-bit data can be output serially to an external DAC */
	chip->output_bits = output_12bits ? 12 : 10;
	chip->internal_10_bit = 0;
	if (chip->internal_10_bit)
		chip->output_mask = (1 << (chip->output_bits - 1));
	else
		chip->output_mask = (1 << (12 - 1));
	chip->divider = dividers[divider];

	chip->signal = -2;
	chip->step = 0;

	return get_vclk(chip);
}


/**********************************************************************************************

     OKIM6258_stop -- stop emulation of an OKIM6258-compatible chip

***********************************************************************************************/

void okim6258_stop(struct okim6258 *chip) {
}

void okim6258_reset(struct okim6258 *chip) {
	chip->master_clock = chip->initial_clock;
	chip->clock_buffer[0x00] = (chip->initial_clock & 0x000000FF) >>  0;
	chip->clock_buffer[0x01] = (chip->initial_clock & 0x0000FF00) >>  8;
	chip->clock_buffer[0x02] = (chip->initial_clock & 0x00FF0000) >> 16;
	chip->clock_buffer[0x03] = (chip->initial_clock & 0xFF000000) >> 24;
	chip->divider = dividers[chip->initial_div];
	if (chip->SmpRateFunc != NULL)
		chip->SmpRateFunc(chip->SmpRateData, get_vclk(chip));

	chip->signal = -2;
	chip->step = 0;
	chip->status = 0;

	// Valley Bell: Added reset of the Data In register.
	chip->data_in = 0x00;
	chip->data_buf[0] = chip->data_buf[1] = 0x00;
	chip->data_buf_pos = 0x00;
	chip->data_empty = 0xFF;
	chip->pan = 0x00;
}

/**********************************************************************************************

     okim6258_set_divider -- set the master clock divider

***********************************************************************************************/
void okim6258_set_divider(struct okim6258 *chip, int val) {
	chip->divider = dividers[val];

	if (chip->SmpRateFunc != NULL)
		chip->SmpRateFunc(chip->SmpRateData, get_vclk(chip));
}


/**********************************************************************************************

     okim6258_set_clock -- set the master clock

***********************************************************************************************/
void okim6258_set_clock(struct okim6258 *chip, int val) {
	if (val) {
		chip->master_clock = val;
	} else {
		chip->master_clock =	(chip->clock_buffer[0x00] <<  0) |
								(chip->clock_buffer[0x01] <<  8) |
								(chip->clock_buffer[0x02] << 16) |
								(chip->clock_buffer[0x03] << 24);
	}

	if (chip->SmpRateFunc != NULL)
		chip->SmpRateFunc(chip->SmpRateData, get_vclk(chip));
}


/**********************************************************************************************

     okim6258_get_vclk -- get the VCLK/sampling frequency

***********************************************************************************************/
int okim6258_get_vclk(struct okim6258 *chip) {
	return get_vclk(chip);
}


/**********************************************************************************************

     okim6258_data_w -- write to the control port of an OKIM6258-compatible chip

***********************************************************************************************/
static void okim6258_data_w(struct okim6258 *chip, uint8_t data) {
	if (chip->data_empty >= 0x02)
		chip->data_buf_pos = 0x00;
	chip->data_in_last = data;
	chip->data_buf[chip->data_buf_pos & 0x0F] = data;
	chip->data_buf_pos += 0x01;
	chip->data_buf_pos &= 0xF7;
	if ((chip->data_buf_pos >> 4) == (chip->data_buf_pos & 0x0F)) {
		//fprintf(stderr, "Warning: FIFO full!\n");
		chip->data_buf_pos = (chip->data_buf_pos & 0xF0) | ((chip->data_buf_pos-1) & 0x07);
	}
	chip->data_empty = 0x00;
}


/**********************************************************************************************

     okim6258_ctrl_w -- write to the control port of an OKIM6258-compatible chip

***********************************************************************************************/
static void okim6258_ctrl_w(struct okim6258 *chip, uint8_t data) {
	if (data & COMMAND_STOP) {
		chip->status &= ~(STATUS_PLAYING | STATUS_RECORDING);
		return;
	}

	if (data & COMMAND_PLAY) {
		if (!(chip->status & STATUS_PLAYING)) {
			chip->status |= STATUS_PLAYING;

			/* Also reset the ADPCM parameters */
			chip->signal = -2;	// Note: XM6 lets this fade to 0 when nothing is going on
			chip->step = 0;
			chip->nibble_shift = 0;

			chip->data_buf[0x00] = data;
			chip->data_buf_pos = 0x01;	// write pos 01, read pos 00
			chip->data_empty = 0x00;
		}
		chip->step = 0;	// this line was verified with the source of XM6
		chip->nibble_shift = 0;
	} else {
		chip->status &= ~STATUS_PLAYING;
	}

	if(data & COMMAND_RECORD) {
		// logerror("M6258: Record enabled\n");
		chip->status |= STATUS_RECORDING;
	} else {
		chip->status &= ~STATUS_RECORDING;
	}
}

static void okim6258_set_clock_byte(struct okim6258 *chip, uint8_t Byte, uint8_t val) {
	chip->clock_buffer[Byte] = val;
}

static void okim6258_pan_w(struct okim6258 *chip, uint8_t data) {
	chip->pan = data;
}


void okim6258_write(struct okim6258 *chip, uint8_t Port, uint8_t Data) {
	switch(Port) {
	case 0x00:
		okim6258_ctrl_w(chip, /*0x00, */Data);
		break;
	case 0x01:
		okim6258_data_w(chip, /*0x00, */Data);
		break;
	case 0x02:
		okim6258_pan_w(chip, Data);
		break;
	case 0x08:
	case 0x09:
	case 0x0A:
		okim6258_set_clock_byte(chip, Port & 0x03, Data);
		break;
	case 0x0B:
		okim6258_set_clock_byte(chip, Port & 0x03, Data);
		okim6258_set_clock(chip, 0);
		break;
	case 0x0C:
		okim6258_set_divider(chip, Data);
		break;
	}
}


void okim6258_set_options(struct okim6258 *chip, uint16_t options) {
	chip->internal_10_bit = (options >> 0) & 0x01;
}

void okim6258_set_srchg_cb(struct okim6258 *chip, SRATE_CALLBACK CallbackFunc, void* DataPtr) {
	// set Sample Rate Change Callback routine
	chip->SmpRateFunc = CallbackFunc;
	chip->SmpRateData = DataPtr;
}

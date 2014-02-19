#include <stdio.h>
#include <math.h>
#include "PDX.h"
#include "WAVWriter.h"

static const int index_shift[8] = { -1, -1, -1, -1, 2, 4, 6, 8 };
static int diff_lookup[49*16];

// ADPCM conversion code from MAME

static uint8_t O6258_output_bits = 10;	// output_12bits ? 12 : 10
static int32_t O6258_signal;
static int32_t O6258_step;
static uint8_t signal_gain = 6;

static void compute_tables(void) {
	// nibble to bit map
	static const int nbl2bit[16][4] = {
		{ 1, 0, 0, 0}, { 1, 0, 0, 1}, { 1, 0, 1, 0}, { 1, 0, 1, 1},
		{ 1, 1, 0, 0}, { 1, 1, 0, 1}, { 1, 1, 1, 0}, { 1, 1, 1, 1},
		{-1, 0, 0, 0}, {-1, 0, 0, 1}, {-1, 0, 1, 0}, {-1, 0, 1, 1},
		{-1, 1, 0, 0}, {-1, 1, 0, 1}, {-1, 1, 1, 0}, {-1, 1, 1, 1}
	};
	
	int step, nib;
	
	// loop over all possible steps
	for (step = 0; step <= 48; step++) {
		// compute the step value
		int stepval = (int)floor(16.0 * pow(11.0 / 10.0, (double)step));
		
		// loop over all nibbles and compute the difference
		for (nib = 0; nib < 16; nib++) {
			diff_lookup[step*16 + nib] = nbl2bit[nib][0] *
				(stepval   * nbl2bit[nib][1] +
				 stepval/2 * nbl2bit[nib][2] +
				 stepval/4 * nbl2bit[nib][3] +
				 stepval/8);
		}
	}
	
	return;
}

static INT16 clock_adpcm(uint8_t nibble) {
	int32_t max = (1 << (O6258_output_bits - 1)) - 1;
	int32_t min = -(1 << (O6258_output_bits - 1));
	int32_t final;
	
	O6258_signal += diff_lookup[O6258_step * 16 + (nibble & 15)];
	
	// clamp to the maximum
	if (O6258_signal > max)
		O6258_signal = max;
	else if (O6258_signal < min)
		O6258_signal = min;
	
	// adjust the step size and clamp
	O6258_step += index_shift[nibble & 7];
	if (O6258_step > 48)
		O6258_step = 48;
	else if (O6258_step < 0)
		O6258_step = 0;
	
	// return the signal scaled up to 32767
	//return O6258_signal << 4;
	final = O6258_signal << signal_gain;
	if (final < -0x8000)
		final = -0x8000;
	else if (final > 0x7FFF)
		final = 0x7FFF;
	return final;
}

void okim6258_decode(uint32_t SampleCnt, int16_t* OutData, const uint8_t* InData) {
	uint32_t CurSmpl;
	uint8_t nibble_shift;
	uint8_t nibble;
	
	O6258_signal = -2;
	O6258_step = 0;
	nibble_shift = 0;
	
	for (CurSmpl = 0; CurSmpl < SampleCnt; CurSmpl ++)
	{
		// Compute the new amplitude and update the current step */
		nibble = (InData[CurSmpl / 2] >> nibble_shift) & 0x0F;
		nibble_shift ^= 4;
		OutData[CurSmpl] = clock_adpcm(nibble);
	}
	
	return;
}

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		try {
			PDXLoader p(argv[i]);
			for(int j = 0; j < PDX_NUM_SAMPLES; j++) {
				if(p.samples[j].length > 0) {
					uint8_t *data = p.loadSample(j);
					int16_t *wavData = new int16_t[p.samples[j].length * 2];
					compute_tables();
					okim6258_decode(p.samples[j].length * 2, wavData, data);
					char buf[256];
					snprintf(buf, sizeof(buf), "%s-%d.wav", argv[i], j);
					WAVWriter::write(buf, wavData, p.samples[j].length * 2, 15600, 16, 1);
					delete[] data;
				}
			}
		} catch(exceptionf &e) {
			printf("Error loading %s: %s\n", argv[i], e.what());
		}
	}
}

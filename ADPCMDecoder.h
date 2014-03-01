#ifndef ADPCM_DECODER_H_
#define ADPCM_DECODER_H_

#include <math.h>
#include <stdint.h>

// ADPCM conversion code adapted from MAME
struct ADPCMDecoder {
	int diff_lookup[49*16];
	uint8_t output_bits;	// output_12bits ? 12 : 10
	int32_t signal;
	int32_t step;
	uint8_t signal_gain;

	ADPCMDecoder(uint8_t outputBits = 16, uint8_t signalGain = 6) {
		output_bits = outputBits;
		signal_gain = signalGain;
 		compute_tables();
	}

	void decode(uint8_t *input, uint32_t numSamples, int16_t *output) {
		uint32_t curSmpl;
		uint8_t nibble_shift;
		uint8_t nibble;

		signal = -2;
		step = 0;
		nibble_shift = 0;

		for(curSmpl = 0; curSmpl < numSamples; curSmpl ++) {
			// Compute the new amplitude and update the current step */
			nibble = (input[curSmpl / 2] >> nibble_shift) & 0x0F;
			nibble_shift ^= 4;
			output[curSmpl] = clock(nibble);
		}

		return;
	}

	void compute_tables(void) {
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

	int16_t clock(uint8_t nibble) {
		int32_t max = (1 << (output_bits - 1)) - 1;
		int32_t min = -(1 << (output_bits - 1));
		int32_t final;
		int index_shift[8] = { -1, -1, -1, -1, 2, 4, 6, 8 };

		signal += diff_lookup[step * 16 + (nibble & 15)];

		// clamp to the maximum
		if (signal > max) signal = max;
		else if (signal < min) signal = min;

		// adjust the step size and clamp
		step += index_shift[nibble & 7];
		if (step > 48) step = 48;
		else if (step < 0) step = 0;

		// return the signal scaled up to 32767
		// return signal << 4;
		final = signal << signal_gain;
		if (final < -0x8000) final = -0x8000;
		else if (final > 0x7FFF) final = 0x7FFF;
		return final;
	}
};

#endif /* ADPCM_DECODER_H_ */

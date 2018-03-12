#ifndef MDX_DRIVER_H_
#define MDX_DRIVER_H_

#include <stdint.h>

#include "mdx.h"
#include "adpcm_driver.h"

struct mdx_driver_channel {
	uint8_t *data;
	int len;
	int pos;

	int used;

	// state
	int ended, waiting;
	int ticks_remaining;
	int volume, opm_volume; // cache computed OPM volume

	uint8_t key_on_delay, key_on_delay_counter; // k
	int staccato, staccato_counter; // q
	int voice_num; // @

	int note, pan, pitch;
	int adpcm_freq_num;
	int16_t detune;
	int16_t portamento;
	int sample_pos;

	int skipNoteOff, skipNoteOn;
#define REPEAT_STACK_LIMIT 10
	int repeat_stack[REPEAT_STACK_LIMIT], repeat_stack_depth;

	// MP pitch LFO
	int lfo_delay, lfo_delay_counter;
	struct mdx_lfo pitch_lfo, amplitude_lfo;

	// MH OPM LFO
	uint8_t pms_ams, keysync, mhon;

};

struct mdx_driver {
	struct mdx_file *f;
	struct mdx_driver_channel channels[16];
	struct adpcm_mixer *adpcm_mixer;
	int channel_mask;

	int max_loops, cur_loop, loop_chan;

	uint8_t opm_cache[256]; // to avoid spamming duplicate commands

	void *data_ptr;
	void (*write_opm)(struct mdx_driver *r, uint8_t reg, uint8_t val, void *data_ptr);
	void (*set_tempo)(struct mdx_driver *r, int tempo, void *data_ptr);
	void (*tick_cb)(struct mdx_driver *r, void *data_ptr);

	int ended;
	int fade_rate, fade_counter, fade_value;
};

void mdx_driver_init(struct mdx_driver *r, struct mdx_file *f);
void mdx_driver_tick(struct mdx_driver *r);
void mdx_driver_start_fadeout(struct mdx_driver *r, int rate); // rate is expressed in ticks and represents the time between volume decrements

#endif /* MDX_DRIVER_H_ */

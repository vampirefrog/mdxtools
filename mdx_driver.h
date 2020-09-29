#ifndef MDX_DRIVER_H_
#define MDX_DRIVER_H_

#include <stdint.h>
#include "mdx.h"
#include "pdx.h"
#include "timer_driver.h"
#include "fm_driver.h"
#include "adpcm_driver.h"

struct mdx_driver_track {
	uint8_t *data;
	int len;
	int pos;

	int used;

	// state
	int ended, waiting, loop_num;
	int ticks_remaining;
	int volume, opm_volume; // cache computed OPM volume

	uint8_t key_on_delay, key_on_delay_counter; // k
	int staccato, staccato_counter; // q
	int voice_num; // @

	int note, pan, pitch;
	int adpcm_freq_num;
	int16_t detune;
	int16_t portamento;

	int skipNoteOff, skipNoteOn;

	// MDX LFOs
	int lfo_delay, lfo_delay_counter;
	struct mdx_lfo pitch_lfo, amplitude_lfo;

	// MH OPM LFO
	uint8_t pms_ams, keysync, mhon;
};

struct mdx_driver {
	struct mdx_file *mdx_file;
	struct pdx_file *pdx_file;

	struct timer_driver *timer_driver;
	struct fm_driver *fm_driver;
	struct adpcm_driver *adpcm_driver;

	struct mdx_driver_track tracks[16];

	int ended;
	int fade_rate, fade_counter, fade_value;
	int track_mask;
	int cur_loop, max_loops;

	void *data_ptr;
	void (*set_tempo)(struct mdx_driver *, int track_num, void *data_ptr);
	void (*unknown_command_cb)(struct mdx_driver *, int track_num, uint8_t cmd, void *data_ptr);
};

void mdx_driver_init(struct mdx_driver *driver, struct timer_driver *timer_driver, struct fm_driver *fm_driver, struct adpcm_driver *adpcm_driver);
void mdx_driver_deinit(struct mdx_driver *driver);
void mdx_driver_tick(struct mdx_driver *driver);
int mdx_driver_load(struct mdx_driver *driver, struct mdx_file *file, struct pdx_file *pfile);
int mdx_driver_start_fadeout(struct mdx_driver *driver, int fade_rate);

#endif /* MDX_DRIVER_H_ */

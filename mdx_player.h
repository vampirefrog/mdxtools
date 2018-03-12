#ifndef MDX_PLAYER_H_
#define MDX_PLAYER_H_

#include "mdx_driver.h"
#include "timer.h"

struct mdx_player {
	struct mdx_driver driver;

	struct timer opm_timer;
	int sample_rate;

	int opm_samples;
	int pcm_samples;
	int pcm_enable;

	void *data_ptr;
	void (*write_wait)(struct mdx_player *p, int wait, void *data_ptr);
	void (*set_adpcm_freq)(struct mdx_player *p, int clock, int divisor, void *data_ptr);
};
void mdx_player_start(struct mdx_player *p, struct mdx_file *f, int sample_rate);
void mdx_player_fade_out(struct mdx_player *p);
void mdx_player_stop(struct mdx_player *p);
void mdx_player_play(struct mdx_player *p, int samples);

#endif /* MDX_PLAYER_H_ */

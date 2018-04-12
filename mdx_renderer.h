#ifndef MDX_RENDERER_H_
#define MDX_RENDERER_H_

#include "mdx_player.h"
#include "mamedef.h"
#include "ym2151.h"
#include "adpcm_driver.h"

struct mdx_renderer {
	struct mdx_player player;

	stream_sample_t *bufL, *bufR, *chipBufL, *chipBufR;
	int num_buf_samples, cur_sample;

	struct ym2151 opm;
	int m6258_rate;

	struct adpcm_driver *adpcm_driver;
	// int adpcm_resample_numerator, adpcm_resample_denominator, adpcm_resample_remainder, adpcm_last_sample[2];
};

void mdx_renderer_init(struct mdx_renderer *r, struct mdx_file *f, struct adpcm_driver *d, int sample_rate, stream_sample_t *bufL, stream_sample_t *bufR, stream_sample_t *chipBufL, stream_sample_t *chipBufR, int num_buf_samples);
void mdx_renderer_end(struct mdx_renderer *r);
void mdx_renderer_render(struct mdx_renderer *r);

#endif /* MDX_RENDERER_H_ */

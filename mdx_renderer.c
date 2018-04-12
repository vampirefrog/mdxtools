#include "mdx_player.h"
#include "mdx_renderer.h"
#include "tools.h"

static void mdx_renderer_write_wait(struct mdx_player *p, int wait, void *data) {
	struct mdx_renderer *r = (struct mdx_renderer *)data;

	if(wait <= 0) return;

	// TODO: mix chipBuf and clip
	// TODO: try soft clip
	stream_sample_t *renderbuf[2] = { r->bufL + r->cur_sample, r->bufR + r->cur_sample };
	r->cur_sample += wait;

	ym2151_update_one(&r->opm, renderbuf, wait);

	adpcm_driver_run(r->adpcm_driver, r->chipBufL, r->chipBufR, wait);
	for(int i = 0; i < wait; i++) {
		renderbuf[0][i] += r->chipBufL[i];
		renderbuf[1][i] += r->chipBufR[i];
	}

	// int num_adpcm_samples = (r->adpcm_resample_remainder + wait * r->adpcm_resample_numerator) / r->adpcm_resample_denominator;
	// stream_sample_t *adpcm_buf[2] = { r->chipBufL, r->chipBufR };
	// if(r->adpcm_resample_remainder > 0) {
	// 	*r->chipBufL = r->adpcm_last_sample[0];
	// 	*r->chipBufR = r->adpcm_last_sample[1];
	// 	adpcm_buf[0] = &r->chipBufL[1];
	// 	adpcm_buf[1] = &r->chipBufR[1];
	// }
	// okim6258_update(0, adpcm_buf, num_adpcm_samples);
	// int numerator = r->adpcm_resample_remainder;
	// for(int i = 0; i < wait; i++) {
	// 	int adpcm_sample = numerator / r->adpcm_resample_denominator;
	// 	renderbuf[0][i] += r->chipBufL[adpcm_sample];
	// 	renderbuf[1][i] += r->chipBufR[adpcm_sample];
	// 	r->adpcm_last_sample[0] = r->chipBufL[adpcm_sample];
	// 	r->adpcm_last_sample[1] = r->chipBufR[adpcm_sample];
	// 	numerator += r->adpcm_resample_numerator;
	// }
	// r->adpcm_resample_remainder = numerator - num_adpcm_samples * r->adpcm_resample_denominator;
}

static void mdx_renderer_write_opm(struct mdx_driver *d, uint8_t reg, uint8_t val, void *data_ptr) {
	struct mdx_renderer *r = (struct mdx_renderer *)data_ptr;
	ym2151_write_reg(&r->opm, reg, val);
}

// static void mdx_renderer_write_oki(struct adpcm_driver *d, uint8_t port, uint8_t data, void *data_ptr) {
// //	struct mdx_renderer *r = (struct mdx_renderer *)data_ptr;
// 	// printf("oki write 0x%02x 0x%02x\n", port, data);
// 	okim6258_write(0, port, data);
// }

// static void mdx_renderer_set_adpcm_freq(struct mdx_player *p, int clock, int divisor, void *data) {
// 	// printf("mdx_renderer_set_adpcm_freq clock=%d divisor=%d\n", clock, divisor);
// 	struct mdx_renderer *r = (struct mdx_renderer *)data;
// 	int d = gcd(clock, divisor * p->sample_rate);
// 	r->adpcm_resample_numerator = clock / d;
// 	r->adpcm_resample_denominator = divisor * p->sample_rate / d;
// }

void mdx_renderer_init(
	struct mdx_renderer *r, struct mdx_file *f, struct adpcm_driver *d,
	int sample_rate, stream_sample_t *bufL, stream_sample_t *bufR,
	stream_sample_t *chipBufL, stream_sample_t *chipBufR, int num_buf_samples
) {
	r->bufL = bufL;
	r->bufR = bufR;
	r->chipBufL = chipBufL;
	r->chipBufR = chipBufR;
	r->num_buf_samples = num_buf_samples;

	ym2151_init(&r->opm, 4000000, sample_rate);
	ym2151_reset_chip(&r->opm);

	// r->m6258_rate = device_start_okim6258(0, 8000000, FOSC_DIV_BY_512, TYPE_4BITS, OUTPUT_12BITS);
	// device_reset_okim6258(0);

	r->player.data_ptr = r;
	r->player.write_wait = mdx_renderer_write_wait;
	r->player.driver.write_opm = mdx_renderer_write_opm;
	r->player.pcm_enable = 1;
	r->player.data_ptr = r;
	r->adpcm_driver = d;
	r->player.driver.adpcm_mixer = &d->mixer;

	mdx_player_start(&r->player, f, sample_rate);
}

void mdx_renderer_render(struct mdx_renderer *r) {
	r->cur_sample = 0;
	mdx_player_play(&r->player, r->num_buf_samples);
}

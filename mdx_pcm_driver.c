#include "mdx_pcm_driver.h"

static void mdx_pcm_driver_set_tempo(struct mdx_driver *d, int tempo, void *data_ptr) {
	struct mdx_pcm_driver *p = (struct mdx_pcm_driver *)data_ptr;
	timer_set_opm_tempo(&p->opm_timer, OPM_CLOCK, p->sample_rate, tempo);
}

// static void mdx_pcm_driver_set_adpcm_freq(struct adpcm_driver *d, int clock, int divisor, void *data_ptr) {
// 	struct mdx_pcm_driver *p = (struct mdx_pcm_driver *)data_ptr;
// 	timer_set_adpcm_freq(&p->adpcm_timer, p->sample_rate, clock / 2, divisor);
// 	if(p->set_adpcm_freq)
// 		p->set_adpcm_freq(p, clock, divisor, p->data_ptr);
// }

void mdx_pcm_driver_start(struct mdx_pcm_driver *p, struct mdx_file *f, int sample_rate) {
	p->sample_rate = sample_rate;

	timer_init(&p->opm_timer);
	timer_set_opm_tempo(&p->opm_timer, OPM_CLOCK, sample_rate, 200);
	p->opm_samples = 0;

	// timer_init(&p->adpcm_timer);
	// timer_set_adpcm_freq(&p->adpcm_timer, sample_rate, 8000000 / 2, 512);
	// p->pcm_samples = 0;

	p->driver.data_ptr = p;
	p->driver.set_tempo = mdx_pcm_driver_set_tempo;
	// p->driver.adpcm_driver.rate_changed = mdx_pcm_driver_set_adpcm_freq;
	p->pcm_enable = 1;
	mdx_driver_init((struct mdx_driver *)&p->driver, f);
}

void mdx_pcm_driver_end(struct mdx_pcm_driver *p) {
}

void mdx_pcm_driver_play(struct mdx_pcm_driver *p, int samples) {
	int remaining_samples = samples;
	while(remaining_samples > 0) {
		int min = 0x7fffffff;
		if(p->opm_samples < min) min = p->opm_samples;
		// if(p->pcm_enable && p->driver.adpcm_driver.running && p->pcm_samples < min) min = p->pcm_samples;
		if(remaining_samples <= min) {
			if(p->write_wait)
				p->write_wait(p, remaining_samples, p->data_ptr);
			// if(p->pcm_enable && p->driver.adpcm_driver.running) p->pcm_samples -= remaining_samples;
			p->opm_samples -= remaining_samples;
			remaining_samples = 0;
		} else {
			if(p->write_wait)
				p->write_wait(p, min, p->data_ptr);
			remaining_samples -= min;
			// if(p->pcm_enable && p->driver.adpcm_driver.running) p->pcm_samples -= min;
			p->opm_samples -= min;
			if(p->opm_samples == 0) {
				mdx_driver_tick(&p->driver);
				p->opm_samples = timer_tick(&p->opm_timer);
			}
			// if(p->pcm_enable && p->driver.adpcm_driver.running && p->pcm_samples == 0) {
			// 	adpcm_driver_tick(&p->driver.adpcm_driver);
			// 	p->pcm_samples = timer_tick(&p->adpcm_timer);
			// }
		}
	}
}

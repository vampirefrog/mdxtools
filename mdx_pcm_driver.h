#ifndef MDX_PCM_DRIVER_H_
#define MDX_PCM_DRIVER_H_

#include "mdx_driver.h"
#include "timer.h"

struct mdx_pcm_driver {
	struct mdx_driver driver;

	struct timer opm_timer;
	int sample_rate;

	int opm_samples;
	int pcm_samples;
	int pcm_enable;

	void *data_ptr;
	void (*write_wait)(struct mdx_pcm_driver *p, int wait, void *data_ptr);
	void (*set_adpcm_freq)(struct mdx_pcm_driver *p, int clock, int divisor, void *data_ptr);
};

void mdx_pcm_driver_init(struct mdx_pcm_driver *driver, struct mdx_file *file, struct pdx_file *pfile, struct fm_driver *fm_driver, struct adpcm_driver *adpcm_driver, int sample_rate);
void mdx_pcm_driver_deinit(struct mdx_pcm_driver *driver);

#endif /* MDX_PCM_DRIVER_H_ */

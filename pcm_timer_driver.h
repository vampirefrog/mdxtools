#ifndef PCM_TIMER_DRIVER_H_
#define PCM_TIMER_DRIVER_H_

#include "timer_driver.h"

struct pcm_timer_driver {
	struct timer_driver timer_driver;
	int sample_rate;
	int numerator, denominator, remainder;
};
int pcm_timer_driver_init(struct pcm_timer_driver *driver, int sample_rate);
void pcm_timer_driver_deinit(struct pcm_timer_driver *driver);
int pcm_timer_driver_estimate(struct pcm_timer_driver *driver, int samples);
int pcm_timer_driver_advance(struct pcm_timer_driver *driver, int samples);

#endif /* PCM_TIMER_DRIVER_H_ */

#ifndef TIMER_DRIVER_
#define TIMER_DRIVER_

struct timer_driver;

typedef void (*timer_driver_tick_callback)(struct timer_driver *, void *data_ptr);
struct timer_driver {
	void *data_ptr;
	timer_driver_tick_callback tick;
	void (*set_opm_tempo)(struct timer_driver *driver, int opm_timer);
};

int timer_driver_init(struct timer_driver *driver);
void timer_driver_deinit(struct timer_driver *driver);
int timer_driver_set_tick_callback(struct timer_driver *driver, timer_driver_tick_callback tick, void *data_ptr);
void timer_driver_set_opm_tempo(struct timer_driver *driver, int opm_timer);

struct pcm_timer_driver {
	struct timer_driver timer_driver;
	int sample_rate;
	int numerator, denominator, remainder;
};
int pcm_timer_driver_init(struct pcm_timer_driver *driver, int sample_rate);
void pcm_timer_driver_deinit(struct pcm_timer_driver *driver);
int pcm_timer_driver_estimate(struct pcm_timer_driver *driver, int samples);
int pcm_timer_driver_advance(struct pcm_timer_driver *driver, int samples);

#endif /* TIMER_DRIVER_ */

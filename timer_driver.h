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

#endif /* TIMER_DRIVER_ */

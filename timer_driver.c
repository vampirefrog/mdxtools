#include <string.h>

#include <stdio.h>

#include "timer_driver.h"

int timer_driver_init(struct timer_driver *driver) {
	memset(driver, 0, sizeof(*driver));

	return 0;
}

void timer_driver_deinit(struct timer_driver *driver) {
	memset(driver, 0, sizeof(*driver));
}

int timer_driver_set_tick_callback(struct timer_driver *driver, timer_driver_tick_callback tick, void *data_ptr) {
	driver->data_ptr = data_ptr;
	driver->tick = tick;

	return 0;
}

void timer_driver_set_opm_tempo(struct timer_driver *driver, int opm_timer) {
	if(driver->set_opm_tempo)
		driver->set_opm_tempo(driver, opm_timer);
}

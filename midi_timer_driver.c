#include "midi_timer_driver.h"

static void midi_timer_driver_set_opm_tempo(struct timer_driver *driver, int opm_tempo) {

}

int midi_timer_driver_init(struct midi_timer_driver *driver) {
	timer_driver_init(&driver->timer_driver);
	driver->ticks_per_quarter_note = 48;
	driver->timer_driver.set_opm_tempo = midi_timer_driver_set_opm_tempo;
}

void midi_timer_driver_deinit(struct midi_timer_driver *driver) {
}

int midi_timer_driver_tick(struct midi_timer_driver *driver) {
	if(!driver->timer_driver.tick) return -1;
	driver->timer_driver.tick((struct timer_driver *)&driver, driver->timer_driver.data_ptr);
	return 0;
}

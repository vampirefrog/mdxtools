#ifndef MIDI_TIMER_DRIVER_H_
#define MIDI_TIMER_DRIVER_H_

#include "timer_driver.h"

struct midi_timer_driver {
	struct timer_driver timer_driver;
	int ticks_per_quarter_note;
};
int midi_timer_driver_init(struct midi_timer_driver *driver);
void midi_timer_driver_deinit(struct midi_timer_driver *driver);
int midi_timer_driver_tick(struct midi_timer_driver *driver);

#endif /* MIDI_TIMER_DRIVER_H_ */

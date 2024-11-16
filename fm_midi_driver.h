#ifndef FM_MIDI_DRIVER_H_
#define FM_MIDI_DRIVER_H_

#include "fm_driver.h"
#include "midilib/midi_file.h"

struct fm_midi_driver_channel {
	int on, pitch, tl, ticks;
};

/* MIDI driver */
struct fm_midi_driver {
	struct fm_driver fm_driver;
	struct midi_file *midi_file;
	struct fm_midi_driver_channel channels[8];
};
void fm_midi_driver_init(struct fm_midi_driver *driver, struct midi_file *midi_file);
void fm_midi_driver_deinit(struct fm_midi_driver *driver);
void fm_midi_driver_tick(struct fm_midi_driver *driver);

#endif /* FM_MIDI_DRIVER_H_ */

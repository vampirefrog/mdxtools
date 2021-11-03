#ifndef FM_OPM_MIDI_DRIVER_H_
#define FM_OPM_MIDI_DRIVER_H_

#include "fm_opm_driver.h"

/* MIDI driver */
struct fm_opm_midi_driver {
	struct fm_opm_driver fm_opm_driver;
};
void fm_opm_midi_driver_init(struct fm_opm_midi_driver *driver);
void fm_opm_midi_driver_deinit(struct fm_opm_midi_driver *driver);

#endif /* FM_OPM_MIDI_DRIVER_H_ */

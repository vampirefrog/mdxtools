#ifndef ADPCM_MIDI_DRIVER_H_
#define ADPCM_MIDI_DRIVER_H_

#include "adpcm_driver.h"
#include "midilib/midi_file.h"

struct adpcm_midi_driver_channel {
	int ticks;
};

struct adpcm_midi_driver {
	struct adpcm_driver adpcm_driver; // parent
	struct midi_file *midi_file;
	struct adpcm_midi_driver_channel channels[8];
};
int adpcm_midi_driver_init(struct adpcm_midi_driver *driver, struct midi_file *midi_file);
int adpcm_midi_driver_tick(struct adpcm_midi_driver *driver);
void adpcm_midi_driver_end(struct adpcm_midi_driver *driver);

#endif /* ADPCM_MIDI_DRIVER_H_ */

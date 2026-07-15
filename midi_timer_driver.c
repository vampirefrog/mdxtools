#include "midi_timer_driver.h"
#include <stdio.h>

static void midi_timer_driver_set_opm_tempo(struct timer_driver *driver, int opm_tempo) {
	struct midi_timer_driver *mididrv = (struct midi_timer_driver *)driver;
	
	uint32_t BPM = 60 * 4000000 / (48 * 1024 * (256 - opm_tempo)); // Convert opm_tempo to Beats per Minute
	printf("opm_tempo: %d, BPM: %d\n", opm_tempo, BPM);
	
	if (mididrv->tempoBPM != BPM) {
		midi_track_write_tempo(&mididrv->midi_file->tracks[0], mididrv->ticks, 60000000 / BPM); // the tempo function takes microseconds per quarter note.
		mididrv->ticks = 0;
		mididrv->tempoBPM = BPM;
	}
}

int midi_timer_driver_init(struct midi_timer_driver *driver, struct midi_file *midi_file) {
	printf("Run midi_timer_driver_init\n");
	timer_driver_init(&driver->timer_driver);
	driver->ticks_per_quarter_note = 48;
	driver->timer_driver.set_opm_tempo = midi_timer_driver_set_opm_tempo;
	driver->ticks = 0;
	driver->midi_file = midi_file;
	driver->tempoBPM=0;
	return 0;
}

void midi_timer_driver_deinit(struct midi_timer_driver *driver) {
}

int midi_timer_driver_tick(struct midi_timer_driver *driver) {
	
	if(!driver->timer_driver.tick) return -1;
	driver->timer_driver.tick((struct timer_driver *)&driver, driver->timer_driver.data_ptr);
	
	driver->ticks++;
	return 0;
}

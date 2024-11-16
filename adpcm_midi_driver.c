#include "adpcm_midi_driver.h"

int adpcm_midi_driver_play(struct adpcm_driver *d, uint8_t channel, uint8_t *data, int len, uint8_t freq, uint8_t vol) {
	return 0;
}

int adpcm_midi_driver_stop(struct adpcm_driver *d, uint8_t channel) {
	return 0;
}

int adpcm_midi_driver_set_volume(struct adpcm_driver *d, uint8_t channel, uint8_t vol) {
	return 0;
}

int adpcm_midi_driver_set_freq(struct adpcm_driver *d, uint8_t channel, uint8_t freq) {
	return 0;
}

int adpcm_midi_driver_set_pan(struct adpcm_driver *d, uint8_t pan) {
	return 0;
}

int adpcm_midi_driver_init(struct adpcm_midi_driver *driver, struct midi_file *midi_file) {
	adpcm_driver_init(&driver->adpcm_driver);
	driver->adpcm_driver.play = adpcm_midi_driver_play;
	driver->adpcm_driver.stop = adpcm_midi_driver_stop;
	driver->adpcm_driver.set_freq = adpcm_midi_driver_set_freq;
	driver->adpcm_driver.set_volume = adpcm_midi_driver_set_volume;
	driver->adpcm_driver.set_pan = adpcm_midi_driver_set_pan;

	driver->midi_file = midi_file;
	for(int i = 0; i < 8; i++)
		driver->channels[i].ticks = 0;

	return 0;
}

void adpcm_midi_driver_end(struct adpcm_midi_driver *driver) {
	for(int i = 0; i < 8; i++) {
		midi_track_write_track_end(&driver->midi_file->tracks[i + 9], driver->channels[i].ticks);
		driver->channels[i].ticks = 0;
	}
}

int adpcm_midi_driver_tick(struct adpcm_midi_driver *driver) {
	for(int i = 0; i < 8; i++)
		driver->channels[i].ticks++;
	return 0;
}

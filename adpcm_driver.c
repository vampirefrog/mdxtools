#include "adpcm_driver.h"

void adpcm_driver_init(struct adpcm_driver *driver) {
	driver->play = 0;
	driver->stop = 0;
	driver->set_volume = 0;
	driver->set_volume = 0;
	driver->set_pan = 0;

	driver->pan = 3;
}

int adpcm_driver_play(struct adpcm_driver *d, uint8_t channel, uint8_t *data, int len, uint8_t freq, uint8_t vol) {
	if(d->play)
		return d->play(d, channel, data, len, freq, vol);
	return 0;
}

int adpcm_driver_stop(struct adpcm_driver *d, uint8_t channel) {
	if(d->stop)
		d->stop(d, channel);
	return 0;
}

int adpcm_driver_set_freq(struct adpcm_driver *d, uint8_t channel, uint8_t freq_num) {
	if(d->set_volume)
		d->set_freq(d, channel, freq_num);
	return 0;
}

int adpcm_driver_set_volume(struct adpcm_driver *d, uint8_t channel, uint8_t vol) {
	if(d->set_volume)
		d->set_volume(d, channel, vol);
	return 0;
}

int adpcm_driver_set_pan(struct adpcm_driver *d, uint8_t pan) {
	if(d->set_pan)
		d->set_pan(d, pan);
	return 0;
}


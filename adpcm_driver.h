#ifndef ADPCM_DRIVER_H_
#define ADPCM_DRIVER_H_

#include <stdint.h>

struct adpcm_driver {
	uint8_t pan;

	int (*play)(struct adpcm_driver *d, uint8_t channel, uint8_t *data, int len, uint8_t freq, uint8_t vol);
	int (*stop)(struct adpcm_driver *d, uint8_t channel);
	int (*set_volume)(struct adpcm_driver *d, uint8_t channel, uint8_t vol);
	int (*set_freq)(struct adpcm_driver *d, uint8_t channel, uint8_t freq);
	int (*set_pan)(struct adpcm_driver *d, uint8_t pan);
};

void adpcm_driver_init(struct adpcm_driver *driver);
int adpcm_driver_play(struct adpcm_driver *d, uint8_t channel, uint8_t *data, int len, uint8_t freq, uint8_t vol);
int adpcm_driver_stop(struct adpcm_driver *d, uint8_t channel);
int adpcm_driver_set_freq(struct adpcm_driver *d, uint8_t channel, uint8_t freq);
int adpcm_driver_set_volume(struct adpcm_driver *d, uint8_t channel, uint8_t vol);
int adpcm_driver_set_pan(struct adpcm_driver *d, uint8_t pan);

#endif /* ADPCM_DRIVER_H_ */

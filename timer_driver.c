#include <string.h>

#include <stdio.h>

#include "timer_driver.h"
#include "tools.h"

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

static void pcm_timer_driver_set_opm_tempo(struct timer_driver *driver, int opm_tempo) {
	struct pcm_timer_driver *pdriver = (struct pcm_timer_driver *)driver;

	int
		clock = 4000000,
		d = gcd(clock, 1024),
		c1 = clock / d,
		c2 = 1024 / d,
		s = pdriver->sample_rate * c2,
		d2 = gcd(c1, s);

	pdriver->numerator = s * (256 - opm_tempo) / d2;
	pdriver->denominator = c1 / d2;
}

int pcm_timer_driver_init(struct pcm_timer_driver *driver, int sample_rate) {
	timer_driver_init(&driver->timer_driver);
	driver->sample_rate = sample_rate;
	driver->numerator = driver->denominator = driver->remainder = 0;
	driver->timer_driver.set_opm_tempo = pcm_timer_driver_set_opm_tempo;

	return 0;
}

void pcm_timer_driver_deinit(struct pcm_timer_driver *driver) {
	timer_driver_deinit(&driver->timer_driver);

	driver->numerator = driver->denominator = driver->remainder = 0;
}

int pcm_timer_driver_estimate(struct pcm_timer_driver *driver, int samples) {
	int denom = driver->remainder;
	for(int i = 1; i <= samples; i++) {
		denom += driver->denominator;
		if(denom >= driver->numerator) {
			return i;
		}
	}

	return samples;
}

int pcm_timer_driver_advance(struct pcm_timer_driver *driver, int samples) {
	int denom = driver->remainder;
	int ticks = 0;
	for(int i = 1; i <= samples; i++) {
		denom += driver->denominator;
		if(denom >= driver->numerator) {
			if(driver->timer_driver.tick) {
				driver->timer_driver.tick((struct timer_driver *)&driver, driver->timer_driver.data_ptr);
			}
			ticks++;
			denom -= driver->numerator;
		}
	}
	driver->remainder = denom;

	return ticks;
}

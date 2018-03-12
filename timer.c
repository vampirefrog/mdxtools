#include "timer.h"
#include "tools.h"

void timer_init(struct timer *t) {
	t->remainder = 0;
	t->denominator = 625;
	t->numerator = 0;
}

void timer_set_opm_tempo(struct timer *t, int clock, int sample_rate, int tempo) {
	int
		d = gcd(clock, 1024),
		c1 = clock / d,
		c2 = 1024 / d,
		s = sample_rate * c2,
		d2 = gcd(c1, s);

	t->numerator = s * (256 - tempo) / d2;
	t->denominator = c1 / d2;
}

void timer_set_adpcm_freq(struct timer *t, int sample_rate, int adpcm_clock, int adpcm_div) {
	int
		d = gcd(adpcm_clock, adpcm_div),
		c1 = adpcm_clock / d,
		c2 = adpcm_div / d,
		s = sample_rate * c2,
		d2 = gcd(c1, s);

	t->numerator = s / d2;
	t->denominator = c1 / d2;
}

int timer_tick(struct timer *t) {
	int numerator = t->numerator + t->remainder;
	int next_tick = numerator / t->denominator;
	t->remainder = numerator - next_tick * t->denominator;
	return next_tick;
}

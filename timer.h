#ifndef TIMER_H_
#define TIMER_H_

struct timer {
	int numerator, denominator, remainder;
};

void timer_init(struct timer *t);
void timer_set_opm_tempo(struct timer *t, int clock, int sample_rate, int tempo);
void timer_set_adpcm_freq(struct timer *t, int sample_rate, int adpcm_clock, int adpcm_div);
int timer_tick(struct timer *t);

#endif /* TIMER_H_ */

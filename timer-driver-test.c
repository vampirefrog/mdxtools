#include <stdio.h>

#include "timer_driver.h"

int main(int argc, char **argv) {
	struct pcm_timer_driver driver;

	pcm_timer_driver_init(&driver, 44100);
	int bpm_tempo = 120;
	int opm_tempo = 256 - (78125 / (16 * bpm_tempo));
	pcm_timer_driver_set_opm_tempo(&driver, 4000000, opm_tempo);
	for(int i = 0; i < 5; i++) {
		int samples = 4096;
		while(samples > 0) {
			int estimation = pcm_timer_driver_estimate(&driver, samples);
			int ticks = pcm_timer_driver_advance(&driver, estimation);
			samples -= estimation;
			printf("i=%d samples=%d estimation=%d ticks=%d\n", i, 4096 - samples, estimation, ticks);
		}
	}

	return 0;
}

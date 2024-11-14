#include "fm_opm_emu_driver.h"

static void fm_opm_emu_driver_write(struct fm_opm_driver *driver, uint8_t reg, uint8_t val) {
	struct fm_opm_emu_driver *emudrv = (struct fm_opm_emu_driver *)driver;

	ym2151_write_reg(&emudrv->opm, reg, val);
}

void fm_opm_emu_driver_init(struct fm_opm_emu_driver *driver, struct vgm_logger *vgm_logger, int sample_rate) {
	driver->sample_rate = sample_rate;
	ym2151_init(&driver->opm, 4000000, sample_rate);
	ym2151_reset_chip(&driver->opm);
	driver->fm_opm_driver.write = fm_opm_emu_driver_write;

	fm_opm_driver_init(&driver->fm_opm_driver, vgm_logger);
}

void fm_opm_emu_driver_deinit(struct fm_opm_emu_driver *driver) {
	ym2151_shutdown(&driver->opm);
}

int fm_opm_emu_driver_estimate(struct fm_opm_emu_driver *d, int num_samples) {
	return num_samples;
}

void fm_opm_emu_driver_run(struct fm_opm_emu_driver *d, stream_sample_t *outL, stream_sample_t *outR, int num_samples) {
	SAMP *buffers[2] = { outL, outR };
	ym2151_update_one(&d->opm, buffers, num_samples);
	for(int i = 0; i < num_samples; i++) {
		outL[i] = outL[i] / 2;
		outR[i] = outR[i] / 2;
	}
}

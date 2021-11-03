#ifndef FM_OPM_EMU_DRIVER_H_
#define FM_OPM_EMU_DRIVER_H_

#include "fm_opm_driver.h"
#include "ym2151.h"

/* OPM emulation driver */
struct fm_opm_emu_driver {
	struct fm_opm_driver fm_opm_driver;

	int sample_rate;
	struct ym2151 opm;
};
void fm_opm_emu_driver_init(struct fm_opm_emu_driver *driver, int sample_rate);
void fm_opm_emu_driver_deinit(struct fm_opm_emu_driver *driver);
int fm_opm_emu_driver_estimate(struct fm_opm_emu_driver *d, int num_samples);
void fm_opm_emu_driver_run(struct fm_opm_emu_driver *d, stream_sample_t *outL, stream_sample_t *outR, int num_samples);

#endif /* FM_OPM_EMU_DRIVER_H_ */

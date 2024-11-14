#ifndef FM_OPM_DRIVER_H_
#define FM_OPM_DRIVER_H_

#include "fm_driver.h"
#include "vgm_logger.h"

/* OPM driver */
struct fm_opm_driver {
	struct fm_driver fm_driver;
	struct vgm_logger *vgm_logger;

	uint8_t opm_cache[256];

	void (*write)(struct fm_opm_driver *driver, uint8_t reg, uint8_t val);
};
void fm_opm_driver_init(struct fm_opm_driver *driver, struct vgm_logger *vgm_logger);
void fm_opm_driver_write(struct fm_opm_driver *driver, uint8_t reg, uint8_t val);

#endif /* FM_OPM_DRIVER_H_ */

#ifndef FM_DRIVER_H_
#define FM_DRIVER_H_

#include <stdint.h>

struct fm_driver {
	void *data_ptr;

	void (*reset_key_sync)(struct fm_driver *driver, int channel);
	void (*set_pms_ams)(struct fm_driver *driver, int channel, uint8_t pms_ams);
	void (*set_pitch)(struct fm_driver *driver, int channel, int pitch);
	void (*set_tl)(struct fm_driver *driver, int channel, uint8_t tl, uint8_t *v);
	void (*note_on)(struct fm_driver *driver, int channel, uint8_t op_mask, uint8_t *v);
	void (*note_off)(struct fm_driver *driver, int channel);
	void (*write_opm_reg)(struct fm_driver *driver, uint8_t reg, uint8_t data);
	void (*set_pan)(struct fm_driver *driver, int channel, uint8_t pan, uint8_t *v);
	void (*set_noise_freq)(struct fm_driver *driver, int channel, int freq);
	void (*load_voice)(struct fm_driver *driver, int channel, uint8_t *v, int voice_num, int opm_volume, int pan);
	void (*load_lfo)(struct fm_driver *driver, int channel, uint8_t wave, uint8_t freq, uint8_t pmd, uint8_t amd);
};
void fm_driver_init(struct fm_driver *driver);
void fm_driver_deinit(struct fm_driver *driver);
void fm_driver_reset_key_sync(struct fm_driver *driver, int channel);
void fm_driver_set_pms_ams(struct fm_driver *driver, int channel, uint8_t pms_ams);
void fm_driver_set_pitch(struct fm_driver *driver, int channel, int pitch);
void fm_driver_set_tl(struct fm_driver *driver, int channel, uint8_t tl, uint8_t *v);
void fm_driver_note_on(struct fm_driver *driver, int channel, uint8_t op_mask, uint8_t *v);
void fm_driver_note_off(struct fm_driver *driver, int channel);
void fm_driver_write_opm_reg(struct fm_driver *driver, uint8_t reg, uint8_t val);
void fm_driver_set_pan(struct fm_driver *driver, int channel, uint8_t pan, uint8_t *v);
void fm_driver_set_noise_freq(struct fm_driver *driver, int channel, int freq);
void fm_driver_load_voice(struct fm_driver *driver, int channel, uint8_t *v, int voice_num, int opm_volume, int pan);
void fm_driver_load_lfo(struct fm_driver *driver, int channel, uint8_t wave, uint8_t freq, uint8_t pmd, uint8_t amd);

#endif /* FM_DRIVER_H_ */

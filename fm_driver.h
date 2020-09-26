#ifndef FM_DRIVER_H_
#define FM_DRIVER_H_

#include "ym2151.h"

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
	void (*load_voice)(struct fm_driver *driver, int channel, uint8_t *v, int opm_volume, int pan);
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
void fm_driver_load_voice(struct fm_driver *driver, int channel, uint8_t *v, int opm_volume, int pan);
void fm_driver_load_lfo(struct fm_driver *driver, int channel, uint8_t wave, uint8_t freq, uint8_t pmd, uint8_t amd);

/* OPM driver */
struct fm_opm_driver {
	struct fm_driver fm_driver;

	uint8_t opm_cache[256];

	void (*write)(struct fm_opm_driver *driver, uint8_t reg, uint8_t val);
};
void fm_opm_driver_write(struct fm_opm_driver *driver, uint8_t reg, uint8_t val);

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

/* MIDI driver */
struct fm_opm_midi_driver {
	struct fm_driver fm_driver;
};
void fm_opm_midi_driver_init(struct fm_opm_midi_driver *driver);
void fm_opm_midi_driver_deinit(struct fm_opm_midi_driver *driver);

#endif /* FM_DRIVER_H_ */

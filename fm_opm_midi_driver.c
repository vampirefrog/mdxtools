#include "fm_opm_midi_driver.h"

void fm_opm_midi_driver_reset_key_sync(struct fm_driver *driver, int channel) {
	struct fm_opm_midi_driver *mididrv = (struct fm_opm_midi_driver *)driver;
	(void)mididrv;
}

void fm_opm_midi_driver_set_pms_ams(struct fm_driver *driver, int channel, uint8_t pms_ams) {
	struct fm_opm_midi_driver *mididrv = (struct fm_opm_midi_driver *)driver;
	(void)mididrv;
}

void fm_opm_midi_driver_set_pitch(struct fm_driver *driver, int channel, int pitch) {
	struct fm_opm_midi_driver *mididrv = (struct fm_opm_midi_driver *)driver;
	(void)mididrv;
}

void fm_opm_midi_driver_set_tl(struct fm_driver *driver, int channel, uint8_t tl, uint8_t *v) {
	struct fm_opm_midi_driver *mididrv = (struct fm_opm_midi_driver *)driver;
	(void)mididrv;
}

void fm_opm_midi_driver_note_on(struct fm_driver *driver, int channel, uint8_t op_mask, uint8_t *v) {
	struct fm_opm_midi_driver *mididrv = (struct fm_opm_midi_driver *)driver;
	(void)mididrv;
}

void fm_opm_midi_driver_note_off(struct fm_driver *driver, int channel) {
	struct fm_opm_midi_driver *mididrv = (struct fm_opm_midi_driver *)driver;
	(void)mididrv;
}

void fm_opm_midi_driver_write_opm_reg(struct fm_driver *driver, uint8_t reg, uint8_t data) {
	struct fm_opm_midi_driver *mididrv = (struct fm_opm_midi_driver *)driver;
	(void)mididrv;
}

void fm_opm_midi_driver_set_pan(struct fm_driver *driver, int channel, uint8_t pan, uint8_t *v) {
	struct fm_opm_midi_driver *mididrv = (struct fm_opm_midi_driver *)driver;
	(void)mididrv;
}

void fm_opm_midi_driver_set_noise_freq(struct fm_driver *driver, int channel, int freq) {
	struct fm_opm_midi_driver *mididrv = (struct fm_opm_midi_driver *)driver;
	(void)mididrv;
}

void fm_opm_midi_driver_load_voice(struct fm_driver *driver, int channel, uint8_t *v, int opm_volume, int pan) {
	struct fm_opm_midi_driver *mididrv = (struct fm_opm_midi_driver *)driver;
	(void)mididrv;
}

void fm_opm_midi_driver_load_lfo(struct fm_driver *driver, int channel, uint8_t wave, uint8_t freq, uint8_t pmd, uint8_t amd) {
	struct fm_opm_midi_driver *mididrv = (struct fm_opm_midi_driver *)driver;
	(void)mididrv;
}

void fm_opm_midi_driver_init(struct fm_opm_midi_driver *driver) {
	driver->fm_driver.reset_key_sync = fm_opm_midi_driver_reset_key_sync;
	driver->fm_driver.set_pms_ams    = fm_opm_midi_driver_set_pms_ams;
	driver->fm_driver.set_pitch      = fm_opm_midi_driver_set_pitch;
	driver->fm_driver.set_tl         = fm_opm_midi_driver_set_tl;
	driver->fm_driver.note_on        = fm_opm_midi_driver_note_on;
	driver->fm_driver.note_off       = fm_opm_midi_driver_note_off;
	driver->fm_driver.write_opm_reg  = fm_opm_midi_driver_write_opm_reg;
	driver->fm_driver.set_pan        = fm_opm_midi_driver_set_pan;
	driver->fm_driver.set_noise_freq = fm_opm_midi_driver_set_noise_freq;
	driver->fm_driver.load_voice     = fm_opm_midi_driver_load_voice;
	driver->fm_driver.load_lfo       = fm_opm_midi_driver_load_lfo;

	fm_opm_driver_init(&driver->fm_opm_driver);
}

void fm_opm_midi_driver_deinit(struct fm_opm_midi_driver *driver) {

}

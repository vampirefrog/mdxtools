#include "fm_driver.h"
#include "ym2151.h"

void fm_driver_init(struct fm_driver *driver) {
	driver->reset_key_sync = 0;
	driver->set_pms_ams = 0;
	driver->set_pitch = 0;
	driver->set_tl = 0;
	driver->note_on = 0;
	driver->note_off = 0;
	driver->write_opm_reg = 0;
	driver->set_pan = 0;
	driver->set_noise_freq = 0;
	driver->load_voice = 0;
	driver->load_lfo = 0;
}

void fm_driver_reset_key_sync(struct fm_driver *driver, int channel) {
	if(driver->reset_key_sync)
		driver->reset_key_sync(driver, channel);
}

void fm_driver_set_pms_ams(struct fm_driver *driver, int channel, uint8_t pms_ams) {
	if(driver->set_pms_ams)
		driver->set_pms_ams(driver, channel, pms_ams);
}

void fm_driver_set_pitch(struct fm_driver *driver, int channel, int pitch) {
	if(driver->set_pitch)
		driver->set_pitch(driver, channel, pitch);
}

void fm_driver_set_tl(struct fm_driver *driver, int channel, uint8_t tl, uint8_t *v) {
	if(driver->set_tl)
		driver->set_tl(driver, channel, tl, v);
}

void fm_driver_note_on(struct fm_driver *driver, int channel, uint8_t op_mask, uint8_t *v) {
	if(driver->note_on)
		driver->note_on(driver, channel, op_mask, v);
}

void fm_driver_note_off(struct fm_driver *driver, int channel) {
	if(driver->note_off)
		driver->note_off(driver, channel);
}

void fm_driver_write_opm_reg(struct fm_driver *driver, uint8_t reg, uint8_t val) {
	if(driver->write_opm_reg)
		driver->write_opm_reg(driver, reg, val);
}

void fm_driver_set_pan(struct fm_driver *driver, int channel, uint8_t pan, uint8_t *v) {
	if(driver->set_pan)
		driver->set_pan(driver, channel, pan, v);
}

void fm_driver_set_noise_freq(struct fm_driver *driver, int channel, int freq) {
	if(driver->set_noise_freq)
		driver->set_noise_freq(driver, channel, freq);
}

void fm_driver_load_voice(struct fm_driver *driver, int channel, uint8_t *v, int opm_volume, int pan) {
	if(driver->load_voice)
		driver->load_voice(driver, channel, v, opm_volume, pan);
}

void fm_driver_load_lfo(struct fm_driver *driver, int channel, uint8_t wave, uint8_t freq, uint8_t pmd, uint8_t amd) {
	if(driver->load_lfo)
		driver->load_lfo(driver, channel, wave, freq, pmd, amd);
}

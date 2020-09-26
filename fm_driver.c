#include "fm_driver.h"
#include "ym2151.h"

static int mdx_note_to_opm(int note) {
	uint8_t tbl[] = {
		0x0, // D#
		0x1, // E
		0x2, // F
		0x4, // F#
		0x5, // G
		0x6, // G#
		0x8, // A
		0x9, // A#
		0xA, // B
		0xC, // C
		0xD, // C#
		0xE, // D
	};
	return (note / 12) * 16 + tbl[note % 12];
}

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

static void fm_opm_driver_reset_key_sync(struct fm_driver *driver, int channel) {
	struct fm_opm_driver *fmdrv = (struct fm_opm_driver *)driver;

	fm_opm_driver_write(fmdrv, 0x01, 0x02);
	fm_opm_driver_write(fmdrv, 0x01, 0x00);
}

static void fm_opm_driver_set_pms_ams(struct fm_driver *driver, int channel, uint8_t pms_ams) {
	struct fm_opm_driver *fmdrv = (struct fm_opm_driver *)driver;

	fm_opm_driver_write(fmdrv, 0x38 + channel, pms_ams);
}

static void fm_opm_driver_set_pitch(struct fm_driver *driver, int channel, int pitch) {
	struct fm_opm_driver *fmdrv = (struct fm_opm_driver *)driver;

	fm_opm_driver_write(fmdrv, 0x28 + channel, mdx_note_to_opm(pitch >> 14));
	fm_opm_driver_write(fmdrv, 0x30 + channel, (pitch >> 6) & 0xfc);
}

static void fm_opm_driver_set_tl(struct fm_driver *driver, int channel, uint8_t tl, uint8_t *v) {
	struct fm_opm_driver *fmdrv = (struct fm_opm_driver *)driver;

	const uint8_t con_masks[8] = {
		0x08, 0x08, 0x08, 0x08, 0x0c, 0x0e, 0x0e, 0x0f,
	};
	int mask = 1;
	for(int i = 0; i < 4; i++, mask <<= 1) {
		if((con_masks[v[1] & 0x07] & mask) > 0) {
			int vol = tl + v[7 + i];
			if(vol > 0x7f) vol = 0x7f;
			fm_opm_driver_write(fmdrv, 0x60 + i * 8 + channel, vol); // TL
		} else {
			fm_opm_driver_write(fmdrv, 0x60 + i * 8 + channel, v[7 + i]); // TL
		}
	}
}

static void fm_opm_driver_note_on(struct fm_driver *driver, int channel, uint8_t op_mask, uint8_t *v) {
	struct fm_opm_driver *fmdrv = (struct fm_opm_driver *)driver;

	fm_opm_driver_write(fmdrv, 0x08, ((op_mask & 0x0f) << 3) | (channel & 0x07)); // Key On
}

static void fm_opm_driver_note_off(struct fm_driver *driver, int channel) {
	struct fm_opm_driver *fmdrv = (struct fm_opm_driver *)driver;

	fm_opm_driver_write(fmdrv, 0x08, channel & 0x07);
}

static void fm_opm_driver_write_opm_reg(struct fm_driver *driver, uint8_t reg, uint8_t data) {
	struct fm_opm_driver *fmdrv = (struct fm_opm_driver *)driver;

	fm_opm_driver_write(fmdrv, reg, data);
}

static void fm_opm_driver_set_pan(struct fm_driver *driver, int channel, uint8_t pan, uint8_t *v) {
	struct fm_opm_driver *fmdrv = (struct fm_opm_driver *)driver;

	fm_opm_driver_write(fmdrv, 0x20 + channel, (pan << 6) | v[1]); // PAN, FL, CON
}

static void fm_opm_driver_set_noise_freq(struct fm_driver *driver, int channel, int freq) {
	struct fm_opm_driver *fmdrv = (struct fm_opm_driver *)driver;

	// TODO is this correct? not writing NE flag
	fm_opm_driver_write(fmdrv, 0x0F, freq & 0x1f);
}

static void fm_opm_driver_load_voice(struct fm_driver *driver, int channel, uint8_t *v, int opm_volume, int pan) {
	struct fm_opm_driver *fmdrv = (struct fm_opm_driver *)driver;

	for(int i = 0; i < 4; i++)
		fm_opm_driver_write(fmdrv, 0x40 + i * 8 + channel, v[ 3 + i]); // DT1, MUL
	fm_opm_driver_set_tl(driver, channel, opm_volume, v);
	for(int i = 0; i < 4; i++)
		fm_opm_driver_write(fmdrv, 0x80 + i * 8 + channel, v[11 + i]); // KS, AR
	for(int i = 0; i < 4; i++)
		fm_opm_driver_write(fmdrv, 0xa0 + i * 8 + channel, v[15 + i]); // AME, D1R
	for(int i = 0; i < 4; i++)
		fm_opm_driver_write(fmdrv, 0xc0 + i * 8 + channel, v[19 + i]); // DT2, D2R
	for(int i = 0; i < 4; i++)
		fm_opm_driver_write(fmdrv, 0xe0 + i * 8 + channel, v[23 + i]); // D1L, RR
	fm_opm_driver_write(fmdrv,  0x20 + channel, (pan << 6) | v[1]); // PAN, FL, CON
}

static void fm_opm_driver_load_lfo(struct fm_driver *driver, int channel, uint8_t wave, uint8_t freq, uint8_t pmd, uint8_t amd) {
	struct fm_opm_driver *fmdrv = (struct fm_opm_driver *)driver;

	fm_opm_driver_write(fmdrv, 0x19, 0x00); // YM2151: LFO Amplitude Modul. Depth
	fm_opm_driver_write(fmdrv, 0x1b, wave & 0x03); // YM2151: LFO Wave Select
	fm_opm_driver_write(fmdrv, 0x18, freq); // YM2151: LFO Frequency
	if(pmd & 0x7f) fm_opm_driver_write(fmdrv, 0x19, pmd); // YM2151: LFO Phase Modul. Depth
	if(amd) fm_opm_driver_write(fmdrv, 0x19, amd); // YM2151: LFO Amplitude Modul. Depth
}

void fm_opm_driver_write(struct fm_opm_driver *driver, uint8_t reg, uint8_t val) {
	if(driver->write)
		driver->write(driver, reg, val);
}

void fm_opm_driver_init(struct fm_opm_driver *driver) {
	fm_driver_init(&driver->fm_driver);

	driver->fm_driver.reset_key_sync = fm_opm_driver_reset_key_sync;
	driver->fm_driver.set_pms_ams    = fm_opm_driver_set_pms_ams;
	driver->fm_driver.set_pitch      = fm_opm_driver_set_pitch;
	driver->fm_driver.set_tl         = fm_opm_driver_set_tl;
	driver->fm_driver.note_on        = fm_opm_driver_note_on;
	driver->fm_driver.note_off       = fm_opm_driver_note_off;
	driver->fm_driver.write_opm_reg  = fm_opm_driver_write_opm_reg;
	driver->fm_driver.set_pan        = fm_opm_driver_set_pan;
	driver->fm_driver.set_noise_freq = fm_opm_driver_set_noise_freq;
	driver->fm_driver.load_voice     = fm_opm_driver_load_voice;
	driver->fm_driver.load_lfo       = fm_opm_driver_load_lfo;

	// reset registers
	for(int i = 0; i < 0x60; i++)
		fm_opm_driver_write(driver, i, 0x00);
	for(int i = 0x60; i < 0x80; i++)
		fm_opm_driver_write(driver, i, 0x7f);
	for(int i = 0x80; i < 0xe0; i++)
		fm_opm_driver_write(driver, i, 0x00);
	for(int i = 0xe0; i <= 0xff; i++)
		fm_opm_driver_write(driver, i, 0x0f);

	// Key off
	for(int i = 0; i < 8; i++)
		fm_opm_driver_write(driver, 0x08, i);
}

static void fm_opm_emu_driver_write(struct fm_opm_driver *driver, uint8_t reg, uint8_t val) {
	struct fm_opm_emu_driver *emudrv = (struct fm_opm_emu_driver *)driver;

	ym2151_write_reg(&emudrv->opm, reg, val);
}

void fm_opm_emu_driver_init(struct fm_opm_emu_driver *driver, int sample_rate) {
	driver->sample_rate = sample_rate;
	ym2151_init(&driver->opm, 4000000, sample_rate);
	ym2151_reset_chip(&driver->opm);
	driver->fm_opm_driver.write = fm_opm_emu_driver_write;

	fm_opm_driver_init(&driver->fm_opm_driver);
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
		outL[i] = outL[i] * 16;
		outR[i] = outR[i] * 16;
	}
}

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

	fm_driver_init(&driver->fm_driver);
}

void fm_opm_midi_driver_deinit(struct fm_opm_midi_driver *driver) {

}

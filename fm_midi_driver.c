#include "fm_midi_driver.h"

void fm_midi_driver_reset_key_sync(struct fm_driver *driver, int channel) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	(void)mididrv;
}

void fm_midi_driver_set_pms_ams(struct fm_driver *driver, int channel, uint8_t pms_ams) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	(void)mididrv;
}

void fm_midi_driver_set_pitch(struct fm_driver *driver, int channel, int pitch) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	if(mididrv->channels[channel].on && pitch != mididrv->channels[channel].pitch) {
		int root_pitch = mididrv->channels[channel].note << 6;
		int detune = ((pitch >> 8) - 5 - root_pitch);

		if(detune != mididrv->channels[channel].detune) {
			midi_track_write_pitch_bend(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 8192 + detune * 4);
			mididrv->channels[channel].detune = detune;
			mididrv->channels[channel].ticks = 0;
		}
	}
	mididrv->channels[channel].pitch = pitch;
}

void fm_midi_driver_set_tl(struct fm_driver *driver, int channel, uint8_t tl, uint8_t *v) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	mididrv->channels[channel].tl = tl;
}

void fm_midi_driver_note_on(struct fm_driver *driver, int channel, uint8_t op_mask, uint8_t *v) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	mididrv->channels[channel].on = 1;
	int pitch = (mididrv->channels[channel].pitch >> 8) - 5;
	int detune = pitch & 0x3f;
	int note = pitch >> 6;
	mididrv->channels[channel].note = note;
	if(detune != mididrv->channels[channel].detune) {
		midi_track_write_pitch_bend(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 8192 + detune * 4);
		mididrv->channels[channel].ticks = 0;
	}
	midi_track_write_note_on(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, note, 127);
	mididrv->channels[channel].ticks = 0;
}

void fm_midi_driver_note_off(struct fm_driver *driver, int channel) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	(void)mididrv;
	mididrv->channels[channel].on = 0;
	int pitch = (mididrv->channels[channel].pitch >> 8) - 5;
	int detune = pitch & 0x3f;
	int note = pitch >> 6;

	mididrv->channels[channel].detune = detune;
	midi_track_write_note_off(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, note, 127);
	mididrv->channels[channel].ticks = 0;
}

void fm_midi_driver_write_opm_reg(struct fm_driver *driver, uint8_t reg, uint8_t data) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	(void)mididrv;
}

void fm_midi_driver_set_pan(struct fm_driver *driver, int channel, uint8_t pan, uint8_t *v) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	(void)mididrv;
}

void fm_midi_driver_set_noise_freq(struct fm_driver *driver, int channel, int freq) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	(void)mididrv;
}

void fm_midi_driver_load_voice(struct fm_driver *driver, int channel, uint8_t *v, int voice_num, int volume, int pan) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	midi_track_write_program_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, voice_num);
	mididrv->channels[channel].ticks = 0;
}

void fm_midi_driver_load_lfo(struct fm_driver *driver, int channel, uint8_t wave, uint8_t freq, uint8_t pmd, uint8_t amd) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	(void)mididrv;
}

void fm_midi_driver_init(struct fm_midi_driver *driver, struct midi_file *midi_file) {
	fm_driver_init(&driver->fm_driver);
	driver->fm_driver.reset_key_sync = fm_midi_driver_reset_key_sync;
	driver->fm_driver.set_pms_ams    = fm_midi_driver_set_pms_ams;
	driver->fm_driver.set_pitch      = fm_midi_driver_set_pitch;
	driver->fm_driver.set_tl         = fm_midi_driver_set_tl;
	driver->fm_driver.note_on        = fm_midi_driver_note_on;
	driver->fm_driver.note_off       = fm_midi_driver_note_off;
	driver->fm_driver.write_opm_reg  = fm_midi_driver_write_opm_reg;
	driver->fm_driver.set_pan        = fm_midi_driver_set_pan;
	driver->fm_driver.set_noise_freq = fm_midi_driver_set_noise_freq;
	driver->fm_driver.load_voice     = fm_midi_driver_load_voice;
	driver->fm_driver.load_lfo       = fm_midi_driver_load_lfo;

	driver->midi_file = midi_file;
	for(int i = 0; i < 8; i++) {
		driver->channels[i].on = 0;
		driver->channels[i].pitch = 0;
		driver->channels[i].note = 0;
		driver->channels[i].detune = 0;
		driver->channels[i].tl = 0;
		driver->channels[i].ticks = 0;
	}
}

void fm_midi_driver_end(struct fm_midi_driver *driver) {
	for(int i = 0; i < 8; i++) {
		midi_track_write_track_end(&driver->midi_file->tracks[i + 1], driver->channels[i].ticks);
		driver->channels[i].ticks = 0;
	}
}

void fm_midi_driver_tick(struct fm_midi_driver *driver) {
	for(int i = 0; i < 8; i++)
		driver->channels[i].ticks++;
}

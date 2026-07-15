#include "adpcm_midi_driver.h"

int adpcm_midi_driver_play(struct adpcm_driver *d, uint8_t channel, uint8_t *data, int len, uint8_t freq, uint8_t vol, int note) {
	// play an entire sample. data is a pointer to an array containing the sample data. len is the length of the sample data.
	#define BASE_NOTE 36
	
	struct adpcm_midi_driver *mididrv = (struct adpcm_midi_driver *)d;
	
	if (mididrv->channels[channel].midi_note != -1) { // make sure that there are no endless notes.
		midi_track_write_note_off(&mididrv->midi_file->tracks[channel + 9], mididrv->channels[channel].ticks, channel+8, mididrv->channels[channel].midi_note, 127);
		mididrv->channels[channel].ticks = 0;
	}
	
	if (vol>8) vol=8;
	int midi_vol = ((double)vol / (double)8) * 127;
	if (midi_vol != mididrv->channels[channel].midi_vol) {
		midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 9], mididrv->channels[channel].ticks, channel+8, 7, midi_vol);
		mididrv->channels[channel].ticks = 0;
		mididrv->channels[channel].midi_vol = midi_vol;
	}
	
	int midi_note = note + BASE_NOTE;
	midi_track_write_note_on(&mididrv->midi_file->tracks[channel + 9], mididrv->channels[channel].ticks, channel+8, midi_note, 127);
	mididrv->channels[channel].ticks = 0;
	mididrv->channels[channel].midi_note = midi_note; // save the currently playing midi note.
	
	return 0;
}

int adpcm_midi_driver_stop(struct adpcm_driver *d, uint8_t channel) {
	struct adpcm_midi_driver *mididrv = (struct adpcm_midi_driver *)d;
	
	if (mididrv->channels[channel].midi_note != -1) {
		midi_track_write_note_off(&mididrv->midi_file->tracks[channel + 9], mididrv->channels[channel].ticks, channel+8, mididrv->channels[channel].midi_note, 127);
		mididrv->channels[channel].ticks = 0;
		mididrv->channels[channel].midi_note = -1; // no note is playing.
	}
	
	return 0;
}

int adpcm_midi_driver_set_volume(struct adpcm_driver *d, uint8_t channel, uint8_t vol) { // TODO: research the adpcm chip's volume control more.
	
	struct adpcm_midi_driver *mididrv = (struct adpcm_midi_driver *)d;

	if (vol>8) vol=8;
	int midi_vol = ((double)vol / (double)8) * 127;
	if (midi_vol != mididrv->channels[channel].midi_vol) {
		midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 9], mididrv->channels[channel].ticks, channel+8, 7, midi_vol);
		mididrv->channels[channel].ticks = 0;
		mididrv->channels[channel].midi_vol = midi_vol;
	}
	
	
	return 0;
}

int adpcm_midi_driver_set_freq(struct adpcm_driver *d, uint8_t channel, uint8_t freq) {
	return 0;
}

int adpcm_midi_driver_set_pan(struct adpcm_driver *d, uint8_t pan) { // sets pan for the entire chip
	struct adpcm_midi_driver *mididrv = (struct adpcm_midi_driver *)d;
	
	int midi_pan = 64;
	
	switch (pan) {
		case 0b10: // right
			midi_pan = 127;
			break;
		case 0b01: // left
			midi_pan = 0;
			break;
		case 0b11: // center
		default:
			midi_pan = 64;
			break;
	}
	
	if (midi_pan != mididrv->midi_pan) {
		for (int channel=0; channel < 8; channel++){
			midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 9], mididrv->channels[channel].ticks, channel+8, 10, midi_pan);
			mididrv->channels[channel].ticks = 0;	
		}
		mididrv->midi_pan = midi_pan;
	}
	return 0;
}

int adpcm_midi_driver_init(struct adpcm_midi_driver *driver, struct midi_file *midi_file) {
	adpcm_driver_init(&driver->adpcm_driver);
	driver->adpcm_driver.play = adpcm_midi_driver_play;
	driver->adpcm_driver.stop = adpcm_midi_driver_stop;
	driver->adpcm_driver.set_freq = adpcm_midi_driver_set_freq;
	driver->adpcm_driver.set_volume = adpcm_midi_driver_set_volume;
	driver->adpcm_driver.set_pan = adpcm_midi_driver_set_pan;

	driver->midi_file = midi_file;
	driver->midi_pan = -1;
	for(int i = 0; i < 8; i++) {
		driver->channels[i].ticks = 0;
		driver->channels[i].midi_note = -1;
		driver->channels[i].midi_vol = -1;
	}

	return 0;
}

void adpcm_midi_driver_end(struct adpcm_midi_driver *driver) {
	for(int i = 0; i < 8; i++) {
		midi_track_write_track_end(&driver->midi_file->tracks[i + 9], driver->channels[i].ticks);
		driver->channels[i].ticks = 0;
	}
}

int adpcm_midi_driver_tick(struct adpcm_midi_driver *driver) {
	for(int i = 0; i < 8; i++)
		driver->channels[i].ticks++;
	return 0;
}

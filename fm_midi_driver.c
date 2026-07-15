#include "fm_midi_driver.h"

#define TRANSPOSE 3 // add 3 semitones to every note. this matches mdx.c line 217.
void fm_midi_driver_reset_key_sync(struct fm_driver *driver, int channel) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	
	midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 74, 127);
	mididrv->channels[channel].ticks = 0;
	midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 74, 0); // TEST. I don't know if this will break the reset.
	mididrv->channels[channel].ticks = 0;
}

void fm_midi_driver_set_pms_ams(struct fm_driver *driver, int channel, uint8_t pms_ams) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	
	uint8_t pms = (pms_ams>>4) & 7;
	uint8_t ams = (pms_ams & 3);
	midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 75, ((double)pms / (double)7) * 127);
	mididrv->channels[channel].ticks = 0;
	midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 76, ((double)ams / (double)3) * 127);
	mididrv->channels[channel].ticks = 0;
}

void fm_midi_driver_set_pitch(struct fm_driver *driver, int channel, int pitch) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	if(mididrv->channels[channel].on && pitch != mididrv->channels[channel].pitch) {
		
		int orig_note_pitch = ((pitch >> 14) & 0x7F) + TRANSPOSE;
		int orig_pitch_bend = ((pitch >> 8)) & 0x3F;
		int note_difference = orig_note_pitch - mididrv->channels[channel].midi_note; // the number of semitones by which the pitch has changed. If negative, go down that many number of semitones.
		int midi_pitch_bend = 8192 + (note_difference * 256 /*pitch bend value of one semitone, when the pitch bend range is 32*/) + (orig_pitch_bend * 4); // midi_pitch_bend needs to include both orig_note_pitch and orig_pitch_bend.
		midi_track_write_pitch_bend(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, midi_pitch_bend);
		mididrv->channels[channel].ticks = 0;
		mididrv->channels[channel].midi_pitch_bend = midi_pitch_bend;
	}
	mididrv->channels[channel].pitch = pitch;
}

void fm_midi_driver_set_tl(struct fm_driver *driver, int channel, uint8_t tl, uint8_t *v) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	mididrv->channels[channel].tl = tl;
	
	const uint8_t con_masks[8] = {
		0x08, 0x08, 0x08, 0x08, 0x0c, 0x0e, 0x0e, 0x0f,
	};
	int mask = 1;
	for(int i = 0; i < 4; i++, mask <<= 1) {
		int op_vol = v[7 + i]; // op_vol is a uint8_t. i is a single operator. There are 4 operators for each channel. There are 8 FM channels total.
		int true_op = 0;
		switch (i) { // TODO: TEST if op vol is being assigned to each operator correctly.
			case 1:
				true_op = 2;
				break;
			case 2:
				true_op = 1;
				break;
			default:
				true_op = i;
				break;
		}
		int midi_vol = 0;
		if((con_masks[v[1] & 0x07] & mask) > 0) {
			int vol = tl + op_vol;
			if(vol > 0x7f) vol = 0x7f;
			midi_vol = 127 - vol; // volume inputs for OPM registers are inverted, so 0 in a register is the loudest. VOPMex by default handles it in a way that's more intuitive where 127 is the loudest, so the volume must be inverted here.
		} else {
			midi_vol = 127 - op_vol;
		}
		if (midi_vol != mididrv->channels[channel].midi_vol[true_op]) {
			midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 16 + true_op /*VOPMax TL CC*/, midi_vol);
			mididrv->channels[channel].ticks = 0;
			mididrv->channels[channel].midi_vol[true_op] = midi_vol;
		}
	}
}

void fm_midi_driver_note_on(struct fm_driver *driver, int channel, uint8_t op_mask, uint8_t *v) { // *v is a pointer to the orignal voice data from the MDX, and "channel" is the new track being written.

	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	
	if (mididrv->channels[channel].on == 1 && mididrv->channels[channel].midi_note != -1) {
		midi_track_write_note_off(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, mididrv->channels[channel].midi_note, 127);
		mididrv->channels[channel].ticks = 0;
	}
	
	// force pitch bend range to 32 for every note. Simple fix for when the DAW / VOPMex forgets pitch bend range. TODO: have this be a command line option for mdx2midi?
	midi_track_write_rpn_msb(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 0, 32); // Pitch Bend Range
	mididrv->channels[channel].ticks = 0;
	
	if (mididrv->channels[channel].midi_pan == -1) mididrv->channels[channel].midi_pan = 64;
	midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 10, mididrv->channels[channel].midi_pan); // prevent VOPMex from forgetting panning when skipping through the song.
	mididrv->channels[channel].ticks = 0;
	
	mididrv->channels[channel].on = 1;

	int orig_pitch_bend = (mididrv->channels[channel].pitch >> 8) & 0x3F;
	int orig_note = ((mididrv->channels[channel].pitch >> 14) & 0x7F) + TRANSPOSE;
	int midi_note = orig_note;
	int midi_pitch_bend = 8192; // centered.
	if (orig_pitch_bend > 32 && 0 /*disable this branch for now for stability*/) { // if the pitch bend would raise the pitch by more than a quarter tone (half a semitone), raise the note by one semitone and invert the pitch bend. This is done to avoid too many large pitch bends in the output.
		midi_note++;
		int downward_orig_pitch_bend = 64 - orig_pitch_bend;
		midi_pitch_bend = 8192 - (downward_orig_pitch_bend * 4);
	} else {
		midi_pitch_bend = 8192 + (orig_pitch_bend * 4);
	}
	//if(midi_pitch_bend != mididrv->channels[channel].midi_pitch_bend) { // I disabled this check because the DAW / VOPMex was forgetting the current pitch bend.
		midi_track_write_pitch_bend(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, midi_pitch_bend);
		mididrv->channels[channel].ticks = 0;
		mididrv->channels[channel].midi_pitch_bend = midi_pitch_bend;
	//}
	
	for (int i=0; i<4; i++){ // ensure VOPMex doesn't discard operator volume after a PC.
		if (mididrv->channels[channel].midi_vol[i] != -1) {
			midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 16 + i, mididrv->channels[channel].midi_vol[i]);
			mididrv->channels[channel].ticks = 0;
		}
	}
		
	midi_track_write_note_on(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, midi_note, 127);
	mididrv->channels[channel].ticks = 0;
	mididrv->channels[channel].midi_note = midi_note; // save the currently playing midi note.
}

void fm_midi_driver_note_off(struct fm_driver *driver, int channel) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	
	mididrv->channels[channel].on = 0;

	midi_track_write_note_off(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, mididrv->channels[channel].midi_note, 127);
	mididrv->channels[channel].ticks = 0;
	mididrv->channels[channel].midi_note = -1; // no note is playing.
}

void fm_midi_driver_write_opm_reg(struct fm_driver *driver, uint8_t reg, uint8_t data) { // TODO: finish implementing all midi functions (just this opm reg write one now). TEST all functions in this file.
	// struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
}

void fm_midi_driver_set_pan(struct fm_driver *driver, int channel, uint8_t pan, uint8_t *v) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	
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
	
	//if (midi_pan != mididrv->channels[channel].midi_pan) { // pan changes don't seem to be frequent enough to need this check.
		midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 10, midi_pan);
		mididrv->channels[channel].ticks = 0;
		mididrv->channels[channel].midi_pan = midi_pan;
	//}
}

void fm_midi_driver_set_noise_freq(struct fm_driver *driver, int channel, int freq) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	
	int midi_freq = ((double)freq / (double)0x1f) * 127;
	
	if (midi_freq != mididrv->channels[channel].freq) {
		midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 82, midi_freq);
		mididrv->channels[channel].ticks = 0;
		mididrv->channels[channel].freq = midi_freq;
	}
}

void fm_midi_driver_load_voice(struct fm_driver *driver, int channel, uint8_t *v, int voice_num, int volume, int pan) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	midi_track_write_program_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, voice_num);
	mididrv->channels[channel].ticks = 0;
}

void fm_midi_driver_load_lfo(struct fm_driver *driver, int channel, uint8_t wave, uint8_t freq, uint8_t pmd, uint8_t amd) {
	struct fm_midi_driver *mididrv = (struct fm_midi_driver *)driver;
	
	midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 12, ((double)wave / (double)3) * 127);
	mididrv->channels[channel].ticks = 0;
	midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 1, freq); // TODO: research freq more and make sure it's implemented correctly.
	mididrv->channels[channel].ticks = 0;
	
	if (pmd & 0b10000000 || amd & 0b10000000 /*TODO: try removing amd here*/) { // only one of amd or pmd is active at a time.
		// PMD
		midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 2, pmd);
		mididrv->channels[channel].ticks = 0;
		midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 3, 0);
		mididrv->channels[channel].ticks = 0;
	} else {
		// AMD
		midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 3, amd);
		mididrv->channels[channel].ticks = 0;
		midi_track_write_control_change(&mididrv->midi_file->tracks[channel + 1], mididrv->channels[channel].ticks, channel, 2, 0);
		mididrv->channels[channel].ticks = 0;
	}
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
		driver->channels[i].midi_note = -1;
		driver->channels[i].midi_pitch_bend = -1;
		driver->channels[i].midi_pan = -1;
		driver->channels[i].freq = -1;
		for(int i2 = 0; i2 < 4; i2++) {
			driver->channels[i].midi_vol[i2] = -1;
		}
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

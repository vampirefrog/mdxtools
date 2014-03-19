#include "MDX.h"

void MDXHeader::read(FileReadStream &s) {
	// Read in the title and PDX file
	title = s.readLine(0x1a);
	if(title && *title) {
		char *nl = strrchr((char *)title, '\r');
		if(nl) *nl = 0;
	}
	pcmFile = s.readLine(0);
	fileBase = s.tell();
	voiceOffset = s.readBigUint16();

	// Read in the channel positions
	memset(channels, 0, sizeof(channels));
	channels[0].offset = s.readBigUint16();
	numChannels = channels[0].offset / 2 - 1;
	if(numChannels > 16) numChannels = 16;
	for(int i = 1; i < numChannels; i++) {
		channels[i].offset = s.readBigUint16();
	}
	for(int i = 0; i < numChannels; i++) {
		channels[i].length = ((i == numChannels - 1) ? voiceOffset : channels[i + 1].offset) - channels[i].offset;
	}

	// Read in the voices
	memset(voices, 0, sizeof(voices));
	s.seek(fileBase + voiceOffset);
	while(!s.eof()) {
		MDXVoice inst;
		if(!inst.load(s)) break;
		if(!voices[inst.number]) // Do not allocate twice the same voice
			voices[inst.number] = new MDXVoice(inst);
	}
}

void MDXHeader::dump() {
	if(title && *title) printf("Title: \"%s\"\n", title);
	if(pcmFile && *pcmFile) printf("PCM File: \"%s\"\n", pcmFile);
	printf("fileBase=%x\n", fileBase);
	printf("numChannels=%d\n", numChannels);
	for(uint16_t i = 0; i < numChannels; i++) {
		printf("%u: offset=%d length=%d\n", i, channels[i].offset, channels[i].length);
	}
	printf("voiceOffset=%d\n", voiceOffset);
	for(uint16_t i = 0; i <= 255; i++) {
		if(voices[i]) voices[i]->dump();
	}
}

bool MDXParser::eat(uint8_t b) {
	handleByte(b);
	pos++;
	switch(state) {
		case None:
			if(b >= 0x00 && b <= 0x7f) {
				handleRest(b + 1);
			} else if(b >= 0x80 && b < 0xdf) {
				nn = b;
				state = NoteDuration;
			} else {
				switch(b) {
					case 0xff: // Set tempo
						state = TempoVal;
						break;
					case 0xfe:
						state = OPMRegisterNum;
						break;
					case 0xfd:
						state = VoiceNum;
						break;
					case 0xfc:
						state = PanVal;
						break;
					case 0xfb:
						state = VolumeVal;
						break;
					case 0xfa:
						handleVolumeDec();
						handleCommand(b);
						state = None;
						break;
					case 0xf9:
						handleVolumeInc();
						handleCommand(b);
						state = None;
						break;
					case 0xf8:
						state = SoundLen;
						break;
					case 0xf7:
						handleDisableKeyOff();
						handleCommand(b);
						state = None;
						break;
					case 0xf6:
						state = RepeatStartCount;
						break;
					case 0xf5:
						state = RepeatEndOffsetMSB;
						break;
					case 0xf4:
						state = RepeatEscapeMSB;
						break;
					case 0xf3:
						state = DetuneMSB;
						break;
					case 0xf2:
						state = PortamentoMSB;
						break;
					case 0xf1:
						state = DataEndMSB;
						break;
					case 0xf0:
						state = KeyOnDelayVal;
						break;
					case 0xef:
						state = SyncSendChannel;
						break;
					case 0xee:
						handleSyncWait();
						handleCommand(b);
						state = None;
						break;
					case 0xed:
						state = ADPCMNoiseFreqVal;
						break;
					case 0xec:
						state = LFOPitchWave;
						break;
					case 0xeb:
						state = LFOVolumeWave;
						break;
					case 0xea:
						state = OPMLFOSyncWave;
						break;
					case 0xe9:
						state = LFODelayVal;
						break;
					case 0xe8:
						handlePCM8ExpansionShift();
						handleCommand(b);
						state = None;
						break;
					case 0xe7:
						state = ExtendedCmd;
						break;
					default:
						handleUndefinedCommand(b);
						handleCommand(b);
						break;
				}
			}
			break;
		case NoteDuration:
			handleNote(nn - 0x80, b + 1);
			state = None;
			break;
		case TempoVal:
			handleSetTempo(b);
			handleCommand(0xff, b);
			state = None;
			break;
		case OPMRegisterNum:
			nn = b;
			state = OPMRegisterVal;
			break;
		case OPMRegisterVal:
			handleSetOPMRegister(nn, b);
			handleCommand(0xfe, nn, b);
			state = None;
			break;
		case VoiceNum:
			handleSetVoiceNum(b);
			handleCommand(0xfd, b);
			state = None;
			break;
		case PanVal:
			handlePan(b);
			handleCommand(0xfc, b);
			state = None;
			break;
		case VolumeVal:
			handleSetVolume(b);
			handleCommand(0xfb, b);
			state = None;
			break;
		case SoundLen:
			handleSoundLength(b);
			handleCommand(0xf8, b);
			state = None;
			break;
		case RepeatStartCount:
			handleRepeatStart(b);
			handleCommand(0xf6, b);
			state = RepeatStartZero;
			break;
		case RepeatStartZero:
			state = None;
			break;
		case RepeatEndOffsetMSB:
			nn = b;
			state = RepeatEndOffsetLSB;
			break;
		case RepeatEndOffsetLSB:
			handleRepeatEnd((nn << 8) | b);
			handleCommand(0xf5, (nn << 8) | b);
			state = None;
			break;
		case RepeatEscapeMSB:
			nn = b;
			state = RepeatEscapeLSB;
			break;
		case RepeatEscapeLSB:
			handleRepeatEscape((nn << 8) | b);
			handleCommand(0xf4, (nn << 8) | b);
			state = None;
			break;
		case DetuneMSB:
			nn = b;
			state = DetuneLSB;
			break;
		case DetuneLSB:
			handleDetune((nn << 8) | b);
			handleCommand(0xf3, (nn << 8) | b);
			state = None;
			break;
		case PortamentoMSB:
			nn = b;
			state = PortamentoLSB;
			break;
		case PortamentoLSB:
			handlePortamento((nn << 8) | b);
			handleCommand(0xf2, (nn << 8) | b);
			state = None;
			break;
		case DataEndMSB:
			if(b == 0) {
				handleDataEnd();
				handleCommand(0xf1, 0);
				state = None;
				return true;
			} else {
				nn = b;
				state = DataEndLSB;
			}
			break;
		case DataEndLSB:
			handleDataEnd((nn << 8) | b);
			handleCommand(0xf1, (nn << 8) | b);
			state = None;
			return true;
			break;
		case KeyOnDelayVal:
			handleKeyOnDelay(b);
			handleCommand(0xf0, b);
			state = None;
			break;
		case SyncSendChannel:
			handleSyncSend(b);
			handleCommand(0xef, b);
			state = None;
			break;
		case ADPCMNoiseFreqVal:
			handleADPCMNoiseFreq(b);
			handleCommand(0xed, b);
			state = None;
			break;
		case LFOPitchWave:
			if(b == 0x80) {
				handleLFOPitchMPOF();
				handleCommand(0xec, b);
				state = None;
			} else if(b == 0x81) {
				handleLFOPitchMPON();
				handleCommand(0xec, b);
				state = None;
			} else {
				nn = b;
				state = LFOPitchPeriodMSB;
			}
			break;
		case LFOPitchPeriodMSB:
			w = b << 8;
			state = LFOPitchPeriodLSB;
			break;
		case LFOPitchPeriodLSB:
			w |= b;
			state = LFOPitchChangeMSB;
			break;
		case LFOPitchChangeMSB:
			v = b << 8;
			state = LFOPitchChangeLSB;
			break;
		case LFOPitchChangeLSB:
			v |= b;
			handleLFOPitch(nn, nn == 0 ? w>>2 : w>>1, nn == 1 ? (v + 255) >> 8 : (v * w + 511) >> 9);
			handleCommand(0xec, nn, w, v);
			state = None;
			break;
		case LFOVolumeWave:
			if(b == 0x80) {
				handleLFOVolumeMAOF();
				handleCommand(0xeb, b);
				state = None;
			} else if(b == 0x81) {
				handleLFOVolumeMAON();
				handleCommand(0xeb, b);
				state = None;
			} else {
				nn = b;
				state = LFOVolumePeriodMSB;
			}
			break;
		case LFOVolumePeriodMSB:
			w = b << 8;
			state = LFOVolumePeriodLSB;
			break;
		case LFOVolumePeriodLSB:
			w |= b;
			state = LFOVolumeChangeMSB;
			break;
		case LFOVolumeChangeMSB:
			v = b << 8;
			state = LFOVolumeChangeLSB;
			break;
		case LFOVolumeChangeLSB:
			v |= b;
			handleLFOVolume(nn, nn == 0 ? w>>2 : w>>1, nn == 1 ? (v + 255) >> 8 : (v * w + 255) >> 8);
			handleCommand(0xeb, nn, w, v);
			state = None;
			break;
		case OPMLFOSyncWave:
			if(b == 0x80) {
				handleOPMLFOMHOF();
				handleCommand(0xea, b);
				state = None;
			} else if(b == 0x81) {
				handleOPMLFOMHON();
				handleCommand(0xea, b);
				state = None;
			} else {
				nn = b;
				state = OPMLFOLFRQ;
			}
			break;
		case OPMLFOLFRQ:
			n2 = b;
			state = OPMLFOPMD;
			break;
		case OPMLFOPMD:
			n3 = b;
			state = OPMLFOAMD;
			break;
		case OPMLFOAMD:
			n4 = b;
			state = OPMLFOPMSAMS;
			break;
		case OPMLFOPMSAMS:
			handleOPMLFO(nn, n2, n3, n4, b);
			handleCommand(0xea, nn, n2, n3, n4, b);
			state = None;
			break;
		case LFODelayVal:
			handleLFODelaySetting(b);
			handleCommand(0xe9, b);
			state = None;
			break;
		case ExtendedCmd:
			switch(b) {
				case 0x00:
					handleKill();
					handleCommand(0xe7, 0x00);
					break;
				case 0x01:
					state = FadeOutValue;
					break;
				case 0x02:
					break;
				case 0x03:
					state = FlagValue;
					break;
				case 0x04:
					break;
				case 0x05:
					break;
				case 0x06:
					break;
			}
			break;
		case FadeOutValue:
			handleFadeOut(b);
			handleCommand(0xe7, 0x01, b);
			state = None;
			break;
		case FlagValue:
			handleFlag(b);
			handleCommand(0xe7, 0x03, b);
			state = None;
		default:
			printf("Unknown state %d\n", state);
			break;
	}
	return false;
}

#include "PDX.h"
#include "Soundfont.h"
#include "ADPCMDecoder.h"

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		PDXLoader p(argv[i]);
		SF2_DATA* s = CreateSF2Base(argv[i]);
		int totalSamplesSize = 0;
		int totalSamples = 0;
		for(int j = 0; j < PDX_NUM_SAMPLES; j++) {
			if(p.samples[j].length > 0) {
				totalSamplesSize += p.samples[j].length + 46;
				totalSamples++;
			}
		}

		int16_t *samples = (int16_t *)malloc(totalSamplesSize * 2);
		memset(samples, 0, totalSamplesSize * 2);
		int curPos = 0;
		printf("%s: %d samples\n", argv[i], totalSamples);
		sfSample *sampleHeaders = (sfSample *)malloc((totalSamples + 1) * sizeof(sfSample));
		ADPCMDecoder d;
		int x = 0;
		for(int j = 0; j < PDX_NUM_SAMPLES; j++) {
			if(p.samples[j].length > 0) {
				uint8_t *ps = p.loadSample(j);
				d.decode(ps, p.samples[j].length, samples + curPos);
				delete[] ps;

				sfSample *h = sampleHeaders + x;
				h->dwSampleRate = 15600;
				h->byOriginalKey = 60;
				h->chCorrection = 0;
				h->wSampleLink = 0;
				h->sfSampleType = monoSample;
				snprintf(h->achSampleName, sizeof(h->achSampleName), "Sample %03hX", x);
				h->dwStart = curPos;
				h->dwEnd = curPos + p.samples[j].length + 46;
				h->dwStartloop = curPos;
				h->dwEndloop = h->dwEnd;

				curPos += (p.samples[j].length + 46);
				x++;
			}
		}
		sfSample *h = sampleHeaders + x;
		memset(h, 0, sizeof(sfSample));
		strcpy(h->achSampleName, "EOS");	// write "End Of Samples" header

		LIST_CHUNK *LstChk = List_GetChunk(s->Lists, FCC_sdta);
		ITEM_CHUNK *ItmChk = Item_MakeChunk(FCC_smpl, totalSamplesSize * 2, samples, 0x00);
		List_AddItem(LstChk, ItmChk);

		LstChk = List_GetChunk(s->Lists, FCC_pdta);
		ItmChk = Item_MakeChunk(FCC_shdr, (totalSamples + 1) * sizeof(sfSample), sampleHeaders, 0x00);	// no free() needed either
		List_AddItem(LstChk, ItmChk);

		sfInst inst[2];
		memset(inst, 0, sizeof(inst));
		snprintf(inst[0].achInstName, sizeof(inst[0].achInstName), "Instrument %d", 1);
		inst[0].wInstBagNdx = 0;
		snprintf(inst[1].achInstName, sizeof(inst[1].achInstName), "EOI");
		inst[1].wInstBagNdx = totalSamples;
		ItmChk = Item_MakeChunk(FCC_inst, sizeof(inst), &inst, 0);
		List_AddItem(LstChk, ItmChk);

		int bagSize = (totalSamples + 1) * sizeof(sfInstBag);
		sfInstBag *bags = (sfInstBag *)malloc(bagSize);
#define GENS_PER_ZONE 3
#define BASE_NOTE 36
		for(int j = 0; j <= totalSamples; j++) {
			bags[j].wInstGenNdx = j * GENS_PER_ZONE;
			bags[j].wInstModNdx = 0;
		}
		ItmChk = Item_MakeChunk(FCC_ibag, bagSize, bags, 0);
		List_AddItem(LstChk, ItmChk);

		int genSize = ((totalSamples) * GENS_PER_ZONE  + 1) * sizeof(sfInstGenList);
		sfInstGenList *instGenLists = (sfInstGenList *)malloc(genSize);
		memset(instGenLists, 0, genSize);
		x = 0;
		for(int j = 0; j < PDX_NUM_SAMPLES; j++) {
			if(p.samples[j].length > 0) {
				int k = x * GENS_PER_ZONE;
				instGenLists[k].sfGenOper = keyRange;
				instGenLists[k].genAmount.ranges.byHi = BASE_NOTE + j;
				instGenLists[k].genAmount.ranges.byLo = BASE_NOTE + j;
				k++;
				instGenLists[k].sfGenOper = overridingRootKey;
				instGenLists[k].genAmount.wAmount = BASE_NOTE + j;
				k++;
				instGenLists[k].sfGenOper = sampleID;
				instGenLists[k].genAmount.wAmount = x;
				x++;
			}
		}
		ItmChk = Item_MakeChunk(FCC_igen, genSize, instGenLists, 0);
		List_AddItem(LstChk, ItmChk);

		sfModList modList;
		memset(&modList, 0, sizeof(modList));
		ItmChk = Item_MakeChunk(FCC_imod, sizeof(modList), &modList, 0);
		List_AddItem(LstChk, ItmChk);

		sfPresetHeader presetHeaders[2];
		snprintf(presetHeaders[0].achPresetName, sizeof(presetHeaders[0].achPresetName), "Preset 1");
		presetHeaders[0].wPreset = 0;
		presetHeaders[0].wBank = 0;
		presetHeaders[0].wPresetBagNdx = 0;
		presetHeaders[0].dwLibrary = 0;
		presetHeaders[0].dwGenre = 0;
		presetHeaders[0].dwMorphology = 0;
		memset(presetHeaders + 1, 0, sizeof(sfPresetHeader));
		snprintf(presetHeaders[1].achPresetName, sizeof(presetHeaders[1].achPresetName), "EOP");
		presetHeaders[i].wPreset = 0xff;
		presetHeaders[i].wBank = 0xff;
		presetHeaders[i].wPresetBagNdx = 1;
		ItmChk = Item_MakeChunk(FCC_phdr, sizeof(presetHeaders), presetHeaders, 0);
		List_AddItem(LstChk, ItmChk);

		sfPresetBag presetBags[2];
		presetBags[0].wGenNdx = 0;
		presetBags[0].wModNdx = 0;
		presetBags[1].wGenNdx = 1;
		presetBags[1].wModNdx = 0;
		ItmChk = Item_MakeChunk(FCC_pbag, sizeof(presetBags), presetBags, 0);
		List_AddItem(LstChk, ItmChk);

		sfGenList presetGenOpers[2];
		presetGenOpers[0].sfGenOper = instrument;
		presetGenOpers[0].genAmount.wAmount = 0;
		presetGenOpers[1].sfGenOper = 0;
		presetGenOpers[1].genAmount.wAmount = 0;
		ItmChk = Item_MakeChunk(FCC_pgen, sizeof(presetGenOpers), presetGenOpers, 0);
		List_AddItem(LstChk, ItmChk);		

		sfModList presetModList;
		memset(&presetModList, 0, sizeof(sfModList));
		ItmChk = Item_MakeChunk(FCC_pmod, sizeof(presetModList), &presetModList, 0);
		List_AddItem(LstChk, ItmChk);

		char buf[256];
		snprintf(buf, sizeof(buf), "%s.sf2", argv[i]);
		WriteSF2toFile(s, buf);
		printf("wrote %s\n", buf);
		free(samples);
		free(sampleHeaders);
		free(bags);
		free(instGenLists);
	}
}

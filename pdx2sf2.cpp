#include <string.h>

extern "C" {
	#include "pdx.h"
	#include "adpcm.h"
	#include "tools.h"
}

#include <algorithm>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "sf2cute/include/sf2cute.hpp"

using namespace sf2cute;

int main(int argc, char **argv) { // TEST more PDX files to ensure no samples are so loud they clip.
	if (argc < 2)
		printf("Usage: pdx2sf2 your_pdx_file.pdx\n");
	for(int i = 1; i < argc; i++) {
		printf("argv[%d]: %s\n", i, argv[i]);
		size_t data_len = 0;
		uint8_t *data = load_file(argv[i], &data_len);
		if(!data) {
			fprintf(stderr, "Could not load %s\n", argv[i]);
			return 1;
		}

		struct pdx_file p;
		{
			int result = pdx_file_load(&p, data, data_len);
			if (result == 0) 
				printf("Successfully loaded PDX file %s.\n", argv[i]);
			else
				printf("Failed to load PDX file %s.\n", argv[i]);
		}

		SoundFont sf2;
		sf2.set_sound_engine("EMU8000"); // ?
		int totalSamplesSize = 0;
		int totalSamples = 0;
		for(int j = 0; j < PDX_NUM_SAMPLES; j++) {
			if(p.samples[j].len > 0) {
				totalSamplesSize += p.samples[j].num_samples + 46;
				totalSamples++;
			}
		}

		int curPos = 0;
		int x = 0;
		// make new instrument here.
		std::shared_ptr<SFInstrument> instrument_1 = sf2.NewInstrument("Instrument 1");
		
#define GENS_PER_ZONE 3
#define BASE_NOTE 36
		x = 0;
		int16_t largest_sample = -0x8000;
		int16_t smallest_sample = 0x7FFF;
		for(int j = 0; j < PDX_NUM_SAMPLES; j++) {
			
			std::vector<int16_t> converted_sample_data(p.samples[j].decoded_data, p.samples[j].decoded_data + p.samples[j].num_samples);
			for (int sampleI = 0; sampleI < converted_sample_data.size(); sampleI++) {
				// debug
				if (converted_sample_data[sampleI] > largest_sample)
					largest_sample = converted_sample_data[sampleI];
				if (converted_sample_data[sampleI] < smallest_sample)
					smallest_sample = converted_sample_data[sampleI];
				
				converted_sample_data[sampleI] = converted_sample_data[sampleI] << 4; // convert from sint12 sample to sint16 sample.
			}
			
			if(p.samples[j].num_samples > 0 && p.samples[j].len > 0) { // Each note in the single instrument plays a different sample.
				char sample_name[13]; // Sample 0x060
				snprintf(sample_name, 13, "Sample 0x%03X", j);
				std::shared_ptr<SFSample> my_sample_pointer = sf2.NewSample(
					std::string(sample_name), // name
					converted_sample_data, // sample data
					0, // start loop
					uint32_t(p.samples[j].num_samples), // end loop
					15600, // sample rate
					60, // root key
					0, // microtuning
					std::weak_ptr<SFSample>(), // pointer to other sample in a left-right stereo sample pair. NULL here because these are all mono samples.
					SFSampleLink::kMonoSample);

				SFInstrumentZone instrument_zone(my_sample_pointer);
				instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(BASE_NOTE + j, BASE_NOTE + j)));
				instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kOverridingRootKey, BASE_NOTE + j));
				// add zone to instrument here.
				instrument_1->AddZone(instrument_zone);
			}
		}
		printf("largest_sample: %d, smallest_sample: %d\n", largest_sample, smallest_sample);

		std::shared_ptr<SFPreset> preset_50 = sf2.NewPreset(
			"Preset 1", 0, 0,
			std::vector<SFPresetZone>{
				SFPresetZone(instrument_1)
			});

		char buf[256];
		replace_ext(buf, sizeof(buf), argv[i], "sf2");
		printf("Writing to file %s\n", buf);
		try {
			std::ofstream ofs(std::string(buf), std::ios::binary);
			sf2.Write(ofs);
			return 0;
		}
		catch (const std::fstream::failure & e) {
			// File output error.
			std::cerr << e.what() << std::endl;
			return 1;
		}
		catch (const std::exception & e) {
			// Other errors.
			// For example: Too many samples.
			std::cerr << e.what() << std::endl;
			return 1;
		}

		free(data);
		for(int j = 0; j < PDX_NUM_SAMPLES; j++) {
			if(p.samples[j].decoded_data != NULL) {
				free(p.samples[j].decoded_data);
				break; // only one allocation; stop after freeing the root pointer
			}
		}
	}
}

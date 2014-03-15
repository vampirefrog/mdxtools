#ifndef FAT_H_
#define FAT_H_

struct DirEntry {
	char name[8];
	char ext[3];
	union {
		uint8_t b;
		struct {
			unused: 1,
			unused2: 1,
			archive: 1,
			subDir: 1,
			volLabel: 1,
			system: 1,
			hidden: 1,
			readOnly: 1,
		} s;
	} attributes;
	uint16_t timeCreated;
	uint16_t dateCreated;
	uint16_t firstCluster;
	uint32_t size;
};

class FAT {
	char description[8];
	uint16_t bytesPerBlock;
	uint8_t blocksPerCluster;
	uint16_t reservedBlocks;
	uint8_t numFATs;
	uint16_t numRootDirEntries;
	uint16_t totalBlocks;
	uint8_t mediaDescriptor;
	uint16_t FATBlocks;
	uint16_t blocksPerTrack;
	uint16_t heads;
	uint32_t hiddenBlocks;
	uint32_t totalBlocks32;
	uint16_t driveNumber;
	uint8_t EBRSignature;
	uint32_t serialNo;
	char label[11];
	char fsId[8];

	uint8_t *bootSect;
	uint16_t bootSectSize;
	uint8_t *fat;
	DirEntry *rootDirEntries;

public:
	void loadBootSector(const char *inFile) {
		bootSect = FS::readFile(inFile, &bootSectSize);
		if(!bootSect) throw exceptionf("Could not load boot sector from %s\n", inFile);
	}
	void addFile(const char *fatPath, const char *inFile);
	void saveImg(const char *filename);
};

#endif /* FAT_H_ */

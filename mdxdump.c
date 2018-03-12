#include "mdx.h"
#include "tools.h"

int main(int argc, char **argv) {
	if(argc < 2) {
		return 1;
	}

	size_t data_len = 0;
	uint8_t *data = load_file(argv[1], &data_len);
	if(!data) {
		fprintf(stderr, "Could not load %s\n", argv[1]);
		return 1;
	}

	mdx_file f;
	mdx_file_load(&f, data, data_len);

	printf("%d channels\n", f.num_channels);
	for(int i = 0; i < f.num_channels; i++) {
		printf("channel %d\n", i);
		int pos = 0;
		dump_cmd(f.channels[i].
	}

	free(data);

	return 0;
}

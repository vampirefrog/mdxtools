#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "cmdline.h"
#include "tools.h"
#include "pdx.h"
#include "md5.h"

char *opt_output = 0;
int opt_recursive = 0;
int opt_header = 0;
int opt_samples = 0;
FILE *of;

static void pdxinfo(const char *filename) {
	if(strcasecmp(filename + strlen(filename) - 4, ".pdx")) return;

	size_t l = 0;
	uint8_t *pdx_data = load_file(filename, &l);

	if(opt_samples) {
		for(int i = 0; i < 96; i++) {
			uint32_t ofs = (pdx_data[i * 8] << 24) | (pdx_data[i * 8 + 1] << 16) | (pdx_data[i * 8 + 2] << 8) | pdx_data[i * 8 + 3];
			uint32_t len = (pdx_data[i * 8 + 4] << 24) | (pdx_data[i * 8 + 5] << 16) | (pdx_data[i * 8 + 6] << 8) | pdx_data[i * 8 + 7];
			if(len == 0) continue;
			if(ofs > l) continue;
			if(ofs + len > l) len = l - ofs;
			if(len == 0) continue;

			printf("%s\t%d\t%d\t%d\t", filename, i, ofs, len);

			struct md5_ctx ctx;
			md5_init_ctx(&ctx);
			md5_process_bytes(pdx_data + ofs, len, &ctx);
			uint8_t smplmd5buf[16];
			md5_finish_ctx(&ctx, smplmd5buf);
			for(int i = 0; i < 16; i++) {
				printf("%02x", smplmd5buf[i]);
			}
			printf("\n");
		}
	} else {
		int num_samples = 0;
		for(int i = 0; i < 96; i++) {
			uint32_t ofs = (pdx_data[i * 8] << 24) | (pdx_data[i * 8 + 1] << 16) | (pdx_data[i * 8 + 2] << 8) | pdx_data[i * 8 + 3];
			uint32_t len = (pdx_data[i * 8 + 4] << 24) | (pdx_data[i * 8 + 5] << 16) | (pdx_data[i * 8 + 6] << 8) | pdx_data[i * 8 + 7];
			if(len == 0) continue;
			if(ofs > l) continue;
			num_samples++;
		}

		printf("%s\t%zu\t%d\t", filename, l, num_samples);

		struct md5_ctx ctx;
		md5_init_ctx(&ctx);
		md5_process_bytes(pdx_data, l, &ctx);
		uint8_t md5buf[16];
		md5_finish_ctx(&ctx, md5buf);
		for(int i = 0; i < 16; i++) {
			printf("%02x", md5buf[i]);
		}
		printf("\n");
	}

	free(pdx_data);
}

static void pdxinfo_dir(const char *name, int recurse) {
	DIR *dir;
	struct dirent *entry;

	if(!(dir = opendir(name)))
		return;

	while((entry = readdir(dir)) != NULL) {
		char path[1024];
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);

#ifdef DIRENT_HAS_D_TYPE
		if (entry->d_type == DT_DIR) {
#else
		struct stat st;
		stat(path, &st);
		if(S_ISDIR(st.st_mode)) {
#endif
			if(!recurse)
				continue;

			pdxinfo_dir(path, recurse);
		} else {
			snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
			pdxinfo(path);
		}
	}
	closedir(dir);
}

int main(int argc, char **argv) {
	int optind = cmdline_parse_args(argc, argv, (struct cmdline_option[]){
		{
			'o', "output",
			"Output file",
			"file",
			TYPE_REQUIRED,
			TYPE_STRING, &opt_output
		},
		{
			'r', "recursive",
			"Recurse directories",
			0,
			TYPE_SWITCH,
			TYPE_NONE, &opt_recursive
		},
		{
			'H', "header",
			"Print header",
			0,
			TYPE_SWITCH,
			TYPE_NONE, &opt_header
		},
		{
			's', "samples",
			"Print samples",
			0,
			TYPE_SWITCH,
			TYPE_NONE, &opt_samples
		},
		CMDLINE_ARG_TERMINATOR
	}, 1, 1, "<file.pdx>");

	if(optind < 0) exit(-optind);

	of = stdout;
	if(opt_output) of = fopen(opt_output, "w");

	if(opt_header) {
		if(opt_samples)
			printf("File\tSample #\tOffset\tLength\tMD5\n");
		else
			printf("File\tSize\tSamples\tMD5\n");
	}
	for(int i = optind; i < argc; i++) {
		struct stat st;
		stat(argv[i], &st);
		if(S_ISDIR(st.st_mode))
			pdxinfo_dir(argv[i], opt_recursive);
		else
			pdxinfo(argv[i]);
	}

	if(opt_output) fclose(of);

	return 0;
}

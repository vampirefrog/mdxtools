#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>

#include "tools.h"
#include "mdx.h"
#include "sjis.h"
#include "cmdline.h"
#include "md5.h"

int opt_utf8 = 0;
char *opt_output = 0;
int opt_recursive = 0;
int opt_header = 0;
int opt_opm = 0;
int opt_pcm = 0;
FILE *of;

static void hexdump(FILE *f, uint8_t *data, int len) {
	for(int i = 0; i < len; i++) {
		fprintf(f, "%02x", data[i]);
	}
}

static void run_through_file(struct mdx_file *f, int *num_cmds_out, int *pcm8_out, int *pcm_notes_out) {
	*pcm8_out = 0;
	memset(num_cmds_out, 0, 16 * sizeof(int));
	if(pcm_notes_out) memset(pcm_notes_out, 0, 96 * sizeof(int));
	for(int i = 0; i < f->num_channels; i++) {
		struct mdx_channel *chan = &f->channels[i];
//		printf("%d/%d len=%d\n", i, f->num_channels, chan->data_len);
		for(int j = 0; j < chan->data_len;) {
			int l = mdx_cmd_len(chan->data, j, chan->data_len - j);
			//printf("cmd=%02x j=%d len=%d calculated=%d l=%d\n", chan->data[j], j, chan->data_len, chan->data_len - j, l);
			if(l < 0) break;
			// stop on performance end command
			num_cmds_out[i]++;
			if(chan->data[j] == 0xf1 && j < chan->data_len -1 && chan->data[j+1] == 0) {
				break;
			}
			if(chan->data[j] == 0xe8) *pcm8_out = 1;
			if(pcm_notes_out && i >= 8 && chan->data[j] >= 0x80 && chan->data[j] <= 0xdf) {
				pcm_notes_out[chan->data[j] - 0x80]++;
			}
			j += l;
		}
	}
}

static void mdxinfo(const char *filename) {
	int len = strlen(filename);
	if(strcasecmp(filename + len - 4, ".mdx")) return;

	struct stat st;
	stat(filename, &st);
	char tmbuf[100];
	strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", localtime(&(st.st_mtime)));

	size_t l = 0;
	uint8_t *mdx_data = load_file(filename, &l);
	struct mdx_file f;
	int r = mdx_file_load(&f, mdx_data, l);

	if(opt_opm) {
		if(r != 0) goto freemdx;
		for(int i = 0; i < 256; i++) {
			if(f.voices[i]) {
				printf("%s\t", filename);
				printf("%d\t", i);
				uint8_t *v = f.voices[i] + 1;
				struct md5_ctx ctx;
				md5_init_ctx(&ctx);
				md5_process_bytes(v, 26, &ctx);
				uint8_t smplmd5buf[16];
				md5_finish_ctx(&ctx, smplmd5buf);
				for(int j = 0; j < 16; j++) {
					printf("%02x", smplmd5buf[j]);
				}
				printf("\t%d", *v++); // fl con
				printf("\t%d", *v++); // slot mask
				for(int j = 0; j < 4; j++) {
					for(int k = 0; k < 6; k++) {
						printf("\t%d", v[j+k*4]);
					}
				}
				printf("\n");
			}
		}
	} else if(opt_pcm) {
		int num_cmds[16];
		int pcm8 = 0;
		int pcm_notes[96]; // number of times each pcm sample was played
		run_through_file(&f, num_cmds, &pcm8, pcm_notes);
		for(int j = 0; j < 96; j++) {
			if(pcm_notes[j] > 0)
				printf("%s\t%d\t%d\n", filename, j, pcm_notes[j]);
		}
	} else {
		printf("%s\t%lu\t%lu\t%s\t%s\t", filename, l, l - f.data_start_ofs, tmbuf, mdx_error_name(r));

		struct md5_ctx ctx;
		uint8_t md5buf[16];

		// entire file MD5
		md5_init_ctx(&ctx);
		md5_process_bytes(mdx_data, l, &ctx);
		md5_finish_ctx(&ctx, md5buf);
		for(int j = 0; j < 16; j++) {
			printf("%02x", md5buf[j]);
		}
		printf("\t");

		// data only MD5
		md5_init_ctx(&ctx);
		md5_process_bytes(mdx_data + f.data_start_ofs, l - f.data_start_ofs, &ctx);
		md5_finish_ctx(&ctx, md5buf);
		for(int j = 0; j < 16; j++) {
			printf("%02x", md5buf[j]);
		}
		printf("\t");

		if(opt_utf8) {
			putchar('"');
			sjis_print_utf8_escaped(f.title, f.title_len);
			putchar('"');
			putchar('\t');
			if(f.pdx_filename_len) {
				putchar('"');
				sjis_print_utf8_escaped(f.pdx_filename, f.pdx_filename_len);
				putchar('"');
			}
			putchar('\t');

			if(f.pdx_filename_len) {
				char pdxbuf[512], pdxutf[512];
				f.pdx_filename[f.pdx_filename_len] = 0;
				int ul = sjis_to_utf8(f.pdx_filename, f.pdx_filename_len, (uint8_t *)pdxutf, sizeof(pdxutf));
				pdxutf[ul] = 0;
				find_pdx_file(filename, pdxutf, pdxbuf, sizeof(pdxbuf));
				printf("%s\t", pdxbuf);
			} else printf("\t");
		} else {
			putchar('"');
			sjis_print_escaped(f.title, f.title_len);
			putchar('"');
			putchar('\t');
			if(f.pdx_filename_len) {
				putchar('"');
				sjis_print_escaped(f.pdx_filename, f.pdx_filename_len);
				putchar('"');
			}
			putchar('\t');

			if(f.pdx_filename_len) {
				char pdxbuf[512];
				f.pdx_filename[f.pdx_filename_len] = 0;
				find_pdx_file(filename, (char *)f.pdx_filename, pdxbuf, sizeof(pdxbuf));
				printf("%s\t", pdxbuf);
			} else printf("\t");
		}

		hexdump(of, f.title, f.title_len);
		putchar('\t');
		hexdump(of, f.pdx_filename, f.pdx_filename_len);
		putchar('\t');

		if(r != 0) {
			printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\n");
			goto freemdx;
		}

		int num_cmds[16];
		int pcm8 = 0;
		run_through_file(&f, num_cmds, &pcm8, 0);

		printf("%d\t", f.num_channels);
		printf("%d\t", pcm8);

		for(int i = 0; i < 16; i++) {
			if(i < f.num_channels) {
				printf("%d\t", num_cmds[i]);
			} else printf("\t");
		}
		printf("\n");
	}

freemdx:
	free(mdx_data);
}

static void mdxinfo_dir(const char *name, int recurse) {
	DIR *dir;
	struct dirent *entry;

	if(!(dir = opendir(name)))
		return;

	while((entry = readdir(dir)) != NULL) {
		char path[1024];
		if (entry->d_type == DT_DIR) {
			if(!recurse)
				continue;
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
			mdxinfo_dir(path, recurse);
		} else {
			snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
			mdxinfo(path);
		}
	}
	closedir(dir);
}

int main(int argc, char **argv) {
	int optind = cmdline_parse_args(argc, argv, (struct cmdline_option[]){
		{
			'u', "utf8",
			"Convert to UTF-8",
			0,
			TYPE_SWITCH,
			0, &opt_utf8
		},
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
			'y', "opm",
			"Print OPM voices",
			0,
			TYPE_SWITCH,
			TYPE_NONE, &opt_opm
		},
		{
			'p', "pcm",
			"Print PCM samples",
			0,
			TYPE_SWITCH,
			TYPE_NONE, &opt_pcm
		},
		CMDLINE_ARG_TERMINATOR
	}, 1, 0, "<file.mdx>");

	if(optind < 0) exit(-optind);

	of = stdout;
	if(opt_output) of = fopen(opt_output, "w");

	if(opt_header) {
		if(opt_opm) {
			printf("File\tVoice\tMD5\tFL CON\tSlot Mask");
			const char *operators[] = { "M1", "M2", "C1", "C2" };
			const char *regs[] = { "DT1 MUL", "TL", "KS AR", "AME D1R", "DT2 D2R", "D1L RR" };
			for(int i = 0; i < 4; i++)
				for(int j = 0; j < 6; j++)
					printf("\t%s %s", operators[i], regs[j]);
			printf("\n");
		} else if(opt_pcm) {
			printf("File\tSample\tTimes Used\n");
		} else {
			printf("File\tSize\tData size\tDate\tError\tMD5\tData MD5\tTitle\tPCM File\tPDX file\tTitle hex\tPCM File hex\tChannels\tPCM8\tA\tB\tC\tD\tE\tF\tG\tH\tP\tQ\tR\tS\tT\tU\tV\tW\n");
		}
	}
	for(int i = optind; i < argc; i++) {
		struct stat st;
		stat(argv[i], &st);
		if(S_ISDIR(st.st_mode))
			mdxinfo_dir(argv[i], opt_recursive);
		else
			mdxinfo(argv[i]);
	}

	if(opt_output) fclose(of);

	return 0;
}

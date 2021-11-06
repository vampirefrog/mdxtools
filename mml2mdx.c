#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "mdx_compiler.h"
#include "cmdline.h"
#include "tools.h"

int opt_stats = 0;
char *opt_output = 0;

struct mdx_compiler compiler;

int main(int argc, char **argv) {
	int optind = cmdline_parse_args(argc, argv, (struct cmdline_option[]){
		{
			's', "stats",
			"Dump statistics about compilation",
			0,
			TYPE_SWITCH,
			TYPE_INT, &opt_stats
		},
		{
			'o', "output",
			"Choose output file",
			"file",
			TYPE_REQUIRED,
			TYPE_STRING, &opt_output
		},
		CMDLINE_ARG_TERMINATOR
	}, 1, 1, "<file.mml>");

	if(optind < 0) {
		return -optind;
	}

	if(opt_output && argc - optind > 1) {
		fprintf(stderr, "When specifying output file, please specify only one input file.\n");
		return 1;
	}

	for(int i = optind; i < argc; i++) {
		FILE *f = fopen(argv[i], "r");
		if(!f) {
			fprintf(stderr, "Could not open %s: %s\n", argv[i], strerror(errno));
			continue;
		}

		mdx_compiler_init(&compiler);

		int y = mdx_compiler_parse(&compiler, f);
		if(y == 1) {
			fprintf(stderr, "parsing failed: invalid input\n");
		} else if(y == 2) {
			fprintf(stderr, "parsing failed: memory exhausted\n");
		} else {
			if(opt_stats)
				mdx_compiler_dump(&compiler);
			if(!opt_output || !opt_output[0]) {
				char mdxbuf[PATH_MAX];
				replace_ext(mdxbuf, sizeof(mdxbuf), argv[i], "mdx");
				opt_output = mdxbuf;
			}
			mdx_compiler_save(&compiler, opt_output);
		}

		mdx_compiler_destroy(&compiler);
	}

	return 0;
}

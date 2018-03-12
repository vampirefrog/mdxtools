#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "cmdline.h"

void cmdline_print_help(char *argv0, struct cmdline_option *opts, char *args_text) {
	printf("Usage: %s [options] %s\n", argv0, args_text);
	for(struct cmdline_option *opt = opts; opt->option_type != TYPE_END; opt++) {
		printf("\t");
		int l = 0;
		if(opt->opt_char) {
			printf("-%c", opt->opt_char);
			l += 2;
		}
		if(opt->opt_char && opt->opt_long) {
			printf(", ");
			l += 2;
		}
		if(opt->opt_long) {
			printf("--%s", opt->opt_long);
			l += 2 + strlen(opt->opt_long);
		}
		if(opt->option_type == TYPE_REQUIRED) {
			char *arg_name = opt->arg_name ? opt->arg_name : "X";
			printf(" <%s>", arg_name);
			l += 3 + strlen(arg_name);
		}
		else if(opt->option_type == TYPE_OPTIONAL) {
			char *arg_name = opt->arg_name ? opt->arg_name : "X";
			printf(" [%s]", arg_name);
			l += 3 + strlen(arg_name);
		}
#define ALIGN 20
		if(l < ALIGN)
			for(int i = ALIGN; i > l; i--)
				printf(" ");
		if(opt->opt_help) printf("    %s\n", opt->opt_help);
	}
}

static void cmdline_set_option_argument(struct cmdline_option *opt, char *str) {
	switch(opt->option_argument_type) {
		case TYPE_INT:
			if(opt->target)
				*((int *)opt->target) = strtol(str, 0, 0);
			break;
		case TYPE_FLOAT:
			if(opt->target)
				*((float *)opt->target) = strtof(str, 0);
			break;
		case TYPE_STRING:
			if(opt->target)
				*((char **)opt->target) = str;
			break;
		default:
			break;
	}
}

int cmdline_parse_args(int argc, char **argv, struct cmdline_option *opts, int num_required, int max_args, char *args_text) {
	struct cmdline_option *expecting_opt = 0;
	int first_nonopt = 1;
	int stop_parsing = 0;
	for(int i = 1; i < argc; i++) {
		if(argv[i][0] == '-' && argv[i][1] != 0 && !stop_parsing) {
			if(argv[i][1] == '-' && argv[i][2] == 0) {
				stop_parsing = 1;
			} else if((argv[i][1] == 'h' && argv[i][2] == 0) || !strcmp(argv[i], "--help")) {
				cmdline_print_help(argv[0], opts, args_text);
				return -1;
			} else {
				if(expecting_opt && expecting_opt->option_type == TYPE_REQUIRED)
					fprintf(stderr, "Required argument not found: %s\n", argv[first_nonopt-1]);

				expecting_opt = 0;
				int found = 0;
				for(struct cmdline_option *opt = opts; opt->option_type != TYPE_END; opt++) {
					if(opt->opt_char > 0 && argv[i][1] == opt->opt_char) {
						if(opt->option_type == TYPE_SWITCH) {
							if(opt->target) *((int *)opt->target) = 1;
						} else if(argv[i][2] != 0) {
							cmdline_set_option_argument(opt, &argv[i][2]);
						} else {
							expecting_opt = opt;
						}
						found = 1;
					} else if(opt->opt_long && argv[i][1] == '-' && !strcmp(&argv[i][2], opt->opt_long)) {
						if(opt->option_type == TYPE_SWITCH) {
							if(opt->target) *((int *)opt->target) = 1;
						} else {
							expecting_opt = opt;
						}
						found = 1;
					}
				}
				if(!found) {
					fprintf(stderr, "Unknown option: %s\n", argv[i]);
				}
			}
		} else if(expecting_opt) {
			cmdline_set_option_argument(expecting_opt, argv[i]);
			expecting_opt = 0;
		} else continue; // non-opt opt, just skip over it and don't execute code below

		char *curarg = argv[i];
		for(int j = i-1; j >= first_nonopt; j--) {
			argv[j+1] = argv[j];
		}
		argv[first_nonopt] = curarg;
		first_nonopt++;
	}
	if(argc - first_nonopt < num_required) {
		cmdline_print_help(argv[0], opts, args_text);
		return -1;
	}
	return first_nonopt;
}

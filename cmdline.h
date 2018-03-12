#ifndef CMDLINE_H_
#define CMDLINE_H_

struct cmdline_option {
	int opt_char;
	char *opt_long;
	char *opt_help;
	char *arg_name;
	enum {
		TYPE_END = 0,
		TYPE_SWITCH,
		TYPE_REQUIRED,
		TYPE_OPTIONAL
	} option_type;
	enum {
		TYPE_NONE,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_STRING
	} option_argument_type;
	void *target;
};

#define CMDLINE_ARG_TERMINATOR { 0, 0, 0, 0, TYPE_END, 0, 0 }

int cmdline_parse_args(int argc, char **argv, struct cmdline_option opts[], int num_required_args, int max_args, char *args_text);

#endif /* CMDLINE_H_ */

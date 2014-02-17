#include "MDXMML.h"

int main(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		try {
			printf("/* %s */\n", argv[i]);
			MDXMML m(argv[i]);
		} catch(exceptionf *e) {
			printf("Exception caught: %s\n", e->what());
		}
	}
	return 0;
}

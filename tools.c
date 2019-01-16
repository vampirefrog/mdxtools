#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#ifndef __EMSCRIPTEN__
#include <zlib.h>
#endif /* __EMSCRIPTEN */
#include "tools.h"

uint8_t *load_file(const char *filename, size_t *size_out) {
	FILE *f = fopen(filename, "rb");
	if(!f) {
		fprintf(
			stderr,
			"Could not open %s: %s (%d)\n",
			filename,
			strerror(errno),
			errno
		);
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	uint8_t *data = malloc(size);
	if(!data) {
		fprintf(
			stderr,
			"Could not allocate %lu bytes for %s: %s (%d)\n",
			size, filename, strerror(errno), errno
		);
		fclose(f);
		return NULL;
	}
	rewind(f);
	fread(data, 1, size, f);
	fclose(f);

	if(size_out) *size_out = size;
	return data;
}

#ifndef __EMSCRIPTEN__
uint8_t *load_gzfile(const char *filename, size_t *size_out) {
	gzFile f = gzopen(filename, "rb");
	if(!f) {
		fprintf(
			stderr,
			"Could not open %s: %s (%d)\n",
			filename,
			strerror(errno),
			errno
		);
		return NULL;
	}

	uint8_t *data = malloc(1024);
	if(!data) {
		fprintf(
			stderr,
			"Could not allocate 1024 bytes for %s: %s (%d)\n",
			filename, strerror(errno), errno
		);
		gzclose(f);
		return NULL;
	}
	size_t size = 0;
	while(1) {
		int r = gzread(f, data + size, 1024);
		if(r < 0) break;
		size += r;
		if(r == 1024) {
			data = realloc(data, size + 1024);
			if(!data) {
				fprintf(
					stderr,
					"Could not reallocate from %lu bytes to %lu bytes for %s: %s (%d)\n",
					(unsigned long)size, (unsigned long)size + 1024, filename, strerror(errno), errno
				);
				gzclose(f);
				return NULL;
			}
		} else break;
	}
	gzclose(f);

	if(size_out) *size_out = size;
	return data;
}
#endif /* __EMSCRIPTEN */


int gcd(int a, int b) {
	int c = a % b;

	while(c > 0) {
		a = b;
		b = c;
		c = a % b;
	}

	return b;
}
int find_pdx_file(const char *mdx_file_path, const char *pdx_filename, char *out, int out_len) {
	char *s = strdup(mdx_file_path);
	char *d = dirname(s);

	char buf[256];
	struct stat st;

	*out = 0;

	for(int i = 0; i < 2; i++) {
		if(i == 0) {
			snprintf(buf, sizeof(buf), "%s", pdx_filename);
		} else {
			snprintf(buf, sizeof(buf), "%s.PDX", pdx_filename);
		}
		int r = stat(buf, &st);
		if(r == 0) {
			strncpy(out, buf, out_len);
			goto good;
		}

		int found = 0;
		struct dirent **namelist;
		int n = scandir(d, &namelist, NULL, alphasort);
		while(n--) {
			if(!found && !strcasecmp(namelist[n]->d_name, buf)) {
				snprintf(out, out_len, "%s/%s", d, namelist[n]->d_name);
				found = 1;
			}
			free(namelist[n]);
		}
		free(namelist);

		if(found)
			goto good;
	}

good:
	free(s);
	return 0;
}

void csv_quote(char *str, size_t len) {
	if(len == 0) len = strlen(str);

	if(str == 0) {
		putchar('\\');
		putchar('N');
		return;
	}

	putchar('"');
	for(int i = 0; i < len; i++) {
		switch(str[i]) {
			case 0:
				putchar('\\');
				putchar(0);
				break;
			case '\\':
				putchar('\\');
				putchar('\\');
				break;
			case '\b':
				putchar('\\');
				putchar('b');
				break;
			case '\n':
				putchar('\\');
				putchar('n');
				break;
			case '\r':
				putchar('\\');
				putchar('r');
				break;
			case '\t':
				putchar('\\');
				putchar('t');
				break;
			case 26:
				putchar('\\');
				putchar('Z');
				break;
			case '"':
				putchar('"');
				putchar('"');
				break;
			default: putchar(str[i]);
		}
	}
	putchar('"');
}

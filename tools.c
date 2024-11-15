#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>
#ifndef WIN32
#include <unistd.h>
#ifndef __EMSCRIPTEN__
#include <zlib.h>
#endif /* __EMSCRIPTEN */
#endif
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
			"Could not allocate %zu bytes for %s: %s (%d)\n",
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

#ifdef __MINGW32__
int alphasort(const struct dirent **a, const struct dirent **b) {
	return strcmp((*a)->d_name, (*b)->d_name);
}
#endif

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
	char *dn = dirname(s);

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

		DIR *d;
		struct dirent *dir;
		d = opendir(dn);
		int found = 0;
		if(d) {
			while((dir = readdir(d)) != NULL) {
				if(!strcasecmp(dir->d_name, buf)) {
					snprintf(out, out_len, "%s/%s", dn, dir->d_name);
					found = 1;
				}
			}
			closedir(d);
		}

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

void hex_dump(const uint8_t *data, size_t len) {
	for(size_t i = 0; i < len; i++) printf("%02x", data[i]);
}

int replace_ext(char *out, size_t out_size, const char *in, const char *ext) {
	int inlen = strlen(in);
	int extlen = strlen(ext);
	if(out_size < inlen + extlen + 2)
		return -1;
	strncpy(out, in, out_size);
	char *b = basename(out);
	char *pp = strrchr(b, '.');
	if(!pp)
		pp = b + strlen(b);
	if(pp - out + extlen + 1 > out_size)
		extlen = out_size - (pp - out + 1);
	if(extlen > 0)
		strncpy(pp + 1, ext, extlen+1);

	return 0;
}

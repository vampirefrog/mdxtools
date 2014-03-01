#include <stdlib.h>
#include <string.h>
#include <iconv.h>

#include <stdio.h>
#include <errno.h>

#include "tools.h"

char *iconvAlloc(const char *str, const char *to, const char *from, int len) {
	int chunk_size = 256, alloc_size = 0;
	if(len < 0) {
		char *p = (char *)memchr(str, 0, 512); // FIXME: rawmemchr not available on MinGW?
		len = p - str;
	}
	iconv_t cd = iconv_open(to, from);
	if(cd < 0) {
		fprintf(stderr, "Could not open iconv from=%s to=%s\n", from, to);
		return NULL;
	}
	char *ret = 0;
	size_t inbytesleft = len, outbytesleft = 0;
	char *inbuf = (char *)str;
	while(inbytesleft > 0) {
		alloc_size += chunk_size;
		ret = (char  *)realloc(ret, alloc_size+8);
		outbytesleft += chunk_size;
		char *outbuf = ret + alloc_size - chunk_size;
		memset(outbuf, 0, chunk_size+8);
		iconv(cd, NULL, NULL, &outbuf, &outbytesleft); // flush any shift sequences
		iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
		if(errno == EINVAL || errno == EILSEQ) {
			free(ret);
			return NULL;
		}
	}
	iconv_close(cd);
	return ret;
}

void hexDump(const uint8_t *data, size_t len) {
	for(size_t i = 0; i < len; i++) printf("%02x", data[i]);
	printf("\n");
}
#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"

void buffer_init(struct buffer *buf) {
	buf->data = malloc(1024);
	buf->data_len = 0;
	buf->allocated_len = 1024;
}

void buffer_put(struct buffer *buf, uint8_t b) {
	if(buf->data_len >= buf->allocated_len) {
		buf->allocated_len += 1024;
		buf->data = realloc(buf->data, buf->allocated_len);
	}
	buf->data[buf->data_len++] = b;
}

void buffer_dump(struct buffer *buf) {
	for(int i = 0; i < buf->data_len; i++) {
		printf("%02x ", buf->data[i]);
		if(i > 0 && !(i & 0x0f)) printf("\n");
	}
	printf("\n");
}

int buffer_reserve(struct buffer *buf, int len) {
	if(buf->data_len + len > buf->allocated_len) {
		buf->allocated_len = (buf->data_len + len + 1023) & ~0x3ff;
		buf->data = realloc(buf->data, buf->allocated_len);
		if(!buf->data)
			return -1;
	}

	return 0;
}

int buffer_write(struct buffer *buf, uint8_t *data, int len) {
	buffer_reserve(len);

	memcpy(buf->data + buf->data_len, data, len);
	buf->data_len += len;

	return len;
}

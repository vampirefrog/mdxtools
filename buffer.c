#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"

void buffer_init(struct buffer *buf) {
	buf->data = malloc(1024);
	buf->data_len = 0;
	buf->allocated_len = 1024;
}

void buffer_destroy(struct buffer *buf) {
	if(buf->data)
		free(buf->data);
	buf->data = 0;
	buf->data_len = 0;
	buf->allocated_len = 0;
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
	buffer_reserve(buf, len);

	memcpy(buf->data + buf->data_len, data, len);
	buf->data_len += len;

	return len;
}

int buffer_write_buffer(struct buffer *buf, struct buffer *from) {
	buffer_reserve(buf, from->data_len);

	memcpy(buf->data + buf->data_len, from->data, from->data_len);
	buf->data_len += from->data_len;

	return from->data_len;
}

void buffer_write_uint8(struct buffer *buf, uint8_t b) {
	if(buf->data_len >= buf->allocated_len) {
		buf->allocated_len += 1024;
		buf->data = realloc(buf->data, buf->allocated_len);
	}
	buf->data[buf->data_len++] = b;
}

void buffer_write_big_int16(struct buffer *buf, int16_t i) {
	buffer_write_uint8(buf, i >> 8);
	buffer_write_uint8(buf, i);
}

void buffer_write_big_uint16(struct buffer *buf, uint16_t i) {
	buffer_write_uint8(buf, i >> 8);
	buffer_write_uint8(buf, i);
}

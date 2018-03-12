#pragma once

#include <stdint.h>

struct buffer {
	uint8_t *data;
	int data_len;
	int allocated_len;
};

void buffer_init(struct buffer *buf);
void buffer_put(struct buffer *buf, uint8_t b);
void buffer_write(struct buffer *buf, uint8_t *data, int len);
void buffer_dump(struct buffer *buf);
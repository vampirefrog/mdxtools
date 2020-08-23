#pragma once

#include <stdint.h>

struct buffer {
	uint8_t *data;
	int data_len;
	int allocated_len;
};

void buffer_init(struct buffer *buf);
void buffer_destroy(struct buffer *buf);
void buffer_put(struct buffer *buf, uint8_t b);
int buffer_reserve(struct buffer *buf, int len);
int buffer_write(struct buffer *buf, uint8_t *data, int len);
int buffer_write_buffer(struct buffer *buf, struct buffer *from);
void buffer_dump(struct buffer *buf);
void buffer_write_big_uint32(struct buffer *buf, uint32_t i);
void buffer_write_big_int32(struct buffer *buf, uint32_t i);
void buffer_write_big_uint16(struct buffer *buf, uint16_t i);
void buffer_write_big_int16(struct buffer *buf, int16_t i);
void buffer_write_uint8(struct buffer *buf, uint8_t i);
void buffer_write_little_uint32(struct buffer *buf, uint32_t i);
void buffer_write_little_int32(struct buffer *buf, uint32_t i);
void buffer_write_little_uint16(struct buffer *buf, uint16_t i);
void buffer_write_little_int16(struct buffer *buf, uint16_t i);

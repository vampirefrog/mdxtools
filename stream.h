#pragma once

#include <stdint.h>
#include <stdio.h>

struct stream {
	size_t position;
	int _errno;

	size_t (*read)(struct stream *, void *ptr, size_t size);
	size_t (*seek)(struct stream *, long offset, int whence);
	int (*eof)(struct stream *);
	long (*tell)(struct stream *);
};

struct read_stream;

struct read_stream {
	struct stream stream;
};

int stream_seek(struct stream *stream, long offset, int whence);
uint8_t stream_read_uint8(struct stream *stream);
uint16_t stream_read_big_uint16(struct stream *stream);
uint32_t stream_read_big_uint32(struct stream *stream);
int stream_read_compare(struct stream *stream, const void *data, int len);
int stream_eof(struct stream *stream);
long stream_tell(struct stream *stream);

struct mem_read_stream {
	struct read_stream read_stream;

	uint8_t *data;
	size_t data_size;
};

int mem_read_stream_init(struct mem_read_stream *stream, uint8_t *data, size_t data_size);

struct file_read_stream {
	struct read_stream read_stream;

	FILE *f;
};

int file_read_stream_init(struct file_read_stream *stream, char *filename);

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include "stream.h"
#include "tools.h"

int stream_seek(struct stream *stream, long offset, int whence) {
	return stream->seek(stream, offset, whence);
}

int stream_read(struct stream *stream, void *ptr, size_t size) {
	return stream->read(stream, ptr, size);
}

int stream_eof(struct stream *stream) {
	return stream->eof(stream);
}

uint8_t stream_read_uint8(struct stream *stream) {
	uint8_t r;
	stream_read(stream, &r, 1);
	stream->_errno = errno;
	return r;
}

uint16_t stream_read_big_uint16(struct stream *stream) {
	uint8_t buf[2];
	stream_read(stream, buf, 2);
	return buf[0] << 16 | buf[1];
}

uint32_t stream_read_big_uint32(struct stream *stream) {
	uint8_t buf[4];
	stream_read(stream, buf, 4);
	return buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
}

int stream_read_compare(struct stream *stream, const void *data, int len) {
	if(!len) len = strlen((char *)data);
	uint8_t *buf = malloc(len);
	if(buf) {
		int ret = !(stream_read(stream, buf, len) < len || memcmp(buf, data, len));
		free(buf);
		return ret;
	}
	return 0;
}

size_t mem_read_stream_read(struct stream *stream, void *ptr, size_t size) {
	struct mem_read_stream *read_stream = (struct mem_read_stream *)stream;
	int read_len = MIN(read_stream->data_size - stream->position, size);
	memcpy(ptr, read_stream->data + stream->position, read_len);
	stream->position += read_len;
	stream->_errno = errno;
	return read_len;
}

size_t mem_read_stream_seek(struct stream *stream, long offset, int whence) {
	struct mem_read_stream *read_stream = (struct mem_read_stream *)stream;
	stream->position = MIN(offset, read_stream->data_size);
	stream->_errno = 0;
	return stream->position;
}

int mem_read_stream_eof(struct stream *stream) {
	struct mem_read_stream *read_stream = (struct mem_read_stream *)stream;
	stream->position = stream->position >= read_stream->data_size;
	return stream->position;
}

long mem_read_stream_tell(struct stream *stream) {
	return stream->position;
}

int mem_read_stream_init(struct mem_read_stream *stream, uint8_t *data, size_t data_size) {
	stream->data = data;
	stream->data_size = data_size;

	stream->read_stream.stream.read = mem_read_stream_read;
	stream->read_stream.stream.seek = mem_read_stream_seek;
	stream->read_stream.stream.eof  = mem_read_stream_eof;
	stream->read_stream.stream.tell  = mem_read_stream_tell;

	return 0;
}

size_t file_read_stream_read(struct stream *stream, void *ptr, size_t size) {
	struct file_read_stream *read_stream = (struct file_read_stream *)stream;
	size_t r = fread(ptr, 1, size, read_stream->f);
	stream->_errno = errno;
	return r;
}

size_t file_read_stream_seek(struct stream *stream, long offset, int whence) {
	struct file_read_stream *read_stream = (struct file_read_stream *)stream;
	size_t r = fseek(read_stream->f, offset, whence);
	stream->_errno = errno;
	return r;
}

int file_read_stream_eof(struct stream *stream) {
	struct file_read_stream *read_stream = (struct file_read_stream *)stream;
	int r = feof(read_stream->f);
	stream->_errno = errno;
	return r;
}

long file_read_stream_tell(struct stream *stream) {
	struct file_read_stream *read_stream = (struct file_read_stream *)stream;
	long r = ftell(read_stream->f);
	stream->_errno = errno;
	return r;
}

int file_read_stream_init(struct file_read_stream *stream, char *filename) {
	stream->f = fopen(filename, "rb");
	stream->read_stream.stream._errno = errno;
	if(!stream->f) return -1;

	stream->read_stream.stream.read = file_read_stream_read;
	stream->read_stream.stream.seek = file_read_stream_seek;
	stream->read_stream.stream.eof = file_read_stream_eof;
	stream->read_stream.stream.tell = file_read_stream_tell;

	return 0;
}


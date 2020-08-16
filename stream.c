#include <errno.h>
#include <stdint.h>
#include <string.h>
#include "stream.h"
#include "tools.h"

size_t stream_seek(struct stream *stream, long offset, int whence) {
	return stream->seek(stream, offset, whence);
}

int stream_read(struct stream *stream, void *ptr, size_t size) {
	return stream->read(stream, ptr, size);
}

int stream_eof(struct stream *stream) {
	return stream->eof(stream);
}

ssize_t stream_write(struct stream *stream, void *ptr, size_t size) {
	return stream->write(stream, ptr, size);
}

ssize_t stream_write_big_uint16(struct stream *stream, uint16_t i) {
	uint8_t buf[2] = { i >> 8, i & 0xff };
	return stream_write(stream, buf, 2);
}

ssize_t stream_write_big_uint32(struct stream *stream, uint32_t i) {
	uint8_t buf[4] = { i >> 24, i >> 16, i >> 8, i };
	return stream_write(stream, buf, 4);
}

ssize_t stream_write_buffer(struct stream *stream, struct buffer *buffer) {
	return stream_write(stream, buffer->data, buffer->data_len);
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

size_t mem_stream_write(struct stream *stream, void *ptr, size_t size) {
	struct mem_stream *mem_stream = (struct mem_stream *)stream;
	if(mem_stream->buffer->data_len < stream->position + size) {
		int err = buffer_reserve(mem_stream->buffer, stream->position + size - mem_stream->buffer->data_len);
		stream->_errno = errno;
		if(err) return 0;
		mem_stream->buffer->data_len += stream->position + size - mem_stream->buffer->data_len;
	}
	memcpy(mem_stream->buffer->data + stream->position, ptr, size);

	stream->position += size;
	stream->_errno = errno;
	return size;
}

size_t mem_stream_read(struct stream *stream, void *ptr, size_t size) {
	struct mem_stream *mem_stream = (struct mem_stream *)stream;
	int read_len = MIN(mem_stream->buffer->data_len - stream->position, size);
	memcpy(ptr, mem_stream->buffer->data + stream->position, read_len);
	stream->position += read_len;
	stream->_errno = errno;
	return read_len;
}

size_t mem_stream_seek(struct stream *stream, long offset, int whence) {
	struct mem_stream *mem_stream = (struct mem_stream *)stream;
	stream->position = MIN(offset, mem_stream->buffer->data_len);
	stream->_errno = 0;
	return stream->position;
}

int mem_stream_eof(struct stream *stream) {
	struct mem_stream *mem_stream = (struct mem_stream *)stream;
	stream->position = stream->position >= mem_stream->buffer->data_len;
	return stream->position;
}

long mem_stream_tell(struct stream *stream) {
	return stream->position;
}

int mem_stream_init(struct mem_stream *stream, struct buffer *buffer) {
	if(!buffer)
		return 1;

	stream->buffer = buffer;
	stream->stream.write = mem_stream_write;
	stream->stream.read = mem_stream_read;
	stream->stream.seek = mem_stream_seek;
	stream->stream.eof  = mem_stream_eof;
	stream->stream.tell = mem_stream_tell;

	return 0;
}

size_t file_stream_read(struct stream *stream, void *ptr, size_t size) {
	struct file_stream *file_stream = (struct file_stream *)stream;
	size_t r = fread(ptr, 1, size, file_stream->f);
	stream->_errno = errno;
	return r;
}

size_t file_stream_write(struct stream *stream, void *ptr, size_t size) {
	struct file_stream *write_stream = (struct file_stream *)stream;
	size_t r = fwrite(ptr, 1, size, write_stream->f);
	stream->_errno = errno;
	return r;
}

size_t file_stream_seek(struct stream *stream, long offset, int whence) {
	struct file_stream *file_stream = (struct file_stream *)stream;
	size_t r = fseek(file_stream->f, offset, whence);
	stream->_errno = errno;
	return r;
}

int file_stream_eof(struct stream *stream) {
	struct file_stream *file_stream = (struct file_stream *)stream;
	int r = feof(file_stream->f);
	stream->_errno = errno;
	return r;
}

long file_stream_tell(struct stream *stream) {
	struct file_stream *file_stream = (struct file_stream *)stream;
	long r = ftell(file_stream->f);
	stream->_errno = errno;
	return r;
}

int file_stream_init(struct file_stream *stream, char *filename, const char *mode) {
	stream->f = fopen(filename, mode);
	stream->stream._errno = errno;
	if(!stream->f) return -1;

	stream->stream.read = file_stream_read;
	stream->stream.write = file_stream_write;
	stream->stream.seek = file_stream_seek;
	stream->stream.eof = file_stream_eof;
	stream->stream.tell = file_stream_tell;

	return 0;
}

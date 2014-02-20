#ifndef FILESTREAM_H__
#define FILESTREAM_H__

// Just a simple layer to make life easy for reading ints and whatnot

#include <zlib.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <iconv.h>

#include "exceptionf.h"
#include "Buffer.h"

struct FileStream {
	gzFile zf;
	FILE *f;

	FileStream() { f = 0; }
	FileStream(const char *filename, const char *mode="rb") {
		this->open(filename, mode);
	}
	~FileStream() {
		this->close();
	}

	void open(const char *filename, const char *mode="rb") {
		zf = 0;
		if(strchr(mode, 'r')) {
			f = fopen(filename, mode);
			if(!f) throw new exceptionf("Could not open %s: %s (%d)", filename, strerror(errno), errno);
			char head[2];
			if(fread(head, 1, 2, f) < 2) throw new exceptionf("Could not read signature bytes for %s.", filename);
			rewind(f);
			if(head[0] == 0x1f && head[1] == 0x8b) { // detected a gzip file
				zf = gzdopen(fileno(f), mode);
				if(!zf) throw new exceptionf("Could not open %s: %s (%d)", filename, strerror(errno), errno);
			}
		} else f = fopen(filename, mode);
	}
	void close() {
		if(zf) gzclose(zf);
		fclose(f);
	}
	z_off_t tell() {
		return zf ? gztell(zf) : ftell(f);
	}
	int eof() {
		return zf ? gzeof(zf) : feof(f);
	}

	bool readCompare(const char *chars, int len = 0) {
		if(len == 0) len = strlen(chars);
		char *buf = new char[len];
		bool ret = !(read(buf, len) < len || memcmp(buf, chars, len));
		delete[] buf;
		return ret;
	}
	z_off_t seek(z_off_t offset, int whence = SEEK_SET) {
		return zf ? gzseek(zf, offset, whence) : fseek(f, offset, whence);
	}
	int read(void *buf, size_t len) {
		return zf ? gzread(zf, buf, len) : fread(buf, 1, len, f);
	}
	uint32_t littleUint32(uint32_t i) {
		return ((i & 0xff) << 24) | ((i & 0xff00) << 8) | ((i & 0xff0000) >> 8) | ((i & 0xff000000) >> 24);
	}
	uint16_t littleUint16(uint16_t i) {
		return ((i & 0xff) << 8) | ((i & 0xff00) >> 8);
	}
	uint32_t readUint32Big() {
		uint32_t i;
		read(&i, sizeof(i));
		return ntohl(i);
	}
	uint32_t readUint32() {
		return littleUint32(readUint32Big());
	}
	uint16_t readUint16Big() {
		uint16_t i;
		read(&i, sizeof(i));
		return ntohs(i);
	}
	uint16_t readUint16() {
		return littleUint16(readUint16Big());
	}
	int16_t littleInt16(int16_t i) { return ((i & 0xff) << 8) | ((i & 0xff00) >> 8); }
	int16_t readInt16Big() { int16_t i; read(&i, sizeof(i)); return ntohs(i); }
	int16_t readInt16() { return littleInt16(readInt16Big()); }
	uint8_t readUint8() {
		return getc();
	}
	int getc() {
		return zf ? gzgetc(zf) : fgetc(f);
	}
	char *readLine(int stop = '\n') { // TODO: optimize
		char *s = new char[1];
		*s = 0;
		int c, l = 0;
		while((c = getc()) != EOF && c != stop) {
			char *ns = new char[l+2];
			memcpy(ns, s, l);
			delete[] s;
			s = ns;
			s[l++] = c;
			s[l] = 0;
		}
		return s;
	}
	const char *readLineIconv(int stop = '\n', const char *t = NULL, const char *f = NULL) { //TODO: optimize
		iconv_t i = iconv_open(f, t);
		char *line = readLine(stop);
		size_t linelen = strlen(line);
		size_t outlen = 256;
		char *out = new char[outlen];
		char *ret = out;
		iconv(i, &line, &linelen, &out, &outlen);
		out[0] = 0;
		iconv_close(i);
		return ret;
	}

	void write(const void *s, int len=-1) {
		if(len < 0) len = strlen((char *)s);
		if(zf) gzwrite(zf, (char *)s, len); else fwrite((char *)s, 1, len, f);
	}

	void write(Buffer &buf) {
		write(buf.data, buf.len);
	}

	void writeUint32(uint32_t i) {
		uint32_t n = littleUint32(ntohl(i));
		write(&n, sizeof(n));
	}
	void writeUint16(uint16_t i) {
		uint16_t n = littleUint16(ntohs(i));
		write(&n, sizeof(n));
	}
	void writeUint8(uint8_t i) {
		write(&i, 1);
	}
	void fill(size_t size, int c = 0) {
		unsigned char *buf = (unsigned char *)malloc(size);
		if(buf) {
			memset(buf, c, size);
			write(buf, size);
			free(buf);
		}
	}
};

#endif /* FILESTREAM_H__ */

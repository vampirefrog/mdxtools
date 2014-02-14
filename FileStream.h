#ifndef FILESTREAM_H__
#define FILESTREAM_H__

// Just a simple layer to make life easy for reading ints and whatnot

#include <zlib.h>
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <iconv.h>

#include "exceptionf.h"
#include "Buffer.h"

struct FileStream {
	gzFile f;
	
	FileStream() { f = 0; }
	FileStream(const char *filename, const char *mode="r") {
		this->open(filename, mode);
	}
	~FileStream() {
		this->close();
	}

	void open(const char *filename, const char *mode="r") {
		f = gzopen(filename, mode);
		if(!f) throw new exceptionf("Could not open %s: %s (%d)", filename, strerror(errno), errno);
	}
	void close() {
		gzclose(f);
	}
	z_off_t tell() {
		return gztell(f);
	}
	int eof() {
		return gzeof(f);
	}

	bool readCompare(const char *chars, int len = 0) {
		if(len == 0) len = strlen(chars);
		char *buf = new char[len];
		bool ret = !(read(buf, len) < len || memcmp(buf, chars, len));
		delete[] buf;
		return ret;
	}
	z_off_t seek(z_off_t offset, int whence = SEEK_SET) {
		return gzseek(f, offset, whence);
	}
	int read(void *buf, size_t len) {
		return gzread(f, buf, len);
	}
	uint32_t littleUint32(uint32_t i) {
		return ((i & 0xff) << 24) | ((i & 0xff00) << 8) | ((i & 0xff0000) >> 8) | ((i & 0xff000000) >> 24);
	}
	uint16_t littleUint16(uint16_t i) {
		return ((i & 0xff) << 8) | ((i & 0xff00) >> 8);
	}
	uint32_t readUint32Big() {
		uint32_t i;
		gzread(f, &i, sizeof(i));
		return ntohl(i);
	}
	uint32_t readUint32() {
		return littleUint32(readUint32Big());
	}
	uint16_t readUint16Big() {
		uint16_t i;
		gzread(f, &i, sizeof(i));
		return ntohs(i);
	}
	uint16_t readUint16() {
		return littleUint16(readUint16Big());
	}
	int16_t littleInt16(int16_t i) { return ((i & 0xff) << 8) | ((i & 0xff00) >> 8); }
	int16_t readInt16Big() { int16_t i; gzread(f, &i, sizeof(i)); return ntohs(i); }
	int16_t readInt16() { return littleInt16(readInt16Big()); }
	uint8_t readUint8() {
		return gzgetc(f);
	}
	char *readLine(int stop = '\n') { // TODO: optimize
		char *s = new char[1];
		*s = 0;
		int c, l = 0;
		while((c = gzgetc(f)) != EOF && c != stop) {
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

	void write(const char *s, int len=-1) {
		if(len < 0) len = strlen(s);
		gzwrite(f, s, len);
	}

	void write(Buffer &buf) {
		gzwrite(f, buf.data, buf.len);
	}

	void writeUint32(uint32_t i) {
		uint32_t n = littleUint32(ntohl(i));
		gzwrite(f, &n, sizeof(n));
	}
	void writeUint16(uint16_t i) {
		uint32_t n = littleUint16(ntohs(i));
		gzwrite(f, &n, sizeof(n));
	}
	void writeUint8(uint8_t i) {
		gzwrite(f, &i, 1);
	}
	void fill(size_t size, int c = 0) {
		unsigned char *buf = (unsigned char *)malloc(size);
		if(buf) {
			memset(buf, c, size);
			gzwrite(f, buf, size);
			free(buf);
		}
	}
};

#endif /* FILESTREAM_H__ */

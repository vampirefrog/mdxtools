#ifndef STREAM_H_
#define STREAM_H_

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include "exceptionf.h"

class Stream {
public:
	size_t pos;
};

class Endian {
public:
	static uint32_t swap32(uint32_t i) {
		return ((i & 0xff) << 24) | ((i & 0xff00) << 8) | ((i & 0xff0000) >> 8) | ((i & 0xff000000) >> 24);
	}
	static uint16_t swap16(uint16_t i) {
		return ((i & 0xff) << 8) | ((i & 0xff00) >> 8);
	}

	static uint32_t toLittleUint32(uint32_t i) { return swap32(htonl(i)); }
	static uint16_t toLittleUint16(uint16_t i) { return swap16(htons(i)); }
	static int32_t toLittleInt32(int32_t i) { return swap32(htonl(i)); }
	static int16_t toLittleInt16(int16_t i) { return swap16(htons(i)); }

	static uint32_t fromLittleUint32(uint32_t i) { return swap32(ntohl(i)); }
	static uint16_t fromLittleUint16(uint16_t i) { return swap16(ntohs(i)); }
	static int32_t fromLittleInt32(int32_t i) { return swap32(ntohl(i)); }
	static int16_t fromLittleInt16(int16_t i) { return swap16(ntohs(i)); }

	static uint32_t toBigUint32(uint32_t i) { return htonl(i); }
	static uint16_t toBigUint16(uint16_t i) { return htons(i); }
	static int32_t toBigInt32(int32_t i) { return htonl(i); }
	static int16_t toBigInt16(int16_t i) { return htons(i); }

	static uint32_t fromBigUint32(uint32_t i) { return ntohl(i); }
	static uint16_t fromBigUint16(uint16_t i) { return ntohs(i); }
	static int32_t fromBigInt32(int32_t i) { return ntohl(i); }
	static int16_t fromBigInt16(int16_t i) { return ntohs(i); }
};

class ReadStream: public Stream {
public:
	virtual size_t read(void *p, size_t n) { return 0; } // Read at most n bytes or to EOF. Pass p=NULL to allocate.
	//virtual void *readUntil(uint8_t c, void *p, size_t n = 0) {} // Read at most n bytes until value c is met, or EOF. Pass p=NULL to allocate
	virtual uint8_t readUint8() { return 0; }
	uint16_t readBigUint16() { uint16_t i; read(&i, sizeof(i)); return Endian::fromBigUint16(i); }
	uint32_t readBigUint32() { uint32_t i; read(&i, sizeof(i)); return Endian::fromBigUint32(i); }
	int16_t readBigInt16() { int16_t i; read(&i, sizeof(i)); return Endian::fromBigInt16(i); }
	int32_t readBigInt32() { int32_t i; read(&i, sizeof(i)); return Endian::fromBigInt32(i); }
	uint16_t readLittleUint16() { uint16_t i; read(&i, sizeof(i)); return Endian::fromLittleUint16(i); }
	uint32_t readLittleUint32() { uint32_t i; read(&i, sizeof(i)); return Endian::fromLittleUint32(i); }
	int16_t readLittleInt16() { int16_t i; read(&i, sizeof(i)); return Endian::fromLittleInt16(i); }
	int32_t readLittleInt32() { int32_t i; read(&i, sizeof(i)); return Endian::fromLittleInt32(i); }
	bool readCompare(const char *str) { return readCompare(str, strlen(str)); }
	bool readCompare(const void *data, size_t len) {
		uint8_t *buf = new uint8_t[len];
		bool ret = !(read(buf, len) < len || memcmp(buf, data, len));
		delete[] buf;
		return ret;
	}
};

class Buffer;
class WriteStream: public Stream {
public:
	virtual size_t write(const void *p, size_t n) { return 0; }
	virtual void writeUint8(uint8_t i) { }
	virtual void fill(size_t size, int c = 0) {
		char buf[256];
		memset(buf, c, size);
		while(size) {
			size_t writeSize = sizeof(buf);
			if(size < writeSize) writeSize = size;
			write(buf, writeSize);
			size -= writeSize;
		}
	}
	size_t write(const char *p) { return write((void *)p, strlen(p)); }
	size_t write(Buffer &b);
	void writeBigUint16(uint16_t i)    { uint16_t w = Endian::toBigUint16(i);    write(&w, sizeof(w)); }
	void writeBigUint32(uint32_t i)    { uint32_t w = Endian::toBigUint32(i);    write(&w, sizeof(w)); }
	void writeBigInt16(int16_t i)      {  int16_t w = Endian::toBigInt16(i);     write(&w, sizeof(w)); }
	void writeBigInt32(int32_t i)      {  int32_t w = Endian::toBigInt32(i);     write(&w, sizeof(w)); }
	void writeLittleUint16(uint16_t i) { uint16_t w = Endian::toLittleUint16(i); write(&w, sizeof(w)); }
	void writeLittleUint32(uint32_t i) { uint32_t w = Endian::toLittleUint32(i); write(&w, sizeof(w)); }
	void writeLittleInt16(int16_t i)   {  int16_t w = Endian::toLittleInt16(i);  write(&w, sizeof(w)); }
	void writeLittleInt32(int32_t i)   {  int32_t w = Endian::toLittleInt32(i);  write(&w, sizeof(w)); }
};

class Buffer: public WriteStream {
public:
	uint8_t *data;
	size_t len, allocLen;
	size_t chunkSize;

	Buffer(): data(0), len(0), allocLen(0), chunkSize(1024) {}
	~Buffer() {
		free(data);
	}
	void extend(size_t nbytes) {
		len += nbytes;
		if(len > allocLen) {
			while(len > allocLen) allocLen += chunkSize;
			data = (uint8_t *)realloc(data, allocLen);
			if(!data) throw exceptionf("Could not reallocate %d bytes", allocLen);
		}
	}
	using WriteStream::write;
	size_t write(const void *p, size_t dataLen) {
		extend(dataLen);
		memcpy(data + len - dataLen, p, dataLen);
		return dataLen;
	}
	void writeUint8(uint8_t i) {
		extend(1);
		data[len-1] = i;
	}
	void clear() {
		free(data);
		data = 0;
		len = allocLen = 0;
	}
};

#endif /* STREAM_H_ */

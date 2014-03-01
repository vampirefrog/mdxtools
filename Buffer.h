#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdlib.h>
#include "exceptionf.h"

struct Buffer {
	uint8_t *data;
	size_t len, alloc_len;
	size_t chunk_size;

	Buffer(): data(0), len(0), alloc_len(0), chunk_size(1024) {}
	~Buffer() {
		free(data);
	}
	void extend(size_t nbytes) {
		len += nbytes;
		if(len > alloc_len) {
			while(len > alloc_len) alloc_len += chunk_size;
			data = (uint8_t *)realloc(data, alloc_len);
			if(!data) throw new exceptionf("Could not reallocate %d bytes", alloc_len);
		}
	}
	void putUint8(uint8_t b) {
		extend(1);
		data[len - 1] = b;
	}
	void putUint32(uint32_t i) {
		putUint8(i & 0xff);
		putUint8((i >> 8) & 0xff);
		putUint8((i >> 16) & 0xff);
		putUint8(i >> 24);
	}
	void putUint16(uint16_t i) {
		putUint8(i & 0xff);
		putUint8(i >> 8);
	}
	void put(uint8_t *d, size_t dataLen) {
		extend(dataLen);
		memcpy(data + len - dataLen, d, dataLen);
	}
};

#endif /* BUFFER_H_ */

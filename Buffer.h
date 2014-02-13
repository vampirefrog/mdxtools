#ifndef BUFFER_H_
#define BUFFER_H_

struct Buffer {
	uint8_t *data;
	uint32_t len, alloc_len;
	int chunk_size;

	Buffer() {
		data = 0;
		len = 0;
		chunk_size = 1024;
	}
	~Buffer() {
		if(data) free(data);
	}
	void extend(unsigned int nbytes) {
		len += nbytes;
		if(len > alloc_len) {
			alloc_len += chunk_size;
			data = (uint8_t *)realloc(data, alloc_len);
		}
	}
	void putUint8(uint8_t b) {
		extend(1);
		data[len - 1] = b;
	}
};

#endif /* BUFFER_H_ */

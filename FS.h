#ifndef FS_H_
#define FS_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <exception>
#include <zlib.h>

#include "Stream.h"
#include "exceptionf.h"

class FileOpenException: public std::exception {

};

class FSReadStream: public ReadStream {
public:
	virtual void open(const char *filename) {}
	void openCaseInsensitive(const char *filename) {
		return open(filename);
	}
	virtual void close() {}
	virtual int seek(size_t ofs, int whence = SEEK_SET) { return 0; }
	virtual bool eof() { return true; }
	virtual size_t tell() { return 0; }
	char *readLine(int stop = '\n') { // TODO: optimize
		char *s = new char[1];
		*s = 0;
		int c, l = 0;
		while((c = readUint8()) != stop && !eof()) {
			char *ns = new char[l+2];
			memcpy(ns, s, l);
			delete[] s;
			s = ns;
			s[l++] = c;
			s[l] = 0;
		}
		return s;
	}
};

class FileReadStream: public FSReadStream {
	FILE *f;
public:
	FileReadStream(const char *filename) { open(filename); }
	FileReadStream() {}
	~FileReadStream() { close(); }

	void open(const char *filename) {
		f = fopen(filename, "rb");
		if(!f) throw exceptionf("Could not open %s: %s (%d)", filename, strerror(errno), errno);
	}
	void close() { fclose(f); }
	size_t read(void *ptr, size_t len) { return fread(ptr, 1, len, f); }
	uint8_t readUint8() { return fgetc(f); }
	size_t tell() { return ftell(f); }
	int seek(size_t offset, int whence = SEEK_SET) { return fseek(f, offset, whence); }
	bool eof() { return feof(f); }
};

class ZFileReadStream: public FSReadStream {
	gzFile f;
public:
	ZFileReadStream(const char *filename) { throw FileOpenException(); open(filename); }
	ZFileReadStream() {}
	~ZFileReadStream() { close(); }

	void open(const char *filename) { f = gzopen(filename, "rb"); }
	void close() { gzclose(f); }
};

class FileWriteStream: public WriteStream {
	FILE *f;
public:
	FileWriteStream() {}
	FileWriteStream(const char *filename) {
		open(filename);
	}
	~FileWriteStream() { close(); }

	void open(const char *filename) {
		f = fopen(filename, "wb");
		if(!f) throw FileOpenException();
	}
	void close() {
		fclose(f);
	}
	using WriteStream::write;
	size_t write(const void *data, size_t s) {
		return fwrite(data, 1, s, f);
	}
	void writeUint8(uint8_t i) { fputc(i, f); }
};

class FS {
public:
	static bool fileExists(const char *filename) {
		struct stat st;
		if(stat(filename, &st) < 0) return false;
		if(S_ISREG(st.st_mode)) return true;
		return false;
	}
	static FSReadStream openRead(const char *filename) {
		try {
			return ZFileReadStream(filename);
		} catch(FileOpenException e) {
			return FileReadStream(filename);
		}
	}
};

#endif /* FS_H_ */

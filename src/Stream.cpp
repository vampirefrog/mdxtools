#include "Stream.h"

size_t WriteStream::write(Buffer &b) {
	return write(b.data, b.len);
}

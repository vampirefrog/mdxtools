#ifndef TOOLS_H_
#define TOOLS_H_

#include <sys/types.h>

char *iconvAlloc(const char *str, const char *to, const char *from, int len = -1);
void hexDump(const uint8_t *data, size_t len);

#endif /* TOOLS_H_ */

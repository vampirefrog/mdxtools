#ifndef TOOLS_H_
#define TOOLS_H_

#include <sys/types.h>

char *iconvAlloc(const char *str, const char *to, const char *from, int len = -1);
void hexDump(const uint8_t *data, size_t len);
char *replaceExtension(const char *str, const char *newExt, char *dst = NULL, size_t maxLen = 0);

#endif /* TOOLS_H_ */

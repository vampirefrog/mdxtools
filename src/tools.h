#ifndef TOOLS_H_
#define TOOLS_H_

#include <stdint.h>
#include <sys/types.h>

char *iconvAlloc(const char *str, const char *to, const char *from, int len = -1);
void hexDump(const uint8_t *data, size_t len);
/** Replace a filename extension
 *  if dst is not specified, an internal static buffer is returned
 * maxLen must be greater than the length of the new extension + 1 (otherwise it will overflow)
 */
char *replaceExtension(const char *str, const char *newExt, char *dst = NULL, size_t maxLen = 0);

#define MIN(a, b) ( (a) < (b) ? (a) : (b) )
#define MAX(a, b) ( (a) > (b) ? (a) : (b) )

char cleanChar(char c);


#endif /* TOOLS_H_ */

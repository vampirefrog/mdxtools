#ifndef SJIS_H_
#define SJIS_H_

#include <stdint.h>

int sjis_strlen(uint8_t *data, int len);
int sjis_to_utf8(uint8_t *sjis_data, int sjis_len, uint8_t *utf8_data, int utf8_len);
int utf8_to_sjis(uint8_t *utf8_data, int utf8_len, uint8_t *sjis_data, int sjis_len);
uint32_t sjis_char_to_unicode(uint16_t sjis);
uint16_t unicode_char_to_sjis(uint32_t unicode);

int sjis_print_utf8_escaped(uint8_t *sjis_data, int sjis_len);
int sjis_print_escaped(uint8_t *sjis_data, int sjis_len);
uint16_t jis_to_sjis(uint16_t val);

#endif /* SJIS_H_ */

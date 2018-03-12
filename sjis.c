#include <stdio.h>

#include "sjis.h"

int sjis_strlen(uint8_t *data, int len) {
	int l = 0;
	for(int i = 0; i < len; i++) {
		l++;
		if(data[i] >= 0x80) {
			l++;
			i++;
		}
	}
	return l;
}

int sjis_to_utf8(uint8_t *sjis_data, int sjis_len, uint8_t *utf8_data, int utf8_len) {
	if(sjis_len < 1) return 0;
	if(utf8_len < 1) return -1;

	int j = 0;
	for(int i = 0; i < sjis_len; i++) {
		uint32_t c;
		if((sjis_data[i] >= 0x80 && sjis_data[i] <= 0xa0) || (sjis_data[i] >= 0xe0 && sjis_data[i] <= 0xff)) {
			if(sjis_len - i < 2)
				return -1;
			c = sjis_char_to_unicode((sjis_data[i] << 8) | sjis_data[i+1]);
			i++;
		} else if(sjis_data[i] < 0x20) {
			c = sjis_data[i];
		} else {
			c = sjis_char_to_unicode(sjis_data[i]);
		}

		if(c < 0x80) {
			if(utf8_len - j < 1)
				return j;
			utf8_data[j++] = c;
		} else if(c < 0x800) {
			if(utf8_len - j < 2)
				return j;
			utf8_data[j++] = 192 + c / 64;
			utf8_data[j++] = 128 + c % 64;
		} else if(c - 0xd800u < 0x800) {
			return j;
		} else if(c < 0x10000) {
			if(utf8_len - j < 3)
				return j;
			utf8_data[j++] = 224 + c / 4096;
			utf8_data[j++] = 128 + c / 64 % 64;
			utf8_data[j++] = 128 + c % 64;
		} else if (c<0x110000) {
			if(utf8_len - j < 4)
				return j;
			utf8_data[j++] = 240+c/262144;
			utf8_data[j++] = 128+c/4096%64;
			utf8_data[j++] = 128+c/64%64;
			utf8_data[j++] = 128+c%64;
		} else return j;
	}
	return j;
}

static void put_escaped(uint8_t c) {
	switch(c) {
		case 0:
			putchar('\\');
			putchar('0');
			break;
		case '\b':
			putchar('\\');
			putchar('b');
			break;
		// case '\r':
		// 	putchar('\\');
		// 	putchar('r');
		// 	break;
		// case '\n':
		// 	putchar('\\');
		// 	putchar('n');
		// 	break;
		case '\t':
			putchar('\\');
			putchar('\t');
			break;
		case '\\':
			putchar('\\');
			putchar('\\');
			break;
		default:
			printf("\\0%02o", c);
	}
}

int sjis_print_utf8_escaped(uint8_t *sjis_data, int sjis_len) {
	if(sjis_len < 1) return 0;

	int j = 0;
	for(int i = 0; i < sjis_len; i++) {
		uint32_t c;
		if(sjis_data[i] >= 0xf0 && sjis_data[i] <= 0xf3) {
			continue;
		} else if((sjis_data[i] >= 0x80 && sjis_data[i] <= 0xa0) || (sjis_data[i] >= 0xe0 && sjis_data[i] <= 0xef)) {
			if(sjis_len - i < 2)
				return -1;
			c = sjis_char_to_unicode((sjis_data[i] << 8) | sjis_data[i+1]);
			i++;
		} else if(sjis_data[i] == '"') {
			putchar('\\');
			putchar('"');
		} else if(sjis_data[i] < 0x20) {
			put_escaped(sjis_data[i]);
			continue;
		} else {
			c = sjis_char_to_unicode(sjis_data[i]);
		}

		if(c < 0x80) {
			putchar(c);
		} else if(c < 0x800) {
			putchar(192 + c / 64);
			putchar(128 + c % 64);
		} else if(c - 0xd800u < 0x800) {
		} else if(c < 0x10000) {
			putchar(224 + c / 4096);
			putchar(128 + c / 64 % 64);
			putchar(128 + c % 64);
		} else if (c<0x110000) {
			putchar(240+c/262144);
			putchar(128+c/4096%64);
			putchar(128+c/64%64);
			putchar(128+c%64);
		}
	}
	return j;
}

int sjis_print_escaped(uint8_t *sjis_data, int sjis_len) {
	if(sjis_len < 1) return 0;

	int j = 0;
	for(int i = 0; i < sjis_len; i++) {
		if(sjis_data[i] >= 0xf0 && sjis_data[i] <= 0xf3) {
			putchar(sjis_data[i]);
			continue;
		} else if((sjis_data[i] >= 0x80 && sjis_data[i] <= 0xa0) || (sjis_data[i] >= 0xe0 && sjis_data[i] <= 0xef)) {
			if(sjis_len - i < 2)
				return -1;
			putchar(sjis_data[i]);
			putchar(sjis_data[i+1]);
			i++;
		} else if(sjis_data[i] < 0x20) {
			put_escaped(sjis_data[i]);
		} else {
			if(sjis_data[i] == '"')
				putchar('\\');
			putchar(sjis_data[i]);
		}
	}
	return j;
}

uint16_t jis_to_sjis(uint16_t val) {
	uint8_t j2 = val & 0xff;
	uint8_t j1 = val >> 8;
	uint8_t s1, s2;
	if(33 <= j1 && j1 <= 94)
		s1 = (j1 + 1) / 2 + 112;
	else
		s1 = (j1 + 1) / 2 + 176;
	if(j1 & 1)
		s2 = j2 + 31 + (j2 / 96);
	else
		s2 = j2 + 126;

	return (s1 << 8) | s2;
}

uint16_t jis_from_sjis(uint16_t val) {
	int status = 0;
	uint8_t j1 = 0;
	uint8_t j2 = 0;
	uint8_t b1 = val >> 8;
	uint8_t b2 = val & 0xff;
	if((b1 >= 0x81 && b1 <= 0x84) || (b1 >= 0x87 && b1 <= 0x9f)) {
		j1 = 2 * (b1 - 0x70) - 1;
		if (b2 >= 0x40 && b2 <= 0x9e) {
			j2 = b2 - 31;
			if (j2 > 95)
				j2 -= 1;
			status = 1;
		} else if (b2 >= 0x9f && b2 <= 0xfc) {
			j2 = b2 - 126;
			j1 += 1;
			status = 1;
		}
	} else if (b1 >= 0xe0 && b1 <= 0xef) {
		j1 = 2 * (b1 - 0xb0) - 1;
		if (b2 >= 0x40 && b2 <= 0x9e) {
			j2 = b2 - 31;
			if (j2 > 95)
				j2 -= 1;
			status = 1;
		} else if (b2 >= 0x9f && b2 <= 0xfc) {
			j2 = b2 - 126;
			j1 += 1;
			status = 1;
		}
	}

	if(status == 0) return 0;
	return (j1 << 8) | j2;
}

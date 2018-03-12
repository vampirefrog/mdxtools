#ifndef TOOLS_H_
#define TOOLS_H_

#include <stdlib.h>
#include <stdint.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

uint8_t *load_file(const char *filename, size_t *size_out);
uint8_t *load_gzfile(const char *filename, size_t *size_out);
int gcd(int a, int b); /* Greatest Common Divisor */
int find_pdx_file(const char *mdx_file_path, const char *pdx_filename, char *out, int out_len);
void csv_quote(char *str, size_t len);

// Execute fn for every file or for each file in every folder
// if recurse = 1, recurse subdirectories
// names and num_names can come from argv and argc
void each_file(const char **names, int num_names, void (*fn)(char *), int recurse);

#endif /* TOOLS_H_ */

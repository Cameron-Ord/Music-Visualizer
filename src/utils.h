#ifndef UTILS_H
#define UTILS_H
#include <stdarg.h>
#include <stdint.h>
#include "filesysdefs.h"

void free_ptrs(size_t size, ...);
size_t get_length(size_t size, ...);
char *find_char_str(char* search_key, Paths* buffer);
#endif

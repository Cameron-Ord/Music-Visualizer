#ifndef UTILS_H
#define UTILS_H
#include "filesysdefs.h"
#include <stdarg.h>
#include <stdint.h>

void free_ptrs(size_t size, ...);
size_t get_length(size_t size, ...);
const char *find_pathstr(const char *search_key, Paths *buffer);
#endif

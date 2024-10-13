#ifndef UTILS_H
#define UTILS_H
#include <stdarg.h>
#include <stdint.h>

void free_ptrs(size_t size, ...);
size_t get_length(size_t size, ...);

#endif

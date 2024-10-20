#ifndef UTILS_H
#define UTILS_H
#include "filesysdefs.h"
#include "fontdef.h"
#include <stdarg.h>
#include <stdint.h>

void free_ptrs(size_t size, ...);
size_t get_length(size_t size, ...);
void *free_text_buffer(TextBuffer *buf);
void *free_paths(Paths *buf);
const char *find_pathstr(const char *search_key, Paths *buffer);
#endif

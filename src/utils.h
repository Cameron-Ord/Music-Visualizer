#ifndef UTILS_H
#define UTILS_H
#include "filesysdefs.h"
#include "fontdef.h"
#include <stdarg.h>
#include <stdint.h>

void free_ptrs(size_t size, ...);
void SDL_ERR_CALLBACK(const char *msg);
void ERRNO_CALLBACK(const char *prefix, const char *msg);
size_t get_length(size_t size, ...);
void *free_text_buffer(TextBuffer *buf, const size_t *count);
void *free_paths(Paths *buf, const size_t *count);
const char *find_pathstr(const char *search_key, Paths *buffer);
#endif

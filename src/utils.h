#ifndef UTILS_H
#define UTILS_H
#include "filesysdefs.h"
#include "fontdef.h"
#include <stdarg.h>
#include <stdint.h>

#define DEFAULT_INPUT_BUFFER_SIZE 24
#define DEFAULT_FILTER_SIZE 12

void remove_char(char **buf, size_t *pos, size_t *size);
void append_char(const char *c, char **buf, size_t *pos, size_t *size);
void *destroy_search_text(Text *s);
void free_ptrs(size_t size, ...);
void SDL_ERR_CALLBACK(const char *msg);
void ERRNO_CALLBACK(const char *prefix, const char *msg);
size_t get_length(size_t size, ...);
void *free_text_buffer(TextBuffer *buf, const size_t *count);
void *free_paths(Paths *buf, const size_t *count);
const char *find_pathstr(const char *search_key, Paths *buffer);
Text *null_replace(size_t i, const size_t *filter_size,
                   TextBuffer *filtered_tb);
size_t determine_new_size(const size_t *size, TextBuffer *buf);
TextBuffer *zero_filter(TextBuffer *buf, const size_t *size);
size_t do_search(char *text_input_buf, const size_t *count,
                 const TextBuffer *base, TextBuffer **filtered);
#endif

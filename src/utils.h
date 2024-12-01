#ifndef UTILS_H
#define UTILS_H
#include "filesysdefs.h"
#include "fontdef.h"
#include "table.h"
#include <stdarg.h>
#include <stdint.h>


int get_sync_pos(unsigned char *buf);
int check_file(const char* path);
int min_titles(TextBuffer *tbuf);
void *scp(void *ptr);
int scc(int code);
int get_char_limit(int width);
void swap_font_ptrs(Table *table, const size_t key, TextBuffer *old_buffer,
                    TextBuffer *replace);
uint8_t determine_alpha(float amplitude);
SDL_Color determine_rgba(float phase, const SDL_Color *prim, uint8_t alpha);
int clamp_font_size(int size);
int not_empty(const char *string);
const char *sformat(char *string);
void free_ptrs(size_t size, ...);
void SDL_ERR_CALLBACK(const char *msg);
void ERRNO_CALLBACK(const char *prefix, const char *msg);
size_t get_length(size_t size, ...);
int valid_ptr(Paths *pbuf, TextBuffer *tbuf);
void *free_text_buffer(TextBuffer *buf, const size_t *count);
void *free_paths(Paths *buf, const size_t *count);
const char *find_pathstr(const char *search_key, Paths *buffer);
int find_type(const char *search_key, Paths *buffer);
#endif


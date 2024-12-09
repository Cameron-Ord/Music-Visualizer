#ifndef UTILS_H
#define UTILS_H
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int check_file_str(const char *str);
int get_char_limit(const int width, const int font_size);
int clamp_font_size(int size);
int not_empty(const char *string);
const char *sformat(char *string);
void free_ptrs(size_t size, ...);
void SDL_ERR_CALLBACK(const char *msg);
void ERRNO_CALLBACK(const char *prefix, const char *msg);
size_t get_length(size_t size, ...);
#endif

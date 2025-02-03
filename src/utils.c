#include "../inc/utils.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

const char *ext_literals[] = {".ogg", ".wav",  ".flac", ".aiff", "aif",
                              ".wv",  ".opus", ".caf",  ".mp3"};
const size_t size = sizeof(ext_literals) / sizeof(ext_literals[0]);

int check_file_str(const char *str) {
  for (size_t i = 0; i < size; i++) {
    if (strstr(str, ext_literals[i])) {
      return 1;
    }
  }

  return 0;
}

int get_char_limit(const int width, const int font_size) {
  if (width < 150) {
    return 2;
  }

  return MIN(200, MAX(3, width / font_size));
}

void SDL_ERR_CALLBACK(const char *msg) {
  fprintf(stderr, "SDL Error -> %s\n", msg);
}

void ERRNO_CALLBACK(const char *prefix, const char *msg) {
  fprintf(stderr, "%s -> %s\n", prefix, msg);
}

const char *sformat(char *str) {
  int i = 0;
  int j = 0;

  while (str[i] != '\0') {
    if (str[i] != ' ') {
      str[j] = tolower(str[i]);
      j++;
    }
    i++;
  }

  str[j] = '\0';
  return str;
}

size_t get_length(size_t size, ...) {
  va_list args;
  va_start(args, size);
  size_t total = 0;

  for (size_t i = 0; i < size; i++) {
    size_t str_length = va_arg(args, size_t);
    total += str_length;
  }

  return total;
}

void free_ptrs(size_t size, ...) {
  va_list args;
  va_start(args, size);
  for (size_t i = 0; i < size; i++) {
    void *ptr = va_arg(args, void *);
    if (ptr) {
      free(ptr);
      ptr = NULL;
    }
  }
}

int clamp_font_size(int size) {
  int min_size = 16;
  int max_size = 24;

  if (size > max_size) {
    size = max_size;
  }

  if (size < min_size) {
    size = min_size;
  }

  return size;
}

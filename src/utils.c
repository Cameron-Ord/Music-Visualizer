#include "utils.h"
#include <ctype.h>

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

/*
 Real ascii chars start at 32, which is the space key. So if a char is
 greater than this, pretty much means it isnt only spaces.
*/
int not_empty(const char *str) {
  int i = 0;

  while (str[i] != '\0') {
    if (str[i] > ' ') {
      return 1;
    }
    i++;
  }

  return 0;
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

// int min_titles(TextBuffer *t, int h) {
// int accumulator = TEXT_SPACING;
// int is_greater = 0;
//
// size_t j = 0;
// for (j = 0; j < t->size; j++) {
// if (t[j].text) {
// int rect_h = t[j].text->rect.h;
// accumulator += rect_h + TEXT_SPACING;
//}
//
//// If this check is met, return with the truthy flag before adding another
//// +1 to the max variable
// if (accumulator >= h) {
// return is_greater + 1;
//}
//
// if (t->max < (int)j) {
// t->max = j;
//}
//}
//
// return is_greater;
//}

#include "utils.h"
#include "fontdef.h"
#include "main.h"
#include "table.h"

#include <ctype.h>
#include <errno.h>
#include <sndfile.h>
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

int get_char_limit(int width) {
  return MIN(100, MAX(3, (width * 0.90) / FONT_SIZE));
}

void *scp(void *ptr) {
  if (!ptr) {
    fprintf(stderr, "SDL failed to create PTR! -> %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  return ptr;
}

int scc(int code) {
  if (code < 0) {
    fprintf(stderr, "SDL code execution failed! -> %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  return code;
}

void SDL_ERR_CALLBACK(const char *msg) {
  fprintf(stderr, "SDL Error -> %s\n", msg);
}

void ERRNO_CALLBACK(const char *prefix, const char *msg) {
  fprintf(stderr, "%s -> %s\n", prefix, msg);
}

void *free_paths(Paths *buf, const size_t *count) {
  if (!buf) {
    return NULL;
  }

  for (size_t i = 0; i < *count; i++) {
    if (buf[i].name) {
      free(buf[i].name);
      buf[i].name_length = 0;
    }

    if (buf[i].path) {
      free(buf[i].path);
      buf[i].path_length = 0;
    }
  }

  free(buf);
  return NULL;
}

void *free_text_buffer(TextBuffer *buf, const size_t *count) {
  if (!buf) {
    return NULL;
  }

  for (size_t i = 0; i < *count; i++) {
    if (buf[i].text) {
      Text *t = buf[i].text;
      if (t->name) {
        free(t->name);
      }

      if (t->tex[0]) {
        SDL_DestroyTexture(t->tex[0]);
      }

      if (t->tex[1]) {
        SDL_DestroyTexture(t->tex[1]);
      }

      free(t);
    }
  }

  free(buf);
  return NULL;
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

int find_type(const char *search_key, Paths *buffer) {
  if (buffer) {
    Paths *start = buffer;

    while (buffer != NULL) {
      if (strcmp(search_key, buffer->name) == 0) {
        return buffer->type;
      }
      buffer++;
    }

    buffer = start;
  }

  return UNKNOWN;
}

const char *find_pathstr(const char *search_key, Paths *buffer) {

  if (buffer) {
    Paths *start = buffer;

    while (buffer != NULL) {
      if (strcmp(search_key, buffer->name) == 0) {
        return buffer->path;
      }
      buffer++;
    }

    buffer = start;
  }

  return NULL;
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

void swap_font_ptrs(Table *table, const size_t key, TextBuffer *old_buffer,
                    TextBuffer *replace) {
  table_set_text(table, key, replace);

  if (old_buffer) {
    for (size_t i = 0; i < old_buffer->size; i++) {
      Text *invalidated = old_buffer[i].text;
      char *invalid_name = invalidated->name;
      SDL_Texture **invalid_tex = invalidated->tex;

      if (invalid_name) {
        free(invalid_name);
      }

      if (invalid_tex[0]) {
        SDL_DestroyTexture(invalid_tex[0]);
      }

      if (invalid_tex[1]) {
        SDL_DestroyTexture(invalid_tex[1]);
      }

      free(invalidated);
    }

    free(old_buffer);
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


int valid_ptr(Paths *p, TextBuffer *t) {
  if ((p && t) && (p->is_valid && t->is_valid)) {
    return 1;
  }

  return 0;
}

int min_titles(TextBuffer *t) {
  int h = win.height;

  int accumulator = TEXT_SPACING;
  int is_greater = 0;

  size_t j = 0;
  for (j = 0; j < t->size; j++) {
    if (t[j].text) {
      int rect_h = t[j].text->rect.h;
      accumulator += rect_h + TEXT_SPACING;
    }

    // If this check is met, return with the truthy flag before adding another
    // +1 to the max variable
    if (accumulator >= h) {
      return is_greater + 1;
    }

    if (t->max < (int)j) {
      t->max = j;
    }
  }

  return is_greater;
}

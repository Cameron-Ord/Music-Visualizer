#include "utils.h"
#include <stdlib.h>
#include <string.h>

void SDL_ERR_CALLBACK(const char *msg) {
  fprintf(stderr, "SDL Error -> %s\n", msg);
}

void ERRNO_CALLBACK(const char *prefix, const char *msg) {
  fprintf(stderr, "%s -> %s\n", prefix, msg);
}

void *free_paths(Paths *buf, const size_t *count) {
  for (size_t i = 0; i < *count; i++) {
    if (buf && buf[i].name) {
      free(buf[i].name);
      buf[i].name_length = 0;
    }

    if (buf && buf[i].path) {
      free(buf[i].path);
      buf[i].path_length = 0;
    }
  }

  if (buf) {
    free(buf);
  }

  return NULL;
}

void *free_text_buffer(TextBuffer *buf, const size_t *count) {
  for (size_t i = 0; i < *count; i++) {
    if (buf && buf[i].text) {
      Text *t = buf[i].text;
      if (t->is_valid && t->name) {
        free(t->name);
      }

      if (t->is_valid && t->tex[0]) {
        SDL_DestroyTexture(t->tex[0]);
      }

      if (t->is_valid && t->tex[1]) {
        SDL_DestroyTexture(t->tex[1]);
      }

      free(t);
    }
  }

  if (buf) {
    free(buf);
  }

  return NULL;
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

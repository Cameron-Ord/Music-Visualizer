#include "utils.h"
#include <stdlib.h>
#include <string.h>

void SDL_ERR_CALLBACK(const char *msg) {
  fprintf(stderr, "SDL Error -> %s\n", msg);
}

void ERRNO_CALLBACK(const char *prefix, const char *msg) {
  fprintf(stderr, "%s -> %s\n", prefix, msg);
}

size_t determine_new_size(const size_t *size, TextBuffer *buf) {
  for (size_t i = 0; i < *size; i++) {
    if (!buf[i].text) {
      // return the iter position as the size, since the current iter is null.
      // if we did i + 1 it would include the null value, which we dont want.
      return i;
    } else {
      printf("other %s\n", buf[i].text->name);
    }
  }

  return *size;
}

Text *null_replace(size_t i, const size_t *size, TextBuffer *buf) {
  for (size_t j = i; j < *size; j++) {
    if (buf[j].text) {
      Text *replace = buf[j].text;
      buf[j].text = NULL;
      return replace;
    }
  }

  return NULL;
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

TextBuffer *zero_filter(TextBuffer *buf, const size_t *size) {
  for (size_t i = 0; i < *size; i++) {
    buf[i].text = NULL;
  }

  return buf;
}

void *destroy_search_text(Text* s){
  if(s){
    free(s->name);
    SDL_DestroyTexture(s->tex[0]);
    free(s);
  }

  return NULL;
}

void do_search(char *text_input_buf, const size_t *count, size_t *filter_size,
               const TextBuffer *base, TextBuffer **filtered) {
  for (size_t i = 0; i < *count; i++) {
    if ((i + 1) > *filter_size) {
      *filter_size = *filter_size * 2;
      TextBuffer *tmp = realloc(*filtered, sizeof(TextBuffer) * *filter_size);
      if (!tmp) {
        ERRNO_CALLBACK("realloc failed!", strerror(errno));
        exit(EXIT_FAILURE);
      }
      *filtered = tmp;
    }

    char *result = strstr(base[i].text->name, text_input_buf);
    if (result) {
      (*filtered)[i].text = base[i].text;
    } else {
      (*filtered)[i].text = NULL;
    }
  }

  for (size_t i = 0; i < *filter_size; i++) {
    if (!(*filtered)[i].text) {
      (*filtered)[i].text = null_replace(i, filter_size, *filtered);
    }
  }

  //  *filter_size = determine_new_size(filter_size, *filtered);
  //  printf("new size: %zu\n", *filter_size);

  // if(*filter_size == 0){
  //   *filter_size = DEFAULT_FILTER_SIZE;
  // }

  // TextBuffer *tmp = realloc(*filtered, sizeof(TextBuffer) * *filter_size);
  // if(!tmp){
  //  ERRNO_CALLBACK("realloc failed!", strerror(errno));
  //  exit(EXIT_FAILURE);
  // }

  /// *filtered = tmp;
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

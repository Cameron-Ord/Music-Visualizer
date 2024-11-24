#include "utils.h"
#include "fontdef.h"
#include <ctype.h>
#include <errno.h>
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

void *destroy_search_text(Text *s) {
  if (s) {
    free(s->name);
    SDL_DestroyTexture(s->tex[0]);
    free(s);
  }

  return NULL;
}

int check_bounds(int max, int input) {
  if (input > max) {
    return 0;
  }

  // typically when I call this function the casted type is unsigned but still a
  // good check to have.
  if (input < 0) {
    return 0;
  }

  return 1;
}

int has_file_extension(const char *string) {
  int i = 0;
  while (string[i] != '\0') {
    if (string[i] == '.') {
      return 1;
    }
    i++;
  }

  return 0;
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

size_t do_search(char *text_input_buf, const size_t *count,
                 const TextBuffer *base, TextBuffer **filtered) {
  size_t filter_count = 0;
  for (size_t i = 0; i < *count; i++) {
    Text *text = base[i].text;
    char *name = text->name;
    // strlen returns size in bytes, so just using malloc here instead of stack
    // allocation for that reason, god knows that even if im forcing ascii, if
    // the original file contains non ascii strlen will still return byte size
    // reflecting that(I believe).
    char *name_cpy = malloc(strlen(name) + 1);
    if (!name_cpy) {
      return 0;
    }

    if (!strcpy(name_cpy, name)) {
      free(name_cpy);
      return 0;
    }

    char *input_cpy = malloc(strlen(text_input_buf) + 1);
    if (!input_cpy) {
      return 0;
    }

    if (!strcpy(input_cpy, text_input_buf)) {
      free(input_cpy);
      free(name_cpy);
      return 0;
    }

    if (strstr(sformat(name_cpy), sformat(input_cpy))) {
      (*filtered)[i].text = text;
      filter_count += 1;
    } else {
      (*filtered)[i].text = NULL;
    }

    free(name_cpy);
    free(input_cpy);
  }

  for (size_t i = 0; i < *count; i++) {
    if (!(*filtered)[i].text) {
      (*filtered)[i].text = null_replace(i, count, *filtered);
    }
  }

  return filter_count;
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

void remove_char(char **buf, size_t *pos, size_t *size) {
  int signed_index = (int)*pos;
  signed_index--;

  if (signed_index < 0) {
    signed_index = 0;
  }

  *pos = signed_index;
  if ((*pos + 1) < DEFAULT_INPUT_BUFFER_SIZE &&
      *size > DEFAULT_INPUT_BUFFER_SIZE) {
    const size_t new_size = DEFAULT_INPUT_BUFFER_SIZE + 1;
    char *tmp = realloc(*buf, new_size);
    if (!buf) {
      ERRNO_CALLBACK("realloc failed!", strerror(errno));
      return;
    }

    *buf = tmp;
    *size = new_size;
  }
  (*buf)[*pos] = '\0';
}

void append_char(const char *c, char **buf, size_t *pos, size_t *size) {
  (*buf)[*pos] = *c;
  (*pos)++;

  if ((*pos + 1) >= *size) {
    size_t new_buf_size = (*size * 2) + 1;
    char *tmp = realloc(*buf, new_buf_size);
    if (!buf) {
      ERRNO_CALLBACK("realloc failed!", strerror(errno));
      return;
    }

    *buf = tmp;
    *size = new_buf_size;
  }

  (*buf)[*pos] = '\0';
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

void do_swap(TextBuffer *search, const size_t *s_count, TextBuffer *replace,
             const size_t *count) {
  for (size_t i = 0; i < *count; i++) {
    Text *replace_text = replace[i].text;
    for (size_t j = 0; j < *s_count; j++) {
      Text **invalidated_text = &search[j].text;
      if (*invalidated_text && replace_text) {
        if (!(*invalidated_text)->name || !replace_text->name) {
          continue;
        }

        if (strcmp(replace_text->name, (*invalidated_text)->name) == 0) {
          *invalidated_text = replace_text;
        }
      }
    }
  }
}

int clamp_font_size(int size) {
  int min_size = 12;
  int max_size = 28;

  if (size > max_size) {
    size = max_size;
  }

  if (size < min_size) {
    size = min_size;
  }

  return size;
}

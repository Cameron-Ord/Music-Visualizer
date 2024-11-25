#include "utils.h"
#include "fontdef.h"
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

uint8_t alpha_min = 255 * 0.70;
uint8_t alpha_low = 255 * 0.80;
uint8_t alpha_decreased = 255 * 0.95;
uint8_t alpha_max = 255;

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

  return -1;
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
  int max_size = 16;

  if (size > max_size) {
    size = max_size;
  }

  if (size < min_size) {
    size = min_size;
  }

  return size;
}

void clamp_rgb_diff(uint8_t *mod, uint8_t base) {
  uint8_t diff = base - *mod;
  uint8_t max_diff = 45;
  if (diff > max_diff) {
    *mod = base - max_diff;
  }
}

// Phase is expected to be a float in range -1.0 to 1.0
SDL_Color determine_rgba(float phase, const SDL_Color *prim, uint8_t alpha) {
  SDL_Color rgba = {0};
  if (alpha <= alpha_min) {
    rgba.r = prim->r, rgba.g = prim->g, rgba.b = prim->b, rgba.a = alpha;
    return rgba;
  }

  float factor_max = 0.05;
  float factor_min = 0.025;

  uint8_t rl1 = prim->r + (255 - prim->r) * factor_min;
  uint8_t gl1 = prim->g + (255 - prim->g) * factor_min;
  uint8_t bl1 = prim->b + (255 - prim->b) * factor_min;

  uint8_t rd1 = prim->r - prim->r * factor_min;
  uint8_t gd1 = prim->g - prim->g * factor_min;
  uint8_t bd1 = prim->b - prim->b * factor_min;

  uint8_t rl2 = prim->r + (255 - prim->r) * factor_max;
  uint8_t gl2 = prim->g + (255 - prim->g) * factor_max;
  uint8_t bl2 = prim->b + (255 - prim->b) * factor_max;

  uint8_t rd2 = prim->r - prim->r * factor_max;
  uint8_t gd2 = prim->g - prim->g * factor_max;
  uint8_t bd2 = prim->b - prim->b * factor_max;

  if (phase <= -0.5) {
    rgba.r = rd2, rgba.g = gd2, rgba.b = bd2, rgba.a = alpha;
  } else if (phase <= 0.0) {
    rgba.r = rd1, rgba.g = gd1, rgba.b = bd1, rgba.a = alpha;
  } else if (phase <= 0.5) {
    rgba.r = rl2, rgba.g = gl2, rgba.b = bl2, rgba.a = alpha;
  } else if (phase <= 1.0) {
    rgba.r = rl1, rgba.g = gl1, rgba.b = bl1, rgba.a = alpha;
  }

  return rgba;
}

// Amplitude values are expected to range from 0.0 to 1.0
uint8_t determine_alpha(float amplitude) {
  if (amplitude < 0.0) {
    return alpha_max;
  }

  if (amplitude <= 0.25) {
    return alpha_min;
  } else if (amplitude <= 0.5) {
    return alpha_low;
  } else if (amplitude <= 0.75) {
    return alpha_decreased;
  } else {
    return alpha_max;
  }

  return alpha_max;
}

size_t clamp_size_t(size_t input, const size_t max) {
  if (input > max - 1) {
    input = max - 1;
  }

  return input;
}

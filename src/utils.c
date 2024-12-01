#include "utils.h"
#include "fontdef.h"
#include "main.h"
#include "table.h"

#include <ctype.h>
#include <errno.h>
#include <sndfile.h>
#include <stdlib.h>
#include <string.h>

uint8_t alpha_max = 255;
uint8_t alpha_min = 255 * 0.25;

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

const char* ext_literals[]={".ogg", ".wav", ".flac", ".aiff", "aif", ".wv", ".opus", ".caf", ".mp3"};
const size_t size = sizeof(ext_literals) / sizeof(ext_literals[0]);

int check_file_str(const char* str){
  for(size_t i = 0; i < size; i++){
    if(strstr(str, ext_literals[i])){
      return  1;
    }
  }

  return 0;
}

int get_char_limit(int width) {
  const int sub_amount = width * 0.10;
  if (width < 100) {
    return 1;
  }
  return MIN(175, MAX(8, (width - sub_amount) / 10));
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

static uint8_t lighten(const uint8_t col, const float f) {
  return col + (255 - col) * f;
}
static uint8_t darken(const uint8_t col, const float f) {
  return col - col * f;
}
// subtlety is key
const float coeff_max = 0.05;
const float coeff_norm = 0.025;
const float coeff_min = 0.01;
// min, max, coefficient
const float ranges[8][3] = {
    {-1.0f, -0.75f, coeff_max}, {-0.75f, -0.5f, coeff_norm},
    {-0.5f, -0.25f, coeff_min}, {-0.25f, 0.0f, 0.0},
    {0.0f, 0.25f, 0.0},         {0.25f, 0.5f, coeff_min},
    {0.5f, 0.75f, coeff_norm},  {0.75f, 1.0f, coeff_max}};

static uint8_t (*const fnptrs[])(const uint8_t, const float) = {
    darken, darken, darken, 0, 0, lighten, lighten, lighten};

// Phase is expected to be a float in range -1.0 to 1.0
SDL_Color determine_rgba(float phase, const SDL_Color *prim, uint8_t alpha) {
  SDL_Color rgba = {0};

  uint8_t r = prim->r;
  uint8_t g = prim->g;
  uint8_t b = prim->b;

  uint8_t *rgb_vals[] = {&r, &g, &b};
  size_t length = sizeof(rgb_vals) / sizeof(rgb_vals[0]);

  for (size_t j = 0; j < 8; j++) {
    const float min = ranges[j][0];
    const float max = ranges[j][1];

    if (phase > max || phase < min) {
      continue;
    }

    if (phase >= min && phase <= max) {
      for (size_t i = 0; i < length; i++) {
        const float coeff = ranges[j][2];
        if (fnptrs[j]) {
          *rgb_vals[i] = fnptrs[j](*rgb_vals[i], coeff);
        }
      }
    }
  }

  rgba.r = r, rgba.g = g, rgba.b = b, rgba.a = alpha;
  return rgba;
}

static uint8_t clamp_alpha(uint8_t a) {
  if (a < alpha_min) {
    a = alpha_min;
  }

  if (a > alpha_max * 0.9) {
    a = alpha_max;
  }

  return a;
}

// Amplitude values are expected to range from 0.0 to 1.0
uint8_t determine_alpha(float amplitude) {
  return clamp_alpha(alpha_max * amplitude);
}

int valid_ptr(Paths *p, TextBuffer *t) {
  if ((p && t) && (p->is_valid && t->is_valid)) {
    return 1;
  }

  return 0;
}

int min_titles(TextBuffer *t) {
  int h = win.height;

  int spacing = 4;
  int accumulator = spacing;
  int is_greater = 0;

  size_t j = 0;
  for (j = 0; j < t->size; j++) {
    if (t[j].text) {
      int rect_h = t[j].text->rect.h;
      accumulator += rect_h + spacing;
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
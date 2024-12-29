#include "font.h"
#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

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

static void zero_data(IdxInfo *i) {
  i->is_valid = 0;
  i->size = 0;
  i->start = 0;
  i->cursor = 0;
  i->max_text_height = 0;
}

static void zero_buffer(TextBuffer *t) { t->text = NULL; }

static int validate_paths(const Paths *p) { return p && p->size > 0; }

static TextBuffer *text_buf_allocate(const size_t size) {
  TextBuffer *tbuf = malloc(sizeof(TextBuffer) * size);
  if (!tbuf) {
    ERRNO_CALLBACK("Malloc failed!", strerror(errno));
    return NULL;
  }

  for (size_t i = 0; i < size; i++) {
    zero_buffer(&tbuf[i]);
    zero_data(&tbuf[i].info);
  }

  tbuf->info.size = size;
  return tbuf;
}

TextBuffer *create_fonts(const Paths *pbuf, SDL_Renderer *r, Font *f,
                         const int w, const SDL_Color *c_text,
                         const SDL_Color *c_sec) {
  if (!validate_paths(pbuf)) {
    return NULL;
  }

  TextBuffer *tbuf = text_buf_allocate(pbuf->size);
  if (!tbuf) {
    return NULL;
  }

  for (size_t i = 0; i < pbuf->size; i++) {
    tbuf[i].text = NULL;
    Text *text = malloc(sizeof(Text));
    if (!text) {
      ERRNO_CALLBACK("malloc() failed!", strerror(errno));
      return tbuf;
    }

    tbuf[i].text = text;

    text->is_valid = 0;
    text->surf[0] = NULL;
    text->tex[0] = NULL;
    text->surf[1] = NULL;
    text->tex[1] = NULL;

    if (!pbuf[i].name) {
      fprintf(stderr, "Path name is NULL!\n");
      return tbuf;
    }

    // strdup uses malloc() so this needs to be freed when not used.
    text->name = strdup(pbuf[i].name);
    if (!text->name) {
      ERRNO_CALLBACK("strdup() failed!", strerror(errno));
      return tbuf;
    }

    text->id = i;

    const size_t max_chars = get_char_limit(w, f->size);
    char name_buffer[max_chars + 1];
    name_buffer[max_chars] = '\0';

    size_t j = 0;
    while (j < max_chars && text->name[j] != '\0') {
      name_buffer[j] = text->name[j];
      j++;
    }

    if (j == max_chars) {
      name_buffer[j - 1] = '~';
    } else {
      name_buffer[j] = '\0';
    }

    text->surf[0] = TTF_RenderText_Solid(f->font, name_buffer, *c_text);
    if (!text->surf[0]) {
      return tbuf;
    }

    text->tex[0] = SDL_CreateTextureFromSurface(r, text->surf[0]);
    if (!text->tex[0]) {
      return tbuf;
    }

    text->width = text->surf[0]->w;
    text->height = text->surf[0]->h;
    SDL_FreeSurface(text->surf[0]);

    text->surf[1] = TTF_RenderText_Solid(f->font, name_buffer, *c_sec);
    if (!text->surf[1]) {
      return tbuf;
    }

    text->tex[1] = SDL_CreateTextureFromSurface(r, text->surf[1]);
    if (!text->tex[1]) {
      return tbuf;
    }

    SDL_FreeSurface(text->surf[1]);

    text->surf[0] = NULL;
    text->surf[1] = NULL;
    text->is_valid = 1;

    if (text->height > tbuf->info.max_text_height) {
      tbuf->info.max_text_height = text->height;
    }
  }

  tbuf->info.is_valid = 1;
  return tbuf;
}

#include "fontdef.h"
#include "main.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

TextBuffer *create_fonts(const Paths *paths_buf, const size_t *count) {

  if (!paths_buf || !count) {
    fprintf(stderr,
            "Paths buf or counter was NULL! -> counter : %p - paths : %p\n",
            (void *)count, (void *)paths_buf);
    return NULL;
  }

  TextBuffer *text_buffer = (TextBuffer *)malloc(sizeof(TextBuffer) * (*count));
  if (!text_buffer) {
    fprintf(stderr, "Malloc failed! -> %s\n", strerror(errno));
    return NULL;
  }

  memset(text_buffer, 0, sizeof(TextBuffer) * (*count));

  for (size_t i = 0; i < *count; i++) {
    Text *text = (Text *)malloc(sizeof(Text));
    if (!text) {
      fprintf(stderr, "Malloc failed! -> %s\n", strerror(errno));
      free(text_buffer);
      return NULL;
    }

    memset(text, 0, sizeof(Text));
    text->is_valid = false;
    text->surf = NULL;
    text->tex = NULL;

    if (!paths_buf[i].name) {
      free(text);
      text = NULL;
      fprintf(stderr, "Path name is NULL! -> continue\n");
      continue;
    }

    // Man just be careful with this, if you seg fault when creating fonts check
    // here first.
    const char *name = paths_buf[i].name;

    text->name = name;
    text->id = i;

    const size_t tmp_size = paths_buf[i].name_length + 64;
    char name_buffer[tmp_size];

    strncpy(name_buffer, name, paths_buf[i].name_length);
    const size_t max_chars = get_char_limit(win.width);

    if (paths_buf[i].name_length > max_chars) {
      size_t locn = max_chars - 1;
      if (name_buffer[locn] == 32) {
        name_buffer[locn] = 46;
      }

      locn = max_chars;
      name_buffer[locn++] = 46;
      name_buffer[locn++] = 46;
      name_buffer[locn++] = 46;
      name_buffer[locn++] = '\0';
    } else {
      name_buffer[paths_buf[i].name_length] = '\0';
    }

    assert(font.font != NULL);
    text->surf = TTF_RenderText_Blended(font.font, name_buffer, vis.text);
    if (!text->surf) {
      text->surf = NULL;
      free(text);
      free(text_buffer);
      return NULL;
    }
    assert(text->surf != NULL);

    text->tex = SDL_CreateTextureFromSurface(rend.r, text->surf);
    if (!text->tex) {
      text->tex = NULL;
      SDL_FreeSurface(text->surf);
      free(text);
      free(text_buffer);
      return NULL;
    }

    assert(text->tex != NULL);
    text->width = text->surf->w;
    text->height = text->surf->h;
    SDL_Rect text_rect = {.x = 0, .y = 0, text->width, text->height};
    text->rect = text_rect;

    SDL_FreeSurface(text->surf);
    text->surf = NULL;

    text->is_valid = true;

    // Assign the pointer created in the current iteration
    text_buffer[i].text = text;
  }

  return text_buffer;
}

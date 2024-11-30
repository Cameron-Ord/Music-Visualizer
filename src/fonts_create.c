#include "fontdef.h"
#include "main.h"
#include "utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

TextBuffer *create_fonts(const Paths *pbuf) {
  if (!pbuf) {
    return NULL;
  }

  TTF_SetFontSize(font.font, clamp_font_size(0.016 * win.width));
  if (pbuf->size <= 0) {
    return NULL;
  }

  TextBuffer *tbuf = malloc(sizeof(TextBuffer) * (pbuf->size));
  if (!tbuf) {
    fprintf(stderr, "Malloc failed! -> %s\n", strerror(errno));
    return NULL;
  }

  tbuf->max = 0;
  tbuf->is_valid = 0;
  tbuf->size = 0;
  tbuf->cursor = 0;
  tbuf->start = 0;
  tbuf->listed = 0;

  for (size_t i = 0; i < pbuf->size; i++) {
    tbuf[i].text = NULL;
    Text *text = malloc(sizeof(Text));
    if (!text) {
      fprintf(stderr, "Malloc failed! -> %s\n", strerror(errno));
      return tbuf;
    }

    text->is_valid = 0;
    text->surf[0] = NULL;
    text->tex[0] = NULL;
    text->surf[1] = NULL;
    text->tex[1] = NULL;

    if (!pbuf[i].name) {
      ERRNO_CALLBACK("Path name is NULL!", "");
      return tbuf;
    }

    text->name = malloc(pbuf[i].name_length + 1);
    if (!text->name) {
      ERRNO_CALLBACK("Failed to allocate pointer!", strerror(errno));
      return tbuf;
    }

    if (!strncpy(text->name, pbuf[i].name, pbuf[i].name_length)) {
      ERRNO_CALLBACK("strncpy() failed!", strerror(errno));
      return tbuf;
    }

    text->name[pbuf[i].name_length] = '\0';
    text->id = i;

    const size_t max_chars = get_char_limit(win.width);
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

    text->surf[0] = TTF_RenderText_Blended(font.font, name_buffer, vis.text);
    if (!text->surf[0]) {
      return tbuf;
    }

    text->tex[0] = SDL_CreateTextureFromSurface(rend.r, text->surf[0]);
    if (!text->tex[0]) {
      return tbuf;
    }

    text->width = text->surf[0]->w;
    text->height = text->surf[0]->h;
    SDL_Rect text_rect = {.x = 0, .y = 0, text->width, text->height};
    text->rect = text_rect;

    SDL_FreeSurface(text->surf[0]);

    text->surf[1] =
        TTF_RenderText_Blended(font.font, name_buffer, vis.secondary);
    if (!text->surf[1]) {
      return tbuf;
    }

    text->tex[1] = SDL_CreateTextureFromSurface(rend.r, text->surf[1]);
    if (!text->tex[1]) {
      return tbuf;
    }

    SDL_FreeSurface(text->surf[1]);

    text->surf[0] = NULL;
    text->surf[1] = NULL;
    text->is_valid = 1;

    // Assign the pointer created in the current iteration
    tbuf[i].text = text;
    tbuf->size++;
  }

  tbuf->is_valid = 1;
  return tbuf;
}

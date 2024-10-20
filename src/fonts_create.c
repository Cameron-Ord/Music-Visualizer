#include "fontdef.h"
#include "main.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

TextBuffer *create_fonts(const Paths *paths_buf, const size_t *count) {

  if (!paths_buf || !count) {
    return NULL;
  }

  TextBuffer *text_buffer = (TextBuffer *)malloc(sizeof(TextBuffer) * (*count));
  if (!text_buffer) {
    fprintf(stderr, "Malloc failed! -> %s\n", strerror(errno));
    return NULL;
  }

  memset(text_buffer, 0, sizeof(TextBuffer) * (*count));

  bool loop_broken = false;
  for (size_t i = 0; i < *count; i++) {
    Text *text = (Text *)malloc(sizeof(Text));
    if (!text) {
      fprintf(stderr, "Malloc failed! -> %s\n", strerror(errno));
      loop_broken = true;
      break;
    }

    memset(text, 0, sizeof(Text));
    text->is_valid = false;
    text->surf = NULL;
    text->tex = NULL;

    if (!paths_buf[i].name) {
      fprintf(stderr, "Path name is NULL!\n");
      loop_broken = true;
      break;
    }

    const char *name = paths_buf[i].name;

    text->name = malloc(paths_buf[i].name_length + 64);
    if (!text->name) {
      fprintf(stderr, "Failed to allocate pointer! -> %s\n", strerror(errno));
      loop_broken = true;
      break;
    }

    strncpy(text->name, name, paths_buf[i].name_length);
    text->name[paths_buf[i].name_length] = '\0';
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

    text->surf = TTF_RenderText_Blended(font.font, name_buffer, vis.text);
    if (!text->surf) {
      text->surf = NULL;
      loop_broken = true;
      break;
    }

    text->tex = SDL_CreateTextureFromSurface(rend.r, text->surf);
    if (!text->tex) {
      text->tex = NULL;
      loop_broken = true;
      break;
    }

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

  if (loop_broken) {
    text_buffer = free_text_buffer(text_buffer, count);
    text_buffer = NULL;
  }

  return text_buffer;
}

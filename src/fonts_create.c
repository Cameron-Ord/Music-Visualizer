#include "fontdef.h"
#include "main.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

Text *create_search_text(const char *input_text_buffer,
                         const size_t *text_buf_len,
                         const size_t *text_buf_position) {
  TTF_SetFontSize(font.font, clamp_font_size(0.016 * win.width));

  Text *text = malloc(sizeof(Text));
  if (!text) {
    ERRNO_CALLBACK("malloc failed!", strerror(errno));
    return NULL;
  }

  if (!input_text_buffer) {
    ERRNO_CALLBACK("input buffer is NULL! - Last errno :", strerror(errno));
    free(text);
    return NULL;
  }

  text->name = malloc(*text_buf_len + 1);
  if (!text->name) {
    ERRNO_CALLBACK("malloc failed!", strerror(errno));
    free(text);
    return NULL;
  }

  if (!strcpy(text->name, input_text_buffer)) {
    ERRNO_CALLBACK("snprintf failed!", strerror(errno));
    free(text->name);
    free(text);
    return NULL;
  }

  text->name[*text_buf_position] = '\0';

  text->id = 1;
  text->surf[0] = TTF_RenderText_Blended(font.font, text->name, vis.text);
  if (!text->surf[0]) {
    text->surf[0] = NULL;
    return NULL;
  }

  text->tex[0] = SDL_CreateTextureFromSurface(rend.r, text->surf[0]);
  if (!text->tex[0]) {
    text->tex[0] = NULL;
    return NULL;
  }

  text->width = text->surf[0]->w;
  text->height = text->surf[0]->h;
  SDL_Rect text_rect = {.x = 0, .y = 0, text->width, text->height};
  text->rect = text_rect;

  SDL_FreeSurface(text->surf[0]);
  text->surf[0] = NULL;
  text->is_valid = true;

  return text;
}

TextBuffer *create_fonts(const Paths *paths_buf, const size_t *count) {
  if (!paths_buf || !count) {
    return NULL;
  }

  TTF_SetFontSize(font.font, clamp_font_size(0.016 * win.width));

  TextBuffer *text_buffer = (TextBuffer *)malloc(sizeof(TextBuffer) * (*count));
  if (!text_buffer) {
    fprintf(stderr, "Malloc failed! -> %s\n", strerror(errno));
    return NULL;
  }

  memset(text_buffer, 0, sizeof(TextBuffer) * (*count));

  bool loop_broken = false;
  for (size_t i = 0; i < *count; i++) {
    Text *text = malloc(sizeof(Text));
    if (!text) {
      fprintf(stderr, "Malloc failed! -> %s\n", strerror(errno));
      loop_broken = true;
      break;
    }

    memset(text, 0, sizeof(Text));
    text->is_valid = false;
    text->surf[0] = NULL;
    text->tex[0] = NULL;
    text->surf[1] = NULL;
    text->tex[1] = NULL;

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

    text->surf[0] = TTF_RenderText_Blended(font.font, name_buffer, vis.text);
    if (!text->surf[0]) {
      text->surf[0] = NULL;
      loop_broken = true;
      break;
    }

    text->tex[0] = SDL_CreateTextureFromSurface(rend.r, text->surf[0]);
    if (!text->tex[0]) {
      text->tex[0] = NULL;
      loop_broken = true;
      break;
    }

    text->surf[1] =
        TTF_RenderText_Blended(font.font, name_buffer, vis.secondary);
    if (!text->surf[1]) {
      text->surf[1] = NULL;
      loop_broken = true;
      break;
    }

    text->tex[1] = SDL_CreateTextureFromSurface(rend.r, text->surf[1]);
    if (!text->tex[1]) {
      text->tex[1] = NULL;
      loop_broken = true;
      break;
    }

    text->width = text->surf[0]->w;
    text->height = text->surf[0]->h;
    SDL_Rect text_rect = {.x = 0, .y = 0, text->width, text->height};
    text->rect = text_rect;

    SDL_FreeSurface(text->surf[0]);
    SDL_FreeSurface(text->surf[1]);

    text->surf[0] = NULL;
    text->surf[1] = NULL;
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

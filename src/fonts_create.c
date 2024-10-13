#include "main.h"
#include <stdio.h>
#include <string.h>


TextBuffer *create_fonts(Paths *paths_buf, const size_t count,
                                   size_t *list_size,
                                   const size_t *buffer_size) {

  TextBuffer *list_buf = malloc(sizeof(TextBuffer) * (*list_size));
  if (!list_buf) {
    fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
    return NULL;
  }

  Text *tmp_buf = malloc(sizeof(Text) * (*buffer_size));
  if (!tmp_buf) {
    fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
    return NULL;
  }

  size_t buffer_i = 0;
  size_t list_i = 0;

  for (size_t i = 0; i < count; i++) {
    if (buffer_i > *buffer_size - 1) {

      Text *text_buffer = malloc(sizeof(Text) * (*buffer_size));
      if (!text_buffer) {
        fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
        return NULL;
      }

      memcpy(text_buffer, tmp_buf, sizeof(Text) * (*buffer_size));
      memset(tmp_buf, 0, sizeof(Text) * (*buffer_size));

      list_buf[list_i].size = *buffer_size;
      list_buf[list_i].buf = text_buffer;

      (*list_size)++;
      list_buf = realloc(list_buf, sizeof(TextBuffer) * (*list_size));

      list_i++;
      buffer_i = 0;
    }

    tmp_buf[buffer_i].id = buffer_i;
    tmp_buf[buffer_i].is_valid = false;

    size_t char_len = strlen(paths_buf[i].name);
    tmp_buf[buffer_i].name = malloc(sizeof(char) * (char_len + 1));
    if (!tmp_buf[buffer_i].name) {
      fprintf(stderr, "Could not allocate pointer! ->%s\n", strerror(errno));
      free(tmp_buf);
      free(list_buf);
      return NULL;
    }

    if (!strcpy(tmp_buf[buffer_i].name, paths_buf[i].name)) {
      fprintf(stderr, "Failed to copy string! -> %s\n", strerror(errno));
      free(tmp_buf[buffer_i].name);
      free(tmp_buf);
      free(list_buf);
      return NULL;
    }

    SDL_Surface *surf =
        TTF_RenderText_Blended(font.font, tmp_buf[buffer_i].name, vis.text);
    if (!surf) {
      fprintf(stderr, "Could not create font surface! -> %s\n", SDL_GetError());
      free(tmp_buf[buffer_i].name);
      free(tmp_buf);
      free(list_buf);
      return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(rend.r, surf);
    if (!texture) {
      fprintf(stderr, "Could not create font texture! -> %s\n", SDL_GetError());
      free(tmp_buf[buffer_i].name);
      free(tmp_buf);
      free(list_buf);
      return NULL;
    }

    SDL_Rect tmp = {0, 0, surf->w, surf->h};

    tmp_buf[buffer_i].rect = tmp;
    tmp_buf[buffer_i].surf = surf;
    tmp_buf[buffer_i].tex = texture;
    tmp_buf[buffer_i].width = surf->w;
    tmp_buf[buffer_i].height = surf->h;
    tmp_buf[buffer_i].is_valid = true;

    buffer_i++;
  }

  if (buffer_i > 0) {
    Text *txt_sub_buf = malloc(sizeof(Text) * buffer_i);
    if (!txt_sub_buf) {
      fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
      return NULL;
    }

    memcpy(txt_sub_buf, tmp_buf, sizeof(Text) * buffer_i);
    memset(tmp_buf, 0, sizeof(Text) * buffer_i);

    list_buf[list_i].buf = txt_sub_buf;
    list_buf[list_i].size = buffer_i;
  }

  if (tmp_buf) {
    free(tmp_buf);
  }

  return list_buf;
}

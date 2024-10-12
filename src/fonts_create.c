#include "main.h"
#include <string.h>

TextBuffer **create_directory_fonts(char **dir_buf, const size_t dir_count,
                                    size_t *text_buf_size,
                                    const size_t *sub_buf_size) {

  TextBuffer **text_buf = malloc(sizeof(Text *) * *text_buf_size);
  if (!text_buf) {
    fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
    return NULL;
  }

  memset(text_buf, 0, sizeof(TextBuffer **) * *text_buf_size);

  Text **tmp_buf = malloc(sizeof(Text *) * (*sub_buf_size));

  size_t sub_increment = 0;
  size_t sub_buffer_level = 0;

  size_t remaining = 0;
  size_t unfilled_flag = 0;

  for (size_t i = 0; i < dir_count; i++) {
    if (sub_increment > *sub_buf_size - 1) {

      Text **txt_sub_buf = malloc(sizeof(Text *) * (*sub_buf_size));
      if (!txt_sub_buf) {
        fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
        return NULL;
      }

      memcpy(txt_sub_buf, tmp_buf, sizeof(Text *) * (*sub_buf_size));
      memset(tmp_buf, 0, sizeof(Text *) * (*sub_buf_size));

      text_buf[sub_buffer_level]->size = *sub_buf_size;
      text_buf[sub_buffer_level]->buf = txt_sub_buf;

      (*text_buf_size)++;
      text_buf = realloc(text_buf, sizeof(TextBuffer *) * (*text_buf_size));

      sub_buffer_level++;
      sub_increment = 0;

      remaining = dir_count - i;
      if (remaining < *sub_buf_size) {
        unfilled_flag = 1;
      }
    }

    Text *text = malloc(sizeof(Text));
    if (!text) {
      fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
      return text_buf;
    }

    text->id = sub_increment;
    text->is_valid = false;

    size_t char_len = strlen(dir_buf[i]);
    text->name = malloc(sizeof(char) * (char_len + 1));
    if (!text->name) {
      fprintf(stderr, "Could not allocate pointer! ->%s\n", strerror(errno));
      free(text);
      return text_buf;
    }

    if (!strcpy(text->name, dir_buf[i])) {
      fprintf(stderr, "Failed to copy string! -> %s\n", strerror(errno));
      free(text->name);
      free(text);
      return text_buf;
    }

    SDL_Surface *surf = TTF_RenderText_Blended(font.font, text->name, vis.text);
    if (!surf) {
      fprintf(stderr, "Could not create font surface! -> %s\n", SDL_GetError());
      free(text->name);
      free(text);
      return text_buf;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(rend.r, surf);
    if (!texture) {
      fprintf(stderr, "Could not create font texture! -> %s\n", SDL_GetError());
      free(text->name);
      free(text);
      return text_buf;
    }

    SDL_Rect tmp = {0, 0, surf->w, surf->h};

    text->rect = tmp;
    text->surf = surf;
    text->tex = texture;
    text->width = surf->w;
    text->height = surf->h;
    text->is_valid = true;

    tmp_buf[sub_increment] = text;

    sub_increment++;
  }

  if (unfilled_flag) {
    Text **txt_sub_buf = malloc(sizeof(Text *) * remaining);
    if (!txt_sub_buf) {
      fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
      return NULL;
    }

    memcpy(txt_sub_buf, tmp_buf, sizeof(Text *) * remaining);
    memset(tmp_buf, 0, sizeof(Text *) * remaining);

    text_buf[sub_buffer_level]->buf = txt_sub_buf;
    text_buf[sub_buffer_level]->size = remaining;
  }

  if (tmp_buf) {
    free(tmp_buf);
  }

  return text_buf;
}

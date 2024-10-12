#include "main.h"
#include <stdio.h>
#include <string.h>

TextBuffer *create_directory_fonts(char **dir_buf, const size_t dir_count,
                                    size_t *list_size,
                                    const size_t *buffer_size) {

  TextBuffer *list_buf = malloc(sizeof(TextBuffer ) * (*list_size));
  if (!list_buf) {
    fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
    return NULL;
  }

  Text **tmp_buf = malloc(sizeof(Text *) * (*buffer_size));
  if(!tmp_buf){
    fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
    return NULL;
  }

  size_t sub_increment = 0;
  size_t sub_buffer_level = 0;

  for (size_t i = 0; i < dir_count; i++) {
    if (sub_increment > *buffer_size - 1) {

      Text **text_buffer = malloc(sizeof(Text *) * (*buffer_size));
      if (!text_buffer) {
        fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
        return NULL;
      }

      memcpy(text_buffer, tmp_buf, sizeof(Text *) * (*buffer_size));
      memset(tmp_buf, 0, sizeof(Text *) * (*buffer_size));

      list_buf[sub_buffer_level].size = *buffer_size;
      list_buf[sub_buffer_level].buf = text_buffer;

      (*list_size)++;
      list_buf = realloc(list_buf, sizeof(TextBuffer *) * (*list_size));

      sub_buffer_level++;
      sub_increment = 0;
    }

    Text *text = malloc(sizeof(Text));
    if (!text) {
      fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
      return list_buf;
    }

    text->id = sub_increment;
    text->is_valid = false;

    size_t char_len = strlen(dir_buf[i]);
    text->name = malloc(sizeof(char) * (char_len + 1));
    if (!text->name) {
      fprintf(stderr, "Could not allocate pointer! ->%s\n", strerror(errno));
      free(text);
      return list_buf;
    }

    if (!strcpy(text->name, dir_buf[i])) {
      fprintf(stderr, "Failed to copy string! -> %s\n", strerror(errno));
      free(text->name);
      free(text);
      return list_buf;
    }

    SDL_Surface *surf = TTF_RenderText_Blended(font.font, text->name, vis.text);
    if (!surf) {
      fprintf(stderr, "Could not create font surface! -> %s\n", SDL_GetError());
      free(text->name);
      free(text);
      return list_buf;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(rend.r, surf);
    if (!texture) {
      fprintf(stderr, "Could not create font texture! -> %s\n", SDL_GetError());
      free(text->name);
      free(text);
      return list_buf;
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

  if (sub_increment > 0) {
    Text **txt_sub_buf = malloc(sizeof(Text *) * sub_increment);
    if (!txt_sub_buf) {
      fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
      return NULL;
    }

    memcpy(txt_sub_buf, tmp_buf, sizeof(Text *) * sub_increment);
    memset(tmp_buf, 0, sizeof(Text *) * sub_increment);

  for(size_t i = 0; i < sub_increment; i++){
    printf("%s\n", txt_sub_buf[i]->name);
  }

    list_buf[sub_buffer_level].buf = txt_sub_buf;
    list_buf[sub_buffer_level].size = sub_increment;
  }

  if (tmp_buf) {
    free(tmp_buf);
  }

  return list_buf;
}

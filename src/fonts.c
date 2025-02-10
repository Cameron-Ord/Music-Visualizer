#include "../inc/font.h"
#include "../inc/main.h"
#include "../inc/renderer.h"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define ATLAS_SIZE 128

Font font = {16, NULL};
Character atlas[ATLAS_SIZE];

static int fill_text_atlas(void) {
  for (int i = 0; i < ATLAS_SIZE; i++) {
    const char *c = (char[]){i, '\0'};
    SDL_Surface *s = TTF_RenderText_Blended(font.font, c, *_text());
    if (!s) {
      sdl_err(SDL_GetError());
      return 0;
    }

    SDL_Texture *t = SDL_CreateTextureFromSurface(get_renderer()->r, s);
    if (!t) {
      SDL_FreeSurface(s);
      sdl_err(SDL_GetError());
      return 0;
    }

    atlas[i].value = i;
    atlas[i].texture = t;
    atlas[i].w = s->w;
    atlas[i].h = s->h;

    SDL_FreeSurface(s);
  }

  return 1;
}

int _fill_text_atlas(void) { return fill_text_atlas(); }

static int ascii_bounds(unsigned int c) {
  if (c > 127) {
    return 0;
  }

  return 1;
}

const Character *atlas_lookup_char(unsigned int c) {
  if (!ascii_bounds(c)) {
    return NULL;
  }

  return &atlas[c];
}

static int open_ttf_file(const char *path, const char *env) {
  const int size = strlen(path) + strlen(env);
  char path_buffer[size + 1];

  if (!strcpy(path_buffer, env)) {
    errno_string("strcpy()\n", strerror(errno));
    return 0;
  }

  if (!strcat(path_buffer, path)) {
    errno_string("strcat()\n", strerror(errno));
    return 0;
  }

  path_buffer[size] = '\0';

  font.font = TTF_OpenFont(path_buffer, font.size);
  if (!font.font) {
    sdl_err(SDL_GetError());
    return 0;
  }

  return 1;
}

int _open_ttf_file(const char *path, const char *env) {
  return open_ttf_file(path, env);
}

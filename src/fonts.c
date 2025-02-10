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
  for (int i = 32; i < ATLAS_SIZE; i++) {
    const char *c = (char[]){i, '\0'};
    SDL_Surface *surf_p = TTF_RenderText_Solid(font.font, c, *_text());
    if (!surf_p) {
      sdl_err(SDL_GetError());
      return 0;
    }

    SDL_Surface *surf_s = TTF_RenderText_Solid(font.font, c, *_sec());
    if (!surf_s) {
      SDL_FreeSurface(surf_p);
      sdl_err(SDL_GetError());
      return 0;
    }

    SDL_Texture *primary =
        SDL_CreateTextureFromSurface(get_renderer()->r, surf_p);
    if (!primary) {
      SDL_FreeSurface(surf_p);
      SDL_FreeSurface(surf_s);
      sdl_err(SDL_GetError());
      return 0;
    }

    SDL_Texture *secondary =
        SDL_CreateTextureFromSurface(get_renderer()->r, surf_s);
    if (!primary) {
      SDL_FreeSurface(surf_p);
      SDL_FreeSurface(surf_s);
      SDL_DestroyTexture(primary);
      sdl_err(SDL_GetError());
      return 0;
    }

    atlas[i].value = i;
    atlas[i].texture[0] = primary;
    atlas[i].texture[1] = secondary;

    atlas[i].w[1] = surf_s->w;
    atlas[i].h[1] = surf_s->h;

    atlas[i].w[0] = surf_p->w;
    atlas[i].h[0] = surf_p->h;

    SDL_FreeSurface(surf_s);
    SDL_FreeSurface(surf_p);
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

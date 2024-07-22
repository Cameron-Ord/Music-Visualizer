#ifndef THEMES_H
#define THEMES_H
#include <SDL2/SDL_render.h>

struct Theme {
  SDL_Color prim;
  SDL_Color secondary;
  SDL_Color tertiary;
  SDL_Color text;
  SDL_Color text_bg;
  char*     name;
};

#endif

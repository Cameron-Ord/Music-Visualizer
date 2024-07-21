#ifndef THEMES_H
#define THEMES_H
#include <SDL2/SDL_render.h>

struct Theme {
  SDL_Color prim;
  SDL_Color secondary;
  SDL_Color tertiary;
  char*     name;
};

#endif

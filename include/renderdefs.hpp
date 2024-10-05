#ifndef RENDERDEFS_HPP
#define RENDERDEFS_HPP

#include <SDL2/SDL_render.h>

struct Coordinates {
  int x;
  int y;
  int height;
  SDL_Rect copy_rect;
};

#endif
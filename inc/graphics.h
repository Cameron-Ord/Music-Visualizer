#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>

struct SettingsGear {
  SDL_Surface* surf;
  SDL_Texture* tex;
  SDL_Rect     rect;
  SDL_Rect     vp;
  int          coord_y;
  int          coord_x;
};

void         convert_pixel_colours(SDL_Surface** surf_ptr);
SDL_Surface* load_image(char* path);
void         set_rect(SDL_Rect* rect_ptr, SDL_Surface* surf, int x, int y);
SDL_Texture* create_image_texture(SDL_Renderer* r, SDL_Surface* surf_ptr);

#endif

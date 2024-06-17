#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>

struct SettingsGear {
  SDL_Surface* surf;
  SDL_Texture* tex;
  SDL_Rect     rect;
};

struct PlayIcon {
  SDL_Surface* surf;
  SDL_Texture* tex;
  SDL_Rect     rect;
};

struct PauseIcon {
  SDL_Surface* surf;
  SDL_Texture* tex;
  SDL_Rect     rect;
};

struct StopIcon {
  SDL_Surface* surf;
  SDL_Texture* tex;
  SDL_Rect     rect;
};

struct SeekIcon {
  SDL_Surface* surf;
  SDL_Texture* tex;
  SDL_Rect     rect_main[2];
};

void         convert_pixel_colours(SDL_Surface** surf_ptr, SDL_Color from_colour, SDL_Color to_colour);
SDL_Surface* load_image(char* path);
void         set_rect(SDL_Rect* rect_ptr, int x, int y, int size_w, int size_h);
SDL_Texture* create_image_texture(SDL_Renderer* r, SDL_Surface* surf_ptr);

#endif

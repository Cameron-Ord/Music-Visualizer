#include "../inc/graphics.h"
#include "../inc/music_visualizer.h"

SDL_Surface*
load_image(char* path) {
  SDL_Surface* surf_ptr = IMG_Load(path);
  if (!surf_ptr) {
    return NULL;
  }
  return surf_ptr;
}

void
set_rect(SDL_Rect* rect_ptr, SDL_Surface* surf) {
  SDL_Rect tmp = { 10, 10, surf->w * 2, surf->h * 2 };
  *rect_ptr    = tmp;
}

SDL_Texture*
create_image_texture(SDL_Renderer* r, SDL_Surface* surf_ptr) {
  SDL_Texture* tex_ptr = SDL_CreateTextureFromSurface(r, surf_ptr);
  if (!tex_ptr) {
    surf_ptr = destroy_surface(surf_ptr);
    return NULL;
  }

  surf_ptr = destroy_surface(surf_ptr);
  return tex_ptr;
}

#include "../inc/graphics.h"
#include "../inc/render.h"

#include <stdio.h>

SDL_Surface*
load_image(char* path) {
  SDL_Surface* surf_ptr = IMG_Load(path);
  if (!surf_ptr) {
    return NULL;
  }
  return surf_ptr;
}

void
convert_pixel_colours(SDL_Surface** dptr) {
  if (*dptr == NULL) {
    return;
  }

  int bpp   = (*dptr)->format->BytesPerPixel;
  u8* pixel = (u8*)(*dptr)->pixels;
  /*This just converts white rgb profiles to the purple I'm using. Later on I intend to make this
   * customizeable so you're not forced to use the purple*/
  for (int y = 0; y < (*dptr)->h; y++) {
    for (int x = 0; x < (*dptr)->w; x++) {
      int y_offset      = y * (*dptr)->pitch;
      int x_offset      = x * bpp;
      u8* current_pixel = pixel + y_offset + x_offset;

      u32 pix_colour = *(u32*)current_pixel;
      /*Assigning a pointer to pix colour with a type conversion*/
      SDL_Color* colour = (SDL_Color*)&pix_colour;

      if (colour->r == 255) {
        colour->r = 189;
      }

      if (colour->g == 255) {
        colour->g = 147;
      }

      if (colour->b == 255) {
        colour->b = 249;
      }

      *(u32*)current_pixel = pix_colour;
    }
  }
}

void
set_rect(SDL_Rect* rect_ptr, SDL_Surface* surf, int x, int y) {
  if (surf != NULL) {
    SDL_Rect tmp = { x, y, surf->w, surf->h };
    *rect_ptr    = tmp;
    return;
  }

  rect_ptr->x = x;
  rect_ptr->y = y;
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

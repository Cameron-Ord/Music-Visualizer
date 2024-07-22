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
convert_pixel_colours(SDL_Surface** dptr, SDL_Color from_color, SDL_Color to_colour) {
  if (dptr == NULL) {
    return;
  }

  int bpp   = (*dptr)->format->BytesPerPixel;
  u8* pixel = (u8*)(*dptr)->pixels;

  for (int y = 0; y < (*dptr)->h; y++) {
    for (int x = 0; x < (*dptr)->w; x++) {
      int y_offset      = y * (*dptr)->pitch;
      int x_offset      = x * bpp;
      u8* current_pixel = pixel + y_offset + x_offset;

      u32 pix_colour = *(u32*)current_pixel;
      /*Assigning a pointer to pix colour with a type conversion*/
      SDL_Color* colour = (SDL_Color*)&pix_colour;

      if (colour->r == from_color.r) {
        colour->r = to_colour.r;
      }

      if (colour->g == from_color.g) {
        colour->g = to_colour.g;
      }

      if (colour->b == from_color.b) {
        colour->b = to_colour.b;
      }

      *(u32*)current_pixel = pix_colour;
    }
  }
}

void
set_rect(SDL_Rect* rect_ptr, int x, int y, int size_w, int size_h) {
  SDL_Rect tmp = { x, y, size_w, size_h };
  *rect_ptr    = tmp;
}

SDL_Texture*
create_image_texture(SDL_Renderer* r, SDL_Surface* surf_ptr) {
  SDL_Texture* tex_ptr = SDL_CreateTextureFromSurface(r, surf_ptr);
  if (!tex_ptr) {
    destroy_surface(surf_ptr);
    return NULL;
  }

  return tex_ptr;
}

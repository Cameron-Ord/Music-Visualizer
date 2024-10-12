#ifndef FONT_DEF
#define FONT_DEF

#include <stdbool.h>
#include <SDL2/SDL_render.h>

typedef struct {
  SDL_Texture *tex;
  SDL_Surface *surf;
  SDL_Rect rect;
  int width;
  int height;
  size_t id;
  bool is_valid;
  char *name;
} Text;

typedef struct {
  Text **buf;
  size_t size;
} TextBuffer;

#endif // FONT_DEF_H

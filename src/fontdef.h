#ifndef FONT_DEF
#define FONT_DEF

#include <stdbool.h>
#include <SDL2/SDL_render.h>

//If this is ever heap allocated, it is to be copied to a buffer and freed.
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

//TextBuffer is heap allocated
typedef struct {
  //Text is heap allocated
  Text *buf;
  size_t size;
} TextBuffer;

#endif // FONT_DEF_H

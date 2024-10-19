#ifndef FONT_DEF
#define FONT_DEF

#include <SDL2/SDL_render.h>
#include <stdbool.h>

// TextBuffer is heap allocated and pointed to in TextBuffer
typedef struct {
  SDL_Texture *tex;
  SDL_Surface *surf;
  SDL_Rect rect;
  int width;
  int height;
  size_t id;
  bool is_valid;
  const char *name;
} Text;

// TextBuffer is heap allocated
typedef struct {
  Text *text;
} TextBuffer;

#endif // FONT_DEF_H

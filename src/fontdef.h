#ifndef FONT_DEF
#define FONT_DEF

#include <SDL2/SDL_render.h>
#include <stdbool.h>

// TextBuffer is heap allocated and pointed to in TextBuffer
typedef struct {
  SDL_Texture *tex[2];
  SDL_Surface *surf[2];
  SDL_Rect rect;
  int width;
  int height;
  size_t id;
  bool is_valid;
  char *name;
} Text;

// TextBuffer is heap allocated
typedef struct {
  Text *text;
} TextBuffer;

#endif // FONT_DEF_H
